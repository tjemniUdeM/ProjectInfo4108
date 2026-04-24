 /*
 * Fichier : jeu.cpp
 * Rôle :
 * Ce fichier contient les fonctions principales du jeu.
 * Il gère les règles de déplacement, les cases spéciales, la lecture/écriture
 * dans les tubes avec read() et write(), ainsi que le comportement de chaque joueur.
 */

#include "jeu.hpp"
#include "affichage.hpp"

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <random>
#include <ctime>
#include <chrono>
#include <thread>
#include <string>

// Vérifie si une position est valide dans la grille 11x11.
bool estDansGrille(int x, int y) {
    return x >= 1 && x <= TAILLE_GRILLE && y >= 1 && y <= TAILLE_GRILLE;
}

// Vérifie si la case donnée est une case spéciale X.
bool estCaseX(int x, int y) {
    return (x == 10 && y == 10) ||
           (x == 3  && y == 9)  ||
           (x == 6  && y == 8)  ||
           (x == 8  && y == 5)  ||
           (x == 2  && y == 4)  ||
           (x == 6  && y == 2)  ||
           (x == 10 && y == 2);
}

// Initialise l'état du jeu au début d'une partie.
void initialiserEtat(EtatJeu& etat) {
    etat.p1x = 6;
    etat.p1y = 6;
    etat.p2x = 6;
    etat.p2y = 6;
    etat.finPartie = 0;
    etat.gagnant = 0;
    etat.tour = 1;
}

/*
 * Cette fonction utilise read() pour lire complètement une structure envoyée dans un tube.
 * On utilise une boucle parce que read() ne garantit pas toujours de recevoir tous les octets d'un coup.
 */
bool lectureComplete(int fd, void* buffer, unsigned long taille) {
    char* ptr = static_cast<char*>(buffer);
    unsigned long total = 0;

    while (total < taille) {
        ssize_t recu = read(fd, ptr + total, taille - total);

        if (recu <= 0) {
            return false;
        }

        total += recu;
    }

    return true;
}

/*
 * Cette fonction utilise write() pour envoyer complètement une structure dans un tube.
 * Comme pour read(), on utilise une boucle pour être sûr que tous les octets sont envoyés.
 */
bool ecritureComplete(int fd, const void* buffer, unsigned long taille) {
    const char* ptr = static_cast<const char*>(buffer);
    unsigned long total = 0;

    while (total < taille) {
        ssize_t envoye = write(fd, ptr + total, taille - total);

        if (envoye <= 0) {
            return false;
        }

        total += envoye;
    }

    return true;
}

// Pause simple pour laisser l'utilisateur suivre le jeu coup par coup.
void attendreEntree(int joueur) {
    std::cout << "\nAppuyez sur ENTREE pour faire jouer le joueur "
              << joueur << "...";
    std::cout.flush();

    std::string ligne;
    std::getline(std::cin, ligne);
}

// Tire un jeton au hasard entre -2 et 2.
static int tirerJeton(std::mt19937& generateur) {
    std::uniform_int_distribution<int> distribution(-2, 2);
    return distribution(generateur);
}

/*
 * Fonction qui applique un tour de jeu pour un joueur.
 * Elle tire dx et dy au hasard, calcule la nouvelle position,
 * applique les règles spéciales et vérifie s'il y a un gagnant.
 */
void jouerTour(EtatJeu& etat, int joueur) {
    attendreEntree(joueur);

    std::mt19937 generateur(static_cast<unsigned int>(time(nullptr) ^ getpid()));

    int dx = tirerJeton(generateur);
    int dy = tirerJeton(generateur);

    int* xActuel;
    int* yActuel;
    int* xAdverse;
    int* yAdverse;

    if (joueur == 1) {
        xActuel = &etat.p1x;
        yActuel = &etat.p1y;
        xAdverse = &etat.p2x;
        yAdverse = &etat.p2y;
    } else {
        xActuel = &etat.p2x;
        yActuel = &etat.p2y;
        xAdverse = &etat.p1x;
        yAdverse = &etat.p1y;
    }

    int nouveauX = *xActuel + dx;
    int nouveauY = *yActuel + dy;

    std::cout << "\nJoueur " << joueur << " joue." << std::endl;
    std::cout << "Tirage : dx = " << dx << ", dy = " << dy << std::endl;

    if (!estDansGrille(nouveauX, nouveauY)) {
        std::cout << "La case finale n'existe pas. Le pion ne bouge pas." << std::endl;
    } else if (estCaseX(nouveauX, nouveauY)) {
        std::cout << "Le joueur tombe sur une case X. Il retourne en (1,1)." << std::endl;
        *xActuel = 1;
        *yActuel = 1;
    } else {
        *xActuel = nouveauX;
        *yActuel = nouveauY;

        /*
         * Si le joueur arrive sur la case de l'adversaire,
         * l'adversaire est renvoyé à la case (1,1).
         */
        if (*xActuel == *xAdverse && *yActuel == *yAdverse) {
            std::cout << "Collision avec le joueur adverse. L'adversaire retourne en (1,1)." << std::endl;
            *xAdverse = 1;
            *yAdverse = 1;
        }

        /*
         * La partie se termine si un joueur arrive à (1,1) ou (11,11)
         * par un déplacement normal.
         */
        if ((*xActuel == 1 && *yActuel == 1) ||
            (*xActuel == 11 && *yActuel == 11)) {
            etat.finPartie = 1;
            etat.gagnant = joueur;
        }
    }

    afficherGrille(etat);
}

/*
 * Fonction exécutée par chaque processus fils.
 * Chaque joueur attend son tour, lit l'état du jeu depuis un tube,
 * joue, puis écrit le nouvel état dans l'autre tube.
 */
void processusJoueur(int joueur, int lectureFd, int ecritureFd, int resultatFd) {
    EtatJeu etat;

    /*
     * Le joueur 1 initialise la partie et commence le premier tour.
     */
    if (joueur == 1) {
        initialiserEtat(etat);
        afficherGrille(etat);
    }

    while (true) {
        if (joueur == 1) {
            jouerTour(etat, 1);

            // Envoie l'état du jeu au joueur 2 avec write().
            ecritureComplete(ecritureFd, &etat, sizeof(EtatJeu));

            if (etat.finPartie) {
                ResultatPartie resultat;
                resultat.gagnant = etat.gagnant;

                // Envoie le gagnant au processus père.
                ecritureComplete(resultatFd, &resultat, sizeof(ResultatPartie));
                break;
            }

            // Attend la réponse du joueur 2 avec read().
            if (!lectureComplete(lectureFd, &etat, sizeof(EtatJeu))) {
                break;
            }

            if (etat.finPartie) {
                break;
            }
        } else {
            // Le joueur 2 attend d'abord que le joueur 1 joue.
            if (!lectureComplete(lectureFd, &etat, sizeof(EtatJeu))) {
                break;
            }

            if (etat.finPartie) {
                break;
            }

            jouerTour(etat, 2);

            // Envoie l'état modifié au joueur 1.
            ecritureComplete(ecritureFd, &etat, sizeof(EtatJeu));

            if (etat.finPartie) {
                ResultatPartie resultat;
                resultat.gagnant = etat.gagnant;

                // Envoie le résultat final au processus père.
                ecritureComplete(resultatFd, &resultat, sizeof(ResultatPartie));
                break;
            }
        }
    }

    // Fermeture des descripteurs utilisés par le processus fils.
    close(lectureFd);
    close(ecritureFd);
    close(resultatFd);

    _exit(0);
}