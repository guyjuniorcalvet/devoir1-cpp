#ifndef DOSSIERPROFESSEUR_H
#define DOSSIERPROFESSEUR_H

#include "Structures.h"
#include <string>
#include <iostream>

/**
 * @class DossierProfesseur
 * @brief Gère une liste chaînée de professeurs.
 *
 * Cette classe est responsable de la création, de la manipulation et de la destruction
 * d'une liste de professeurs. Chaque professeur a une liste de cours souhaités et
 * une liste d'étudiants dirigés.
 * La classe peut lire les données depuis un fichier, effectuer des opérations
 * sur la liste (suppression, recherche), et sauvegarder les modifications.
 */
class DossierProfesseur {
private:
    Professeur* tete; // Pointeur vers le premier professeur de la liste.

    // --- Fonctions utilitaires privées ---

    /**
     * @brief Détruit une liste chaînée de cours.
     * @param teteCours Pointeur vers la tête de la liste de cours à détruire.
     */
    void detruireListeCours(Cours*& teteCours);

    /**
     * @brief Détruit une liste chaînée d'étudiants.
     * @param teteEtudiants Pointeur vers la tête de la liste d'étudiants à détruire.
     */
    void detruireListeEtudiants(Etudiant*& teteEtudiants);

public:
    /**
     * @brief Constructeur.
     * Charge les données des professeurs depuis un fichier pour construire la liste chaînée.
     * @param nomFichierPF Nom du fichier contenant les données des professeurs.
     */
    explicit DossierProfesseur(const std::string& nomFichierPF);

    /**
     * @brief Destructeur.
     * Libère toute la mémoire allouée pour la liste chaînée de professeurs,
     * y compris les listes de cours et d'étudiants associées.
     */
    ~DossierProfesseur();

    /**
     * @brief Supprime un ou plusieurs professeurs par nom.
     * @param nom Le nom du ou des professeurs à supprimer.
     */
    void supprimerProfesseur(const std::string& nom);

    /**
     * @brief Trouve le nom du professeur ayant le plus grand nombre d'étudiants.
     * En cas d'égalité, le professeur avec la plus petite ancienneté est choisi.
     * @return Le nom du professeur.
     */
    std::string trouverProfAvecPlusDEtudiants() const;

    /**
     * @brief Trouve le sigle du cours le plus demandé par les professeurs.
     * En cas d'égalité, le cours associé au professeur ayant la plus faible ancienneté est choisi.
     * @return Le sigle du cours.
     */
    std::string trouverCoursLePlusDemande() const;

    /**
     * @brief Compte combien de professeurs souhaitent donner un cours spécifique.
     * @param sigleCours Le sigle du cours à rechercher.
     * @return Le nombre de professeurs.
     */
    int compterProfsPourUnCours(const std::string& sigleCours) const;

    /**
     * @brief Sauvegarde l'état actuel de la liste de professeurs dans un fichier.
     * @param nomFichierPF Le nom du fichier où sauvegarder les données.
     */
    void sauvegarderDansFichier(const std::string& nomFichierPF) const;

    /**
     * @brief Affiche le contenu complet de la liste de professeurs.
     * Utile pour le débogage.
     */
    void afficherContenu() const;
};

#endif // DOSSIERPROFESSEUR_H
