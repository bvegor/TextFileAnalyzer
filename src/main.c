// main.c
#include <stdio.h>
#include <stdlib.h>
#include <locale.h> 
#include "compteur.h"
#include "interface.h"

int main(int argc, char *argv[]) {

    gtk_init(&argc, &argv);
    
    // Créer et afficher la fenêtre principale
    creerFenetrePrincipale();
    
    // Lancer la boucle principale de GTK
    gtk_main();

    return EXIT_SUCCESS;
}