// compteur.c
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "compteur.h"
#include <locale.h>

// Fonction pour ouvrir un fichier en mode lecture
FILE* ouvrirFichierLecture(const char* chemin) {
    if (chemin == NULL) {
        perror("Chemin de fichier NULL"); // Affiche un message d'erreur
        return NULL; // Retourne NULL si l'ouverture échoue
    }
    FILE* fichier = fopen(chemin, "r");
    if (fichier == NULL) {
        perror("Erreur d'ouverture du fichier");
        return NULL;
    }
    return fichier;
}

// Fonction pour compter le nombre de lignes dans un fichier
int compterLignes(FILE* fichier) {
    int lignes = 0;
    int c;

    // Revenir au début du fichier
    rewind(fichier);

    // Compter les lignes en fonction des sauts de ligne
    while ((c = fgetc(fichier)) != EOF) {
        if (c == '\n') {
            lignes++;
        }
    }

    // Compter la dernière ligne si le fichier ne se termine pas par un saut de ligne
    if (c != '\n' && lignes > 0) {
        lignes++;
    }

    return lignes;
}

// Fonction pour compter le nombre de mots dans un fichier
int compterMots(FILE* fichier) {
    if (fichier == NULL) return 0;
    int mots = 0;
    int dansMot = 0;
    int c, dernierCaractere = 0;

    // Revenir au début du fichier
    rewind(fichier);

    // Lire le fichier caractère par caractère pour détecter les mots
    while ((c = fgetc(fichier)) != EOF) {
        // Ignorer la ponctuation
        if (ispunct(c)) {
            continue; // Passe au caractère suivant si c'est une ponctuation
        }

        if (isspace(c)) {
            if (dansMot) {
                // Si le dernier caractère est une ponctuation, ne compte pas ce mot
                if (ispunct(dernierCaractere)) {
                    mots--; // Décrémente le mot compté à tort
                }   
            }
            dansMot = 0; // Fin d'un mot
        } else if (!dansMot) {
            dansMot = 1; // Début d'un nouveau mot
            mots++;
        }

        dernierCaractere = c; // Mettre à jour le dernier caractère
    }

    return mots;
}

// Fonction pour compter le nombre de caractères (sans espaces) dans un fichier
int compterCaracteres(FILE* fichier) {
    int caracteres = 0;
    int c;

    // Revenir au début du fichier
    rewind(fichier);

    // Lire le fichier caractère par caractère
    while ((c = fgetc(fichier)) != EOF) {
        // Compter seulement si ce n'est pas un caractère d'espacement
        if (!isspace(c)) {
            caracteres++;
        }
    }

    return caracteres;
}

// Fonction pour nettoyer un mot (supprimer les caractères non alphabétiques en début et fin)
void nettoyerMot(char* mot) {
    int longueur = strlen(mot);
    if (longueur == 0) return;

    int i = 0, j = 0;

    // Supprimer les caractères non alphabétiques au début et à la fin
    while (i < longueur && !isalpha(mot[i])) i++; // Ignorer le début non-alphabétique
    while (longueur > 0 && !isalpha(mot[longueur - 1])) longueur--; // Ignorer la fin non-alphabétique

    // Copier uniquement les lettres restantes
    while (i < longueur) {
        if (isalpha(mot[i])) {
            mot[j++] = tolower(mot[i]);
        }
        i++;
    }

    mot[j] = '\0'; // Terminer la chaîne

    // Si le mot est vide ou ne contient que de la ponctuation, le rendre vide
    if (j == 0) {
        mot[0] = '\0';
    }
}

// Fonction pour vérifier si un mot est un palindrome
int estPalindrome(const char *mot) {
    int gauche = 0;
    int droite = strlen(mot) - 1;

    // Vérifier si le mot contient exactement une seule lettre
    int compteurLettres = 0;
    for (int i = 0; mot[i] != '\0'; i++) {
        if (isalpha(mot[i])) {
            compteurLettres++;
        }
    }
    if (compteurLettres == 1) {
        return 0; // Pas un palindrome si une seule lettre
    }
    while (gauche < droite) {
        // Ignorer les caractères non alphabétiques
        while (gauche < droite && !isalpha(mot[gauche])) gauche++;
        while (gauche < droite && !isalpha(mot[droite])) droite--;

        // Comparer les caractères en minuscule
        if (tolower(mot[gauche]) != tolower(mot[droite])) {
            return 0; // Pas un palindrome
        }

        gauche++;
        droite--;
    }

    return 1; // C'est un palindrome
}

// Fonction pour ajouter un mot ou incrémenter sa fréquence
void ajouterMotOuIncrementer(char* mot, struct Mot** tableauMots, int* nombreMots, int* capaciteMots) {
    if (mot == NULL || tableauMots == NULL || nombreMots == NULL || capaciteMots == NULL) return;

    // Chercher si le mot existe déjà
    for (int i = 0; i < *nombreMots; i++) {
        if (strcmp((*tableauMots)[i].mot, mot) == 0) {
            (*tableauMots)[i].frequence++;
            
            if (estPalindrome(mot)) {
                (*tableauMots)[i].isPalindrome = 1;
            }
            return;
        }
    }
    // Si le tableau est plein, réallouer
    if (*nombreMots >= *capaciteMots) {
        *capaciteMots *= 2;
        struct Mot* temp = realloc(*tableauMots, (*capaciteMots) * sizeof(struct Mot));
        if (temp == NULL) {
            fprintf(stderr, "Erreur de réallocation mémoire\n");
            return;
        }
        *tableauMots = temp;
    }
    // Ajouter le nouveau mot
    (*tableauMots)[*nombreMots].mot = strdup(mot);
    if ((*tableauMots)[*nombreMots].mot == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        return;
    }
    (*tableauMots)[*nombreMots].frequence = 1;
    (*tableauMots)[*nombreMots].isPalindrome = estPalindrome(mot); // Initialiser isPalindrome
    (*nombreMots)++;
}

// Fonction pour vérifier si un caractère est alphabétique en UTF-8
int estLettreUTF8(const unsigned char* c) {
    if (*c < 128) {
        // ASCII standard
        return isalpha(*c);
    } else {
        // Caractères multioctets (UTF-8)
        return 1; // Supposons que tout caractère UTF-8 non-ASCII est valide
    }
}

// Vérifie si un caractère fait partie d'un mot (lettres + apostrophes internes)
int estCaractereDeMot(unsigned char* c, unsigned char* precedent, unsigned char* suivant) {
    if (*c < 128) {
        // ASCII : Lettres ou apostrophes internes
        return isalpha(*c) || (*c == '\'' && precedent && suivant && isalpha(*precedent) && isalpha(*suivant));
    } else {
        // UTF-8 : Tout caractère multi-octets est valide
        return 1;
    }
}

// Fonction pour convertir un mot en minuscules UTF-8
void convertirEnMinusculesUTF8(char* mot) {
    unsigned char* p = (unsigned char*)mot;
    while (*p) {
        if (*p < 128) {
            // ASCII standard
            *p = tolower(*p);
        }
        p++;
    }
}

// Fonction pour calculer la fréquence des mots
void calculerFrequenceMots(FILE* fichier, struct Mot** tableauMots, int* nombreMots) {
    if (fichier == NULL || tableauMots == NULL || nombreMots == NULL) return;

    setlocale(LC_ALL, ""); // Configurer la locale pour UTF-8

    char mot[100];
    char buffer[1024];
    int capaciteMots = 10;

    if (*tableauMots == NULL) {
        *tableauMots = malloc(capaciteMots * sizeof(struct Mot));
        if (*tableauMots == NULL) {
            fprintf(stderr, "Erreur d'allocation mémoire\n");
            return;
        }
        *nombreMots = 0;
    }

    while (fgets(buffer, sizeof(buffer), fichier)) {
        unsigned char* p = (unsigned char*)buffer;
        while (*p) {
            // Ignorer les caractères non alphabétiques et apostrophes mal placées
            while (*p && !estCaractereDeMot(p, p - 1, p + 1)) {
                p++;
            }

            // Début d'un mot
            unsigned char* debut = p;
            int len = 0;

            // Lire les caractères alphabétiques
            while (*p && estCaractereDeMot(p, p - 1, p + 1)) {
                len++;
                p++;
            }

            // Copier le mot trouvé
            if (len > 0) {
                strncpy(mot, (const char*)debut, len);
                mot[len] = '\0';

                // Convertir en minuscules
                convertirEnMinusculesUTF8(mot);
                // Ajouter ou incrémenter la fréquence
                ajouterMotOuIncrementer(mot, tableauMots, nombreMots, &capaciteMots);
            }
        }
    }
}

// Fonction pour compter les mots dans une phrase
int compterMotsDansPhrase(const char* phrase) {
    if (phrase == NULL) return 0;
    int mots = 0;
    int dansMot = 0;
    int c, dernierCaractere = 0;

    const char* p = phrase;

    while ((c = *p++) != '\0') {
        // Ignorer la ponctuation
        if (ispunct(c)) {
            continue;
        }

        if (isspace(c)) {
            if (dansMot) {
                // Si le dernier caractère est une ponctuation, ne compte pas ce mot
                if (ispunct(dernierCaractere)) {
                    mots--; // Décrémente le mot compté à tort
                }
                dansMot = 0; // Fin d'un mot
            }
        } else if (!dansMot) {
            dansMot = 1; // Début d'un nouveau mot
            mots++;
        }

        dernierCaractere = c; // Mettre à jour le dernier caractère
    }

    return mots;
}

// Fonction pour nettoyer une phrase (supprimer les espaces en début et en fin)
void nettoyerPhrase(char* phrase) {
    // Supprimer les espaces en début
    char* debut = phrase;
    while (isspace(*debut)) debut++;

    // Supprimer les espaces en fin
    char* fin = phrase + strlen(phrase) - 1;
    while (fin > debut && isspace(*fin)) fin--;
    *(fin + 1) = '\0';

    // Déplacer la phrase nettoyée au début du buffer
    if (debut != phrase) {
        memmove(phrase, debut, strlen(debut) + 1);
    }
}

// Fonction pour analyser les phrases
void analyserPhrases(FILE* fichier, struct Phrase** tableauPhrases, int* nombrePhrases, double* longueurMoyenne, struct Phrase** phrasePlusLongue, struct Phrase** phrasePlusCourte) {
    if (fichier == NULL || tableauPhrases == NULL || nombrePhrases == NULL || longueurMoyenne == NULL || phrasePlusLongue == NULL || phrasePlusCourte == NULL) {
        return;
    }

    // Réinitialiser le fichier
    rewind(fichier);

    // Variables pour la lecture
    char c;
    char buffer[10000]; // Supposer que les phrases ne dépassent pas 10000 caractères
    int index = 0;
    int capacitePhrases = 10;
    *nombrePhrases = 0;
    *longueurMoyenne = 0.0;
    *phrasePlusLongue = NULL;
    *phrasePlusCourte = NULL;

    // Allocation initiale du tableau de phrases
    *tableauPhrases = malloc(capacitePhrases * sizeof(struct Phrase));
    if (*tableauPhrases == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour les phrases\n");
        return;
    }

    while ((c = fgetc(fichier)) != EOF) {
        if (index < sizeof(buffer) - 1) { // Prévenir le dépassement de tampon
            buffer[index++] = c;
        }

        // Si le caractère est un délimiteur de phrase
        if (c == '.' || c == '!' || c == '?') {
            buffer[index] = '\0'; // Terminer la chaîne

            // Nettoyer la phrase
            nettoyerPhrase(buffer);

            // Duplication pour le comptage des mots
            char* phraseTexte = strdup(buffer);
            if (phraseTexte == NULL) {
                fprintf(stderr, "Erreur d'allocation mémoire pour une phrase\n");
                libererTableauPhrases(*tableauPhrases, *nombrePhrases);
                return;
            }

            // Compter les mots dans la phrase en utilisant la nouvelle fonction
            int motsDansPhrase = compterMotsDansPhrase(phraseTexte);
            free(phraseTexte);

            // Vérifier que la phrase contient au moins un mot
            if (motsDansPhrase > 0) {
                // Réallouer le tableau si nécessaire
                if (*nombrePhrases >= capacitePhrases) {
                    capacitePhrases *= 2;
                    struct Phrase* temp = realloc(*tableauPhrases, capacitePhrases * sizeof(struct Phrase));
                    if (temp == NULL) {
                        fprintf(stderr, "Erreur de réallocation mémoire pour les phrases\n");
                        libererTableauPhrases(*tableauPhrases, *nombrePhrases);
                        return;
                    }
                    *tableauPhrases = temp;
                }

                // Stocker la phrase
                (*tableauPhrases)[*nombrePhrases].texte = strdup(buffer);
                if ((*tableauPhrases)[*nombrePhrases].texte == NULL) {
                    fprintf(stderr, "Erreur d'allocation mémoire pour le texte de la phrase\n");
                    libererTableauPhrases(*tableauPhrases, *nombrePhrases);
                    return;
                }
                (*tableauPhrases)[*nombrePhrases].longueur = motsDansPhrase;
                (*nombrePhrases)++;
                *longueurMoyenne += motsDansPhrase;

                // Identifier la phrase la plus longue et la plus courte
                if (*phrasePlusLongue == NULL || motsDansPhrase > (*phrasePlusLongue)->longueur) {
                    *phrasePlusLongue = &(*tableauPhrases)[*nombrePhrases - 1];
                }
                if (*phrasePlusCourte == NULL || motsDansPhrase < (*phrasePlusCourte)->longueur) {
                    *phrasePlusCourte = &(*tableauPhrases)[*nombrePhrases - 1];
                }
            }

            // Réinitialiser le buffer
            index = 0;
            memset(buffer, 0, sizeof(buffer));
        }
    }

    // Traiter la dernière phrase si le fichier ne se termine pas par un délimiteur
    if (index > 0) {
        buffer[index] = '\0';
        nettoyerPhrase(buffer);

        char* phraseTexte = strdup(buffer);
        if (phraseTexte != NULL) {
            int motsDansPhrase = compterMotsDansPhrase(phraseTexte);
            free(phraseTexte);

            if (motsDansPhrase > 0) {
                if (*nombrePhrases >= capacitePhrases) {
                    capacitePhrases *= 2;
                    struct Phrase* temp = realloc(*tableauPhrases, capacitePhrases * sizeof(struct Phrase));
                    if (temp != NULL) {
                        *tableauPhrases = temp;
                    } else {
                        fprintf(stderr, "Erreur de réallocation mémoire pour les phrases\n");
                        libererTableauPhrases(*tableauPhrases, *nombrePhrases);
                        return;
                    }
                }

                (*tableauPhrases)[*nombrePhrases].texte = strdup(buffer);
                if ((*tableauPhrases)[*nombrePhrases].texte != NULL) {
                    (*tableauPhrases)[*nombrePhrases].longueur = motsDansPhrase;
                    (*nombrePhrases)++;
                    *longueurMoyenne += motsDansPhrase;

                    if (*phrasePlusLongue == NULL || motsDansPhrase > (*phrasePlusLongue)->longueur) {
                        *phrasePlusLongue = &(*tableauPhrases)[*nombrePhrases - 1];
                    }
                    if (*phrasePlusCourte == NULL || motsDansPhrase < (*phrasePlusCourte)->longueur) {
                        *phrasePlusCourte = &(*tableauPhrases)[*nombrePhrases - 1];
                    }
                }
            }
        }
    }

    // Calculer la longueur moyenne
    if (*nombrePhrases > 0) {
        *longueurMoyenne /= *nombrePhrases;
    }
}

// Fonction pour libérer la mémoire allouée pour les mots
void libererTableauMots(struct Mot* tableauMots, int nombreMots) {
    if (tableauMots == NULL) return;

    for (int i = 0; i < nombreMots; i++) {
        free(tableauMots[i].mot);
    }
    free(tableauMots);
}

// Fonction pour libérer la mémoire allouée pour les phrases
void libererTableauPhrases(struct Phrase* tableauPhrases, int nombrePhrases) {
    if (tableauPhrases == NULL) return;

    for (int i = 0; i < nombrePhrases; i++) {
        free(tableauPhrases[i].texte);
    }
    free(tableauPhrases);
}

// Fonction pour sauvegarder les résultats, incluant les analyses de phrases
void sauvegarderResultats(const char* cheminSortie,
                           int nombreLignes,
                           int nombreMots,
                           int nombreCaracteres,
                           struct Mot* tableauMots,
                           int nombreMotsDistincts,
                           struct Phrase* tableauPhrases,
                           int nombrePhrases,
                           double longueurMoyenne,
                           struct Phrase* phrasePlusLongue,
                           struct Phrase* phrasePlusCourte) {
    if (cheminSortie == NULL || tableauMots == NULL || nombreMotsDistincts <= 0) {
        fprintf(stderr, "Erreur : paramètres invalides pour la sauvegarde\n");
        return;
    }

    FILE* fichierSortie = fopen(cheminSortie, "w");
    if (fichierSortie == NULL) {
        perror("Erreur lors de l'ouverture du fichier de sortie");
        return;
    }

    fprintf(fichierSortie, "Résultats de l'analyse de texte :\n");
    fprintf(fichierSortie, "------------------------------\n");
    fprintf(fichierSortie, "Nombre de lignes : %d\n", nombreLignes);
    fprintf(fichierSortie, "Nombre de mots : %d\n", nombreMots);
    fprintf(fichierSortie, "Nombre de caractères : %d\n", nombreCaracteres);
    fprintf(fichierSortie, "Nombre de mots distincts : %d\n\n", nombreMotsDistincts);

    fprintf(fichierSortie, "Fréquence des mots :\n");
    fprintf(fichierSortie, "-------------------\n");
    for (int i = 0; i < nombreMotsDistincts; i++) {
        if (tableauMots[i].mot != NULL) {
            fprintf(fichierSortie, "%s : %d : %d\n", tableauMots[i].mot, tableauMots[i].frequence, tableauMots[i].isPalindrome);
        }
    }

    // Ajouter l'analyse des phrases
    fprintf(fichierSortie, "\nAnalyse des Phrases :\n");
    fprintf(fichierSortie, "----------------------\n");
    fprintf(fichierSortie, "Nombre de phrases : %d\n", nombrePhrases);
    if (nombrePhrases > 0) {
        fprintf(fichierSortie, "Longueur moyenne des phrases : %.2f mots\n", longueurMoyenne);
        if (phrasePlusLongue != NULL) {
            fprintf(fichierSortie, "Phrase la plus longue (%d mots) : %s\n", phrasePlusLongue->longueur, phrasePlusLongue->texte);
        }
        if (phrasePlusCourte != NULL) {
            fprintf(fichierSortie, "Phrase la plus courte (%d mots) : %s\n", phrasePlusCourte->longueur, phrasePlusCourte->texte);
        }
    }

    fclose(fichierSortie);
    printf("Résultats sauvegardés dans %s\n", cheminSortie);
}
