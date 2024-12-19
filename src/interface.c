// interface.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "compteur.h"
#include <ctype.h>
#include <windows.h>
#include <gtk/gtk.h>
#include <glib.h> // glib.h pour UTF-8

// Définition de la variable globale pour le bouton de sauvegarde
GtkWidget *save_button = NULL;

// Fonction de comparaison pour qsort
int comparerMots(const void *a, const void *b) {
    struct Mot *motA = (struct Mot *)a;
    struct Mot *motB = (struct Mot *)b;

    // Tri décroissant par fréquence
    if (motB->frequence != motA->frequence) {
        return motB->frequence - motA->frequence;
    } else {
        // Si les fréquences sont égales, trier par ordre alphabétique
        return strcmp(motA->mot, motB->mot);
    }
}

// Fonction pour sélectionner un fichier
static void on_select_file(GtkWidget *widget, gpointer data) {
    GtkFileChooser *file_chooser = GTK_FILE_CHOOSER(widget);
    gchar *filename = gtk_file_chooser_get_filename(file_chooser);

    // Gestion des erreurs
    if (filename == NULL) {
        gtk_label_set_text(GTK_LABEL(data), "Erreur : Aucun fichier sélectionné");
        return;
    }

    // Mettre à jour l'affichage des résultats
    mettreAJourResultats(GTK_WIDGET(data), filename);
    g_free(filename);
}

// Fonction pour ouvrir le dialogue d'enregistrement
void ouvrirDialogueEnregistrement(GtkWidget *parent, gpointer user_data) {
    // Déclaration de la structure pour la boîte de dialogue de sauvegarde de fichier
    OPENFILENAME ofn;
    char szFile[260] = { 0 };
    char filter[] = "Fichiers texte (*.txt)\0*.txt\0Tous les fichiers (*.*)\0*.*\0";

    // Initialisation de la structure OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

    // .txt par défaut
    ofn.lpstrDefExt = "txt";

    // Afficher la boîte de dialogue de sauvegarde
    if (GetSaveFileName(&ofn) == TRUE) {
        // Récupérer les données à partir de l'objet save_button
        int nombreLignes = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(save_button), "nombreLignes"));
        int nombreMots = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(save_button), "nombreMots"));
        int nombreCaracteres = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(save_button), "nombreCaracteres"));
        struct Mot* tableauMots = (struct Mot*)g_object_get_data(G_OBJECT(save_button), "tableauMots");
        int nombreMotsDistincts = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(save_button), "nombreMotsDistincts"));

        // Récupérer les données des phrases
        struct Phrase* tableauPhrases = (struct Phrase*)g_object_get_data(G_OBJECT(save_button), "tableauPhrases");
        int nombrePhrases = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(save_button), "nombrePhrases"));
        gpointer ptr_longueurMoyenne = g_object_get_data(G_OBJECT(save_button), "longueurMoyenne");
        double longueurMoyenne = 0.0;
        if (ptr_longueurMoyenne != NULL) {
            longueurMoyenne = *((double*)ptr_longueurMoyenne);
        }
        struct Phrase* phrasePlusLongue = (struct Phrase*)g_object_get_data(G_OBJECT(save_button), "phrasePlusLongue");
        struct Phrase* phrasePlusCourte = (struct Phrase*)g_object_get_data(G_OBJECT(save_button), "phrasePlusCourte");

        // Vérifiez que les données sont valides
        if (tableauMots == NULL || nombreMotsDistincts <= 0 || tableauPhrases == NULL || nombrePhrases <= 0) {
            MessageBox(NULL, "Erreur : Données invalides pour la sauvegarde.", "Erreur", MB_OK | MB_ICONERROR);
            return;
        }

        // Sauvegarder les résultats
        sauvegarderResultats(szFile, nombreLignes, nombreMots, nombreCaracteres, tableauMots, nombreMotsDistincts, tableauPhrases, nombrePhrases, longueurMoyenne, phrasePlusLongue, phrasePlusCourte);
    }
}

// Fonction pour créer la fenêtre principale
void creerFenetrePrincipale() {
    GtkWidget *fenetre;
    GtkWidget *vbox;
    GtkWidget *select_button;
    GtkWidget *label;
    GtkWidget *scrolled_window;

    // Créer la fenêtre
    fenetre = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(fenetre), "Analyseur de Texte");
    gtk_window_set_default_size(GTK_WINDOW(fenetre), 600, 400);

    // Créer une boîte verticale pour organiser les widgets
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(fenetre), vbox);

    // Créer un bouton pour sélectionner un fichier
    select_button = gtk_file_chooser_button_new("Sélectionnez un fichier", GTK_FILE_CHOOSER_ACTION_OPEN);
    // Filtre .txt
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.txt");
    gtk_file_filter_set_name(filter, "Fichiers texte");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(select_button), filter);
    gtk_box_pack_start(GTK_BOX(vbox), select_button, FALSE, FALSE, 0);

    // Créer une zone de défilement pour les résultats
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    // Créer un label pour afficher les résultats
    label = gtk_label_new("Résultats ici...");
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
    gtk_container_add(GTK_CONTAINER(scrolled_window), label);

    // Ajouter le bouton de sauvegarde
    save_button = gtk_button_new_with_label("Sauvegarder les résultats");
    gtk_widget_set_name(save_button, "save_button"); // Ajouter un nom au bouton de sauvegarde
    gtk_box_pack_start(GTK_BOX(vbox), save_button, FALSE, FALSE, 0);
    gtk_widget_set_sensitive(save_button, FALSE);

    // Connecter le signal de sélection de fichier
    g_signal_connect(select_button, "file-set", G_CALLBACK(on_select_file), label);

    // Connecter le signal de sauvegarde de fichier
    g_signal_connect_swapped(save_button, "clicked", G_CALLBACK(ouvrirDialogueEnregistrement), NULL);

    // Connecter le signal de fermeture de la fenêtre
    g_signal_connect(fenetre, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Afficher tous les widgets
    gtk_widget_show_all(fenetre);
}

// Fonction pour mettre à jour l'affichage des résultats
void mettreAJourResultats(GtkWidget *label, const char *filename) {
    // Vérifier l'extension .txt
    const char *extension = strrchr(filename, '.');
    if (extension == NULL || strcmp(extension, ".txt") != 0) {
        gtk_label_set_text(GTK_LABEL(label), "Erreur : Seuls les fichiers .txt sont acceptés");
        return;
    }

    FILE *fichier = ouvrirFichierLecture(filename);
    if (fichier == NULL) {
        gtk_label_set_text(GTK_LABEL(label), "Erreur lors de l'ouverture du fichier");
        return;
    }

    int lignes = 0, mots = 0, caracteres = 0;
    struct Mot *tableauMots = NULL;
    int nombreMots = 0;
    int capaciteMots = 10;

    // Compter les lignes
    lignes = compterLignes(fichier);
    rewind(fichier);

    // Compter les mots
    mots = compterMots(fichier);
    rewind(fichier);

    // Compter les caractères
    caracteres = compterCaracteres(fichier);
    rewind(fichier);

    // Allocation mémoire pour le tableau de mots
    tableauMots = malloc(sizeof(struct Mot) * capaciteMots);
    if (tableauMots == NULL) {
        gtk_label_set_text(GTK_LABEL(label), "Erreur d'allocation mémoire");
        fclose(fichier);
        return;
    }

    memset(tableauMots, 0, sizeof(struct Mot) * capaciteMots);

    // Calculer la fréquence des mots
    calculerFrequenceMots(fichier, &tableauMots, &nombreMots);
    rewind(fichier);

    // Trier le tableau de mots par fréquence décroissante
    qsort(tableauMots, nombreMots, sizeof(struct Mot), comparerMots);

    // Analyse des phrases
    struct Phrase* tableauPhrases = NULL;
    int nombrePhrases = 0;
    double longueurMoyenne = 0.0;
    struct Phrase* phrasePlusLongue = NULL;
    struct Phrase* phrasePlusCourte = NULL;

    analyserPhrases(fichier, &tableauPhrases, &nombrePhrases, &longueurMoyenne, &phrasePlusLongue, &phrasePlusCourte);
    // Fin de l'analyse des phrases

    // Vérifier que le bouton de sauvegarde est disponible
    if (save_button == NULL) {
        gtk_label_set_text(GTK_LABEL(label), "Erreur : bouton de sauvegarde non trouvé");
        fclose(fichier);
        libererTableauMots(tableauMots, nombreMots);
        libererTableauPhrases(tableauPhrases, nombrePhrases);
        return;
    }

    // Avant de stocker de nouvelles données, libérer les anciennes si elles existent
    struct Mot* old_tableauMots = (struct Mot*)g_object_get_data(G_OBJECT(save_button), "tableauMots");
    if (old_tableauMots != NULL) {
        int old_nombreMotsDistincts = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(save_button), "nombreMotsDistincts"));
        libererTableauMots(old_tableauMots, old_nombreMotsDistincts);
    }

    struct Phrase* old_tableauPhrases = (struct Phrase*)g_object_get_data(G_OBJECT(save_button), "tableauPhrases");
    if (old_tableauPhrases != NULL) {
        int old_nombrePhrases = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(save_button), "nombrePhrases"));
        libererTableauPhrases(old_tableauPhrases, old_nombrePhrases);
    }

    // Stocker les nouvelles données
    g_object_set_data(G_OBJECT(save_button), "nombreLignes", GINT_TO_POINTER(lignes));
    g_object_set_data(G_OBJECT(save_button), "nombreMots", GINT_TO_POINTER(mots));
    g_object_set_data(G_OBJECT(save_button), "nombreCaracteres", GINT_TO_POINTER(caracteres));
    g_object_set_data(G_OBJECT(save_button), "tableauMots", tableauMots);
    g_object_set_data(G_OBJECT(save_button), "nombreMotsDistincts", GINT_TO_POINTER(nombreMots));

    // Stocker les données des phrases
    double* p_longueurMoyenne = malloc(sizeof(double));
    if (p_longueurMoyenne != NULL) {
        *p_longueurMoyenne = longueurMoyenne;
        g_object_set_data_full(G_OBJECT(save_button), "longueurMoyenne", p_longueurMoyenne, g_free);
    }

    g_object_set_data(G_OBJECT(save_button), "tableauPhrases", tableauPhrases);
    g_object_set_data(G_OBJECT(save_button), "nombrePhrases", GINT_TO_POINTER(nombrePhrases));
    g_object_set_data(G_OBJECT(save_button), "phrasePlusLongue", phrasePlusLongue);
    g_object_set_data(G_OBJECT(save_button), "phrasePlusCourte", phrasePlusCourte);

    gtk_widget_set_sensitive(save_button, TRUE);

    // Construire la chaîne de résultats
    GString *resultats = g_string_new("");
    g_string_append_printf(resultats,
                           "Lignes : %d\nMots : %d\nCaractères : %d\n\nFréquence des mots :\n\n",
                           lignes, mots, caracteres);

    for (int i = 0; i < nombreMots; i++) {
        if (tableauMots[i].isPalindrome == 1){
            g_string_append_printf(resultats, "%s => { Occurences : %d | estPalindrome : Oui }\n\n", tableauMots[i].mot, tableauMots[i].frequence);
            continue;
        }
        g_string_append_printf(resultats, "%s => { Occurences : %d }\n\n", tableauMots[i].mot, tableauMots[i].frequence);
    }

    // Ajouter les résultats de l'analyse des phrases
    g_string_append_printf(resultats, "\nAnalyse des Phrases :\n");
    g_string_append_printf(resultats, "----------------------\n");
    g_string_append_printf(resultats, "Nombre de phrases : %d\n", nombrePhrases);
    if (nombrePhrases > 0) {
        g_string_append_printf(resultats, "Longueur moyenne des phrases : %.2f mots\n", longueurMoyenne);
        if (phrasePlusLongue != NULL) {
            g_string_append_printf(resultats, "Phrase la plus longue (%d mots) : %s\n", phrasePlusLongue->longueur, phrasePlusLongue->texte);
        }
        if (phrasePlusCourte != NULL) {
            g_string_append_printf(resultats, "Phrase la plus courte (%d mots) : %s\n", phrasePlusCourte->longueur, phrasePlusCourte->texte);
        }
    }

    // Afficher le texte en UTF-8
    gtk_label_set_text(GTK_LABEL(label), resultats->str);

    g_string_free(resultats, TRUE);

    fclose(fichier);
}
