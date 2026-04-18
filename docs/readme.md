# Analyseur de fichiers texte en C

Application en **C** avec interface graphique **GTK3** permettant d’analyser des fichiers `.txt`.

## Description

Outil simple pour obtenir rapidement des statistiques sur un texte :

- nombre de lignes, mots et caractères  
- fréquence des mots  
- détection des palindromes  
- analyse des phrases (nombre, moyenne, plus longue, plus courte)

Les résultats sont affichés et peuvent être sauvegardés.

## Prérequis

- `gcc`
- MSYS2 / MinGW
- GTK3 :

```sh
pacman -S mingw-w64-x86_64-gtk3
```
