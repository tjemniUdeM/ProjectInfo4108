 /*
 * Fichier : affichage.cpp
 * Rôle :
 * Ce fichier contient la fonction responsable de l'affichage textuel
 * de la grille du jeu. Il affiche les deux joueurs, les cases spéciales X,
 * les cases vides et les positions actuelles des pions.
 */

#include "affichage.hpp"

#include <iostream>
#include <iomanip>

void afficherGrille(const EtatJeu& etat) {
    std::cout << "\n//////////////// GRILLE ////////////////\n\n";

    /*
     * La grille est affichée de haut en bas.
     * On commence donc par y = 11 jusqu'à y = 1 pour que l'affichage
     * ressemble à un vrai repère avec l'origine en bas à gauche.
     */
    for (int y = TAILLE_GRILLE; y >= 1; y--) {
        std::cout << std::setw(2) << y << " | ";

        for (int x = 1; x <= TAILLE_GRILLE; x++) {
            /*
             * Chaque case est affichée selon son contenu :
             * B : les deux joueurs sont sur la même case
             * 1 : joueur 1
             * 2 : joueur 2
             * X : case spéciale
             * _ : case vide
             */
            if (etat.p1x == x && etat.p1y == y && etat.p2x == x && etat.p2y == y) {
                std::cout << "B ";
            } else if (etat.p1x == x && etat.p1y == y) {
                std::cout << "1 ";
            } else if (etat.p2x == x && etat.p2y == y) {
                std::cout << "2 ";
            } else if (estCaseX(x, y)) {
                std::cout << "X ";
            } else {
                std::cout << "_ ";
            }
        }

        std::cout << std::endl;
    }

    // Affichage de l'axe horizontal pour faciliter la lecture des coordonnées.
    std::cout << "    ";
    for (int x = 1; x <= TAILLE_GRILLE; x++) {
        std::cout << "--";
    }

    std::cout << "\n     ";
    for (int x = 1; x <= TAILLE_GRILLE; x++) {
        std::cout << x % 10 << " ";
    }

    // Affichage des positions actuelles des deux joueurs.
    std::cout << "\n\nPosition joueur 1 : (" << etat.p1x << "," << etat.p1y << ")" << std::endl;
    std::cout << "Position joueur 2 : (" << etat.p2x << "," << etat.p2y << ")" << std::endl;

    // Si la partie est terminée, on affiche le gagnant.
    if (etat.finPartie) {
        std::cout << "\nPartie terminee. Gagnant : joueur " << etat.gagnant << std::endl;
    }

    std::cout << "\n////////////////////////////////////////\n";
}