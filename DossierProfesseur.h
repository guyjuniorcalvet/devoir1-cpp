#ifndef DOSSIERPROFESSEUR_H
#define DOSSIERPROFESSEUR_H

#include "Structures.h" // Inclut les définitions de nos structures
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <limits> // Pour std::numeric_limits

class DossierProfesseur {
private:
    Professeur* tete; // Début de la liste chaînée des professeurs

    // Fonctions utilitaires privées pour la gestion des sous-listes
    void detruireListeCours(Cours*& teteCours);
    void detruireListeEtudiants(Etudiant*& teteEtudiants);
    
public:
    // Constructeur: Construit la liste chaînée à partir du fichier PF
    DossierProfesseur(const char* nomFichierPF);

    // Destructeur: Détruit la liste chaînée existante en mémoire
    ~DossierProfesseur();

    // Supprime de la liste chaînée tous les professeurs dont le nom est 'name'
    void supprimer(const std::string& name);

    // Affiche le nom du professeur ayant le plus d'étudiants
    // En cas de litige, choisir le premier professeur le moins ancien
    std::string afficherLeProfPlusEtudiant() const;

    // Affiche le cours le plus demandé de la liste
    // En cas de litige, choisir celui correspondant au (premier) professeur le moins ancien
    std::string afficherCoursPlusDemande() const;

    // Affiche le nombre de professeurs souhaitant enseigner le cours 'coursdonne'
    int affichernbreProfPourUnCours(const std::string& coursdonne) const;

    // Recopie la liste chaînée mise à jour dans le fichier PF
    void recopier(const char* nomFichierPF) const;

    // Fonction d'affichage pour le débogage (non demandée mais utile)
    void afficherTousLesProfesseurs() const;
};

#endif // DOSSIERPROFESSEUR_H