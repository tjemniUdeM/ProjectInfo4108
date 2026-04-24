#ifndef JEU_HPP
#define JEU_HPP

#include <string>

const int TAILLE_GRILLE = 11;

struct EtatJeu {
    int p1x;
    int p1y;
    int p2x;
    int p2y;
    int finPartie;
    int gagnant;
    int tour;
};

struct ResultatPartie {
    int gagnant;
};

bool estDansGrille(int x, int y);
bool estCaseX(int x, int y);
void initialiserEtat(EtatJeu& etat);
void jouerTour(EtatJeu& etat, int joueur);
bool lectureComplete(int fd, void* buffer, unsigned long taille);
bool ecritureComplete(int fd, const void* buffer, unsigned long taille);
void processusJoueur(int joueur, int lectureFd, int ecritureFd, int resultatFd);

#endif