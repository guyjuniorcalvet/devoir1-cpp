#include "DossierProfesseur.h"
#include <iostream>
#include <fstream>
#include <sstream> // Pour std::istringstream

int main() {
    // Nom du fichier contenant les données initiales des professeurs
    const std::string fichierPF_initial = "FP.txt";
    // Nom du fichier contenant les transactions
    const std::string fichierFT = "FT.txt";
    // Nom du fichier pour la recopie finale
    const std::string fichierPF_final = "FP_updated.txt"; // Nom différent pour ne pas écraser l'original pendant les tests

    // Création d'une instance de DossierProfesseur, ce qui charge les données depuis FP.txt
    std::cout << "Chargement des données initiales depuis " << fichierPF_initial << "..." << std::endl;
    DossierProfesseur monDossier(fichierPF_initial);
    monDossier.afficherTousLesProfesseurs(); // Utile pour vérifier le chargement

    // Ouvrir le fichier de transactions FT.txt
    std::ifstream fichierTransactions(fichierFT);
    if (!fichierTransactions.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier de transactions FT.txt" << std::endl;
        return 1;
    }

    std::string ligneTransaction;
    std::cout << "\n--- Exécution des transactions depuis " << fichierFT << " ---" << std::endl;
    while (std::getline(fichierTransactions, ligneTransaction)) {
        // Ignorer les lignes vides ou l'en-tête du fichier
        if (ligneTransaction.empty() || ligneTransaction.find("--- START OF FILE FT.txt ---") != std::string::npos) {
            continue;
        }

        std::cout << "\nTransaction: " << ligneTransaction << std::endl;
        std::istringstream iss(ligneTransaction);
        char operation;
        std::string parametre;

        iss >> operation; 
        iss >> std::ws; 

        switch (operation) {
            case '#':
                std::cout << monDossier.afficherLeProfMoinsEtudiant() << std::endl;
                break;
            case '*':
                std::cout << monDossier.afficherCoursMoinsDemande() << std::endl;
                break;
            case '%':
                iss >> parametre; 
                std::cout << "Nombre de professeurs pour le cours " << parametre << " : " 
                          << monDossier.affichernbreProfPourUnCours(parametre) << std::endl;
                break;
            case '-':
                iss >> parametre;
                monDossier.supprimer(parametre);
                break;
            case '$':
                monDossier.recopier(fichierPF_final);
                break;
            default:
                std::cout << "Opération inconnue: " << operation << std::endl;
                break;
        }
    }
    fichierTransactions.close();

    std::cout << "\n--- Toutes les transactions ont été exécutées ---" << std::endl;
    monDossier.afficherTousLesProfesseurs(); // Afficher l'état final après toutes les transactions
    
    // Le destructeur de monDossier sera appelé automatiquement à la fin du main
    return 0;
}
