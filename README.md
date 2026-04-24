# Projet INFO4108 - Jeu de d'Alembert

## Description

Ce projet a été réalisé dans le cadre du cours **INFO4108 - Programmation Unix**.

Le but du projet est de programmer le jeu de d'Alembert en **C++ sous Linux**.  
Le programme utilise les notions de programmation système Unix, principalement :

- `fork()` pour créer deux processus fils ;
- `pipe()` pour créer des tubes de communication ;
- `read()` et `write()` pour échanger les données entre processus ;
- `waitpid()` pour attendre la fin des processus fils.

Chaque processus fils représente un joueur. Les deux joueurs se déplacent sur une grille de 11 × 11 cases selon des valeurs tirées aléatoirement entre -2 et 2.

---

## Structure du projet

```text
projet_dalembert/
├── main.cpp
├── jeu.hpp
├── jeu.cpp
├── affichage.hpp
├── affichage.cpp
├── score.hpp
├── score.cpp
├── Makefile
└── README.md
