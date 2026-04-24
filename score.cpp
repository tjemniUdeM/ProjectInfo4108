 /*
 * Fichier : score.cpp
 * Rôle :
 * Ce fichier gère le score des parties.
 * Il lit le score actuel dans le fichier score.txt, ajoute une victoire
 * au joueur gagnant, puis réécrit le nouveau score dans le fichier.
 */

#include "score.hpp"

#include <iostream>
#include <fstream>

/*
 * Cette fonction met à jour le fichier score.txt.
 * Si le fichier existe déjà, on lit les anciens scores.
 * Sinon, les scores commencent automatiquement à 0.
 */
void enregistrerScore(int gagnant) {
    int scoreJ1 = 0;
    int scoreJ2 = 0;

    std::ifstream lecture("score.txt");

    if (lecture.is_open()) {
        lecture >> scoreJ1 >> scoreJ2;
        lecture.close();
    }

    // Ajoute une victoire au joueur gagnant.
    if (gagnant == 1) {
        scoreJ1++;
    } else if (gagnant == 2) {
        scoreJ2++;
    }

    /*
     * On réécrit le fichier avec les nouveaux scores.
     * Le fichier contient simplement deux nombres :
     * score du joueur 1 puis score du joueur 2.
     */
    std::ofstream ecriture("score.txt");

    if (!ecriture.is_open()) {
        std::cerr << "Erreur : impossible d'ouvrir score.txt" << std::endl;
        return;
    }

    ecriture << scoreJ1 << " " << scoreJ2 << std::endl;
    ecriture.close();
}

/*
 * Cette fonction affiche le score enregistré dans score.txt.
 * Si le fichier n'existe pas encore, elle affiche 0 victoire pour chaque joueur.
 */
void afficherScore() {
    int scoreJ1 = 0;
    int scoreJ2 = 0;

    std::ifstream lecture("score.txt");

    if (lecture.is_open()) {
        lecture >> scoreJ1 >> scoreJ2;
        lecture.close();
    }

    std::cout << "\n========== SCORE ==========\n";
    std::cout << "Joueur 1 : " << scoreJ1 << " victoire(s)" << std::endl;
    std::cout << "Joueur 2 : " << scoreJ2 << " victoire(s)" << std::endl;
    std::cout << "===========================\n";
}