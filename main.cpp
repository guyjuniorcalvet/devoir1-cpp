#include "DossierProfesseur.h"
#include <iostream>
#include <fstream>
#include <sstream>

// --- Prototypes des fonctions ---
void afficherTitre(const std::string& titre);
void executerTransactions(DossierProfesseur& dossier, const std::string& nomFichierTransactions, const std::string& nomFichierSauvegarde);

/**
 * @brief Point d'entrée principal du programme.
 *
 * Ce programme gère une base de données de professeurs et exécute une série de transactions
 * pour la mettre à jour.
 *
 * 1. Il charge les données initiales des professeurs depuis `FP.txt`.
 * 2. Il affiche l'état initial de la base de données.
 * 3. Il lit et exécute une série de transactions depuis `FT.txt`.
 * 4. Les transactions peuvent inclure :
 *    - '#' : Trouver le professeur avec le plus d'étudiants.
 *    - '*' : Trouver le cours le plus demandé.
 *    - '%' : Compter les professeurs pour un cours donné.
 *    - '-' : Supprimer un professeur.
 *    - '$' : Sauvegarder l'état actuel dans `FP_updated.txt`.
 * 5. Après l'exécution des transactions, il affiche l'état final de la base de données.
 */
int main() {
    // --- Constantes pour les noms de fichiers ---
    const std::string fichierPF_initial = "FP.txt";
    const std::string fichierTransactions = "FT.txt";
    const std::string fichierPF_final = "FP_updated.txt";

    // --- Étape 1: Chargement et affichage initial ---
    afficherTitre("Chargement des données initiales");
    DossierProfesseur monDossier(fichierPF_initial);
    monDossier.afficherContenu();

    // --- Étape 2: Exécution des transactions ---
    executerTransactions(monDossier, fichierTransactions, fichierPF_final);

    // --- Étape 3: Affichage de l'état final ---
    afficherTitre("État final de la liste des professeurs");
    monDossier.afficherContenu();

    return 0;
}

/**
 * @brief Affiche un titre formaté dans la console.
 * @param titre Le texte à afficher comme titre.
 */
void afficherTitre(const std::string& titre) {
    std::cout << "==========================================" << std::endl;
    std::cout << "  Devoir 1 : Gestion de Professeurs" << std::endl;
    std::cout << "  Structure de Données-8INF259 (Automne25)" << std::endl;
    std::cout << "  Collaborateurs :" << std::endl;
    std::cout << "    - Guy Junior Calvet / CALG28109700" << std::endl;
    std::cout << "    - Marie-Anne Randrianarivony / RANI19560500" << std::endl;
    std::cout << "  Date : 6 octobre 2025" << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "  " << titre << std::endl;
    std::cout << "==========================================" << std::endl;
}

/**
 * @brief Lit un fichier de transactions et exécute les opérations correspondantes.
 * @param dossier La référence vers l'objet DossierProfesseur à manipuler.
 * @param nomFichierTransactions Le nom du fichier contenant les transactions.
 * @param nomFichierSauvegarde Le nom du fichier pour l'opération de sauvegarde.
 */
void executerTransactions(DossierProfesseur& dossier, const std::string& nomFichierTransactions, const std::string& nomFichierSauvegarde) {
    afficherTitre("Exécution des transactions");

    std::ifstream fichier(nomFichierTransactions);
    if (!fichier.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier de transactions " << nomFichierTransactions << std::endl;
        return;
    }

    std::string ligne;
    while (std::getline(fichier, ligne)) {
        if (ligne.empty() || ligne.rfind("---", 0) == 0) {
            continue; // Ignorer les lignes vides ou les commentaires
        }

        std::cout << "\n-> Transaction : " << ligne << std::endl;
        std::istringstream iss(ligne);
        char operation;
        iss >> operation;
        iss >> std::ws; // Consommer les espaces blancs

        std::string parametre;
        switch (operation) {
            case '#':
                std::cout << dossier.trouverProfAvecPlusDEtudiants() << std::endl;
                break;
            case '*':
                std::cout << dossier.trouverCoursLePlusDemande() << std::endl;
                break;
            case '%':
                if (iss >> parametre) {
                    int nombre = dossier.compterProfsPourUnCours(parametre);
                    std::cout << "Nombre de professeurs pour le cours '" << parametre << "' : " << nombre << std::endl;
                }
                break;
            case '-':
                if (iss >> parametre) {
                    dossier.supprimerProfesseur(parametre);
                }
                break;
            case '$':
                dossier.sauvegarderDansFichier(nomFichierSauvegarde);
                break;
            default:
                std::cerr << "Opération inconnue : '" << operation << "'" << std::endl;
                break;
        }
    }
    fichier.close();
}
