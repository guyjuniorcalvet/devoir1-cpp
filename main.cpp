#include "DossierProfesseur.h"
#include <iostream>
#include <fstream>
#include <sstream>

int main() {
    const std::string fichierPF_initial = "FP.txt";
    const std::string fichierFT = "FT.txt";
    const std::string fichierPF_final = "FP_updated.txt";

    std::cout << "Chargement des données initiales depuis " << fichierPF_initial << "..." << std::endl;
    DossierProfesseur monDossier(fichierPF_initial);
    monDossier.afficherTousLesProfesseurs();

    std::ifstream fichierTransactions(fichierFT);
    if (!fichierTransactions.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier de transactions " << fichierFT << std::endl;
        return 1;
    }

    std::string ligneTransaction;
    std::cout << "\n--- Exécution des transactions depuis " << fichierFT << " ---" << std::endl;
    while (std::getline(fichierTransactions, ligneTransaction)) {
        if (ligneTransaction.empty()) continue;
        if (ligneTransaction.rfind("---", 0) == 0) continue;

        std::cout << "\nTransaction: " << ligneTransaction << std::endl;
        std::istringstream iss(ligneTransaction);
        char operation;
        iss >> operation;
        iss >> std::ws;

        std::string parametre;
        switch (operation) {
            case '#':
                std::cout << monDossier.afficherLeProfPlusEtudiant() << std::endl;
                break;
            case '*':
                std::cout << monDossier.afficherCoursPlusDemande() << std::endl;
                break;
            case '%':
                if (iss >> parametre) {
                    std::cout << "Nombre de professeurs pour le cours " << parametre << " : "
                              << monDossier.affichernbreProfPourUnCours(parametre) << std::endl;
                }
                break;
            case '-':
                if (iss >> parametre) {
                    monDossier.supprimer(parametre);
                }
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
    monDossier.afficherTousLesProfesseurs();

    return 0;
}
