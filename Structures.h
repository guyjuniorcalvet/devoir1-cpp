#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>

// Structure pour représenter un étudiant
struct Etudiant {
    std::string nom;
    Etudiant* apres; // Pointeur vers l'étudiant suivant
    
    // Constructeur pour faciliter la création d'un étudiant
    Etudiant(const std::string& n) : nom(n), apres(nullptr) {}
};

// Structure pour représenter un cours
struct Cours {
    std::string sigle;
    Cours* suivant; // Pointeur vers le cours suivant
    
    // Constructeur pour faciliter la création d'un cours
    Cours(const std::string& s) : sigle(s), suivant(nullptr) {}
};

// Structure pour représenter un professeur
struct Professeur {
    std::string nom;
    int anciennete;
    Cours* listecours;         // Pointeur vers la liste chaînée des cours du professeur
    Etudiant* listetudiants;   // Pointeur vers la liste chaînée des étudiants dirigés par le professeur
    Professeur* suivant;       // Pointeur vers le professeur suivant dans la liste principale
    
    // Constructeur pour faciliter la création d d'un professeur
    Professeur(const std::string& n, int a) 
        : nom(n), anciennete(a), listecours(nullptr), listetudiants(nullptr), suivant(nullptr) {}
};

#endif // STRUCTURES_H