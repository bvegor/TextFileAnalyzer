// interface.h
#ifndef INTERFACE_H
#define INTERFACE_H

#include <gtk/gtk.h>
#include "compteur.h"

// Déclaration de la variable globale pour le bouton de sauvegarde
extern GtkWidget *save_button;

// Fonction de comparaison pour qsort
int comparerMots(const void *a, const void *b);

// Fonction pour sélectionner un fichier
static void on_select_file(GtkWidget *widget, gpointer data);

// Fonction pour créer et afficher la fenêtre principale
void creerFenetrePrincipale();

// Fonction pour mettre à jour l'affichage des résultats
void mettreAJourResultats(GtkWidget *label, const char *filename);

// Fonction pour ouvrir le dialogue d'enregistrement
void ouvrirDialogueEnregistrement(GtkWidget *parent, gpointer user_data);

#endif // INTERFACE_H
