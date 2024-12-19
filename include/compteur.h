// compteur.h
#ifndef COMPTEUR_H
#define COMPTEUR_H

#include <stdio.h>

// Structure pour stocker un mot et sa fréquence
struct Mot {
    char* mot;
    int frequence;
    int isPalindrome; 
};

// Structure pour stocker les informations des phrases
struct Phrase {
    char* texte;
    int longueur; // Nombre de mots dans la phrase
};

// Fonction pour ouvrir un fichier en mode lecture
FILE* ouvrirFichierLecture(const char* chemin);

// Fonction pour compter le nombre de lignes dans un fichier
int compterLignes(FILE* fichier);

// Fonction pour compter le nombre de mots dans un fichier
int compterMots(FILE* fichier);

// Fonction pour compter le nombre de caractères (sans espaces) dans un fichier
int compterCaracteres(FILE* fichier);

// Fonction pour nettoyer un mot (supprimer les caractères non alphabétiques en début et fin)
void nettoyerMot(char* mot);

// Vérifie si un mot est un palindrome
int estPalindrome(const char* mot);

// Fonction pour ajouter un mot ou incrémenter sa fréquence
void ajouterMotOuIncrementer(char* mot, struct Mot** tableauMots, int* nombreMots, int* capaciteMots);

// Fonction pour vérifier si un caractère est alphabétique en UTF-8
int estLettreUTF8(const unsigned char* c);

// Vérifie si un caractère fait partie d'un mot (lettres + apostrophes internes)
int estCaractereDeMot(unsigned char* c, unsigned char* precedent, unsigned char* suivant);

// Fonction pour convertir un mot en minuscules UTF-8
void convertirEnMinusculesUTF8(char* mot);

// Fonction pour calculer la fréquence des mots
void calculerFrequenceMots(FILE* fichier, struct Mot** tableauMots, int* nombreMots);

// Fonction pour compter les mots dans une chaîne de caractères selon la logique de compterMots
int compterMotsDansPhrase(const char* phrase);

// Fonction pour nettoyer une phrase (supprimer les espaces en début et en fin)
void nettoyerPhrase(char* phrase);

// Fonction pour analyser les phrases
void analyserPhrases(FILE* fichier, struct Phrase** tableauPhrases, int* nombrePhrases, double* longueurMoyenne, struct Phrase** phrasePlusLongue, struct Phrase** phrasePlusCourte);

// Libération de la mémoire allouée pour les mots
void libererTableauMots(struct Mot* tableauMots, int nombreMots);

// Libération de la mémoire allouée pour les phrases
void libererTableauPhrases(struct Phrase* tableauPhrases, int nombrePhrases);

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
                           struct Phrase* phrasePlusCourte);

#endif // COMPTEUR_H
