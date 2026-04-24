 /*
 * Fichier : main.cpp
 * Rôle :
 * Ce fichier contient le processus père du programme.
 * Il crée les tubes avec pipe(), crée deux processus fils avec fork(),
 * attend la fin de la partie avec waitpid(), puis enregistre le score.
 */

#include "jeu.hpp"
#include "score.hpp"

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>

int main() {
    char choix = 'o';

    while (choix == 'o' || choix == 'O') {
        int tubeJ1VersJ2[2];
        int tubeJ2VersJ1[2];
        int tubeResultat[2];

        /*
         * pipe() crée un tube de communication entre processus.
         * Ici, on utilise deux tubes pour permettre aux deux joueurs
         * de s'envoyer l'état du jeu chacun leur tour.
         */
        if (pipe(tubeJ1VersJ2) == -1) {
            std::cerr << "Erreur lors de la creation du tube J1 vers J2." << std::endl;
            return EXIT_FAILURE;
        }

        if (pipe(tubeJ2VersJ1) == -1) {
            std::cerr << "Erreur lors de la creation du tube J2 vers J1." << std::endl;
            return EXIT_FAILURE;
        }

        /*
         * Ce tube sert seulement à envoyer le résultat final
         * du processus gagnant vers le processus père.
         */
        if (pipe(tubeResultat) == -1) {
            std::cerr << "Erreur lors de la creation du tube resultat." << std::endl;
            return EXIT_FAILURE;
        }

        /*
         * fork() duplique le processus courant.
         * Après ce fork, on aura un processus fils qui représente le joueur 1.
         */
        pid_t pidJ1 = fork();

        if (pidJ1 < 0) {
            std::cerr << "Erreur lors du fork du joueur 1." << std::endl;
            return EXIT_FAILURE;
        }

        if (pidJ1 == 0) {
            /*
             * Dans le fils joueur 1, on ferme les extrémités des tubes
             * qui ne sont pas utilisées par ce processus.
             */
            close(tubeJ1VersJ2[0]);
            close(tubeJ2VersJ1[1]);
            close(tubeResultat[0]);

            processusJoueur(1, tubeJ2VersJ1[0], tubeJ1VersJ2[1], tubeResultat[1]);
        }

        /*
         * Deuxième fork : création du processus fils pour le joueur 2.
         */
        pid_t pidJ2 = fork();

        if (pidJ2 < 0) {
            std::cerr << "Erreur lors du fork du joueur 2." << std::endl;
            return EXIT_FAILURE;
        }

        if (pidJ2 == 0) {
            /*
             * Dans le fils joueur 2, on ferme aussi les extrémités
             * des tubes qu'il n'utilise pas.
             */
            close(tubeJ1VersJ2[1]);
            close(tubeJ2VersJ1[0]);
            close(tubeResultat[0]);

            processusJoueur(2, tubeJ1VersJ2[0], tubeJ2VersJ1[1], tubeResultat[1]);
        }

        /*
         * Le processus père ne joue pas.
         * Il ferme donc les tubes utilisés seulement pour la communication entre joueurs.
         */
        close(tubeJ1VersJ2[0]);
        close(tubeJ1VersJ2[1]);
        close(tubeJ2VersJ1[0]);
        close(tubeJ2VersJ1[1]);
        close(tubeResultat[1]);

        ResultatPartie resultat;

        /*
         * Le père lit le résultat final envoyé par le processus gagnant.
         * La fonction lectureComplete utilise read() pour recevoir les données du tube.
         */
        if (lectureComplete(tubeResultat[0], &resultat, sizeof(ResultatPartie))) {
            std::cout << "\nLe gagnant de la partie est le joueur "
                      << resultat.gagnant << "." << std::endl;

            enregistrerScore(resultat.gagnant);
        } else {
            std::cerr << "Erreur : aucun resultat recu." << std::endl;
        }

        close(tubeResultat[0]);

        /*
         * waitpid() permet au processus père d'attendre la fin des deux fils.
         * Cela évite de laisser des processus zombies.
         */
        waitpid(pidJ1, nullptr, 0);
        waitpid(pidJ2, nullptr, 0);

        afficherScore();

        std::cout << "\nVoulez-vous rejouer ? (o/n) : ";
        std::cin >> choix;
    }

    std::cout << "Fin du programme." << std::endl;

    return 0;
}