#include "DossierProfesseur.h"
#include <fstream>
#include <sstream>
#include <map>
#include <limits>

void DossierProfesseur::detruireListeCours(Cours*& teteCours) {
    while (teteCours != nullptr) {
        Cours* temp = teteCours;
        teteCours = teteCours->suivant;
        delete temp;
    }
}

void DossierProfesseur::detruireListeEtudiants(Etudiant*& teteEtudiants) {
    while (teteEtudiants != nullptr) {
        Etudiant* temp = teteEtudiants;
        teteEtudiants = teteEtudiants->apres;
        delete temp;
    }
}

// ✅ Constructeur corrigé
DossierProfesseur::DossierProfesseur(const std::string& nomFichierPF) : tete(nullptr) {
    std::ifstream fichier(nomFichierPF);
    if (!fichier.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier PF: " << nomFichierPF << std::endl;
        return;
    }

    std::string ligne;
    Professeur* dernierProf = nullptr;

    while (true) {
        // Lire le nom du professeur
        if (!std::getline(fichier, ligne)) break;
        if (ligne.empty()) continue;
        std::string nomProf = ligne;

        // Lire ancienneté
        if (!std::getline(fichier, ligne)) break;
        int anciennete = 0;
        try {
            anciennete = std::stoi(ligne);
        } catch (...) {
            std::cerr << "Erreur: Ancienneté invalide pour " << nomProf << " (" << ligne << ")" << std::endl;
            continue;
        }

        // Créer le professeur
        Professeur* nouveauProf = new Professeur(nomProf, anciennete);
        if (tete == nullptr) tete = nouveauProf;
        else dernierProf->suivant = nouveauProf;
        dernierProf = nouveauProf;

        // Lire cours jusqu'à &
        Cours* dernierCours = nullptr;
        while (std::getline(fichier, ligne)) {
            if (ligne == "&") break;
            if (ligne.empty()) continue;
            Cours* nouveauCours = new Cours(ligne);
            if (nouveauProf->listecours == nullptr) nouveauProf->listecours = nouveauCours;
            else dernierCours->suivant = nouveauCours;
            dernierCours = nouveauCours;
        }

        // Lire étudiants jusqu'à &
        Etudiant* dernierEtudiant = nullptr;
        while (std::getline(fichier, ligne)) {
            if (ligne == "&") break;
            if (ligne.empty()) continue;
            Etudiant* nouvelEtudiant = new Etudiant(ligne);
            if (nouveauProf->listetudiants == nullptr) nouveauProf->listetudiants = nouvelEtudiant;
            else dernierEtudiant->apres = nouvelEtudiant;
            dernierEtudiant = nouvelEtudiant;
        }
    }

    fichier.close();
}

// --- Destructeur ---
DossierProfesseur::~DossierProfesseur() {
    Professeur* actuel = tete;
    while (actuel != nullptr) {
        Professeur* suivantProf = actuel->suivant;
        detruireListeCours(actuel->listecours);
        detruireListeEtudiants(actuel->listetudiants);
        delete actuel;
        actuel = suivantProf;
    }
    tete = nullptr;
}

// --- Supprimer ---
void DossierProfesseur::supprimer(const std::string& name) {
    Professeur* actuel = tete;
    Professeur* precedent = nullptr;

    while (actuel != nullptr) {
        if (actuel->nom == name) {
            Professeur* aSupprimer = actuel;
            if (precedent == nullptr) { 
                tete = actuel->suivant;
                actuel = tete;
            } else {
                precedent->suivant = actuel->suivant;
                actuel = precedent->suivant;
            }
            detruireListeCours(aSupprimer->listecours);
            detruireListeEtudiants(aSupprimer->listetudiants);
            delete aSupprimer;
            std::cout << "Professeur " << name << " supprimé." << std::endl;
        } else {
            precedent = actuel;
            actuel = actuel->suivant;
        }
    }
}

// --- Afficher le prof avec le plus d’étudiants ---
std::string DossierProfesseur::afficherLeProfPlusEtudiant() const {
    if (tete == nullptr) {
        return "Aucun professeur dans la liste.";
    }

    std::string nomProfPlusEtudiants = "";
    int maxEtudiants = std::numeric_limits<int>::min();
    int minAnciennetePourEgalite = std::numeric_limits<int>::max();

    Professeur* actuel = tete;
    while (actuel != nullptr) {
        int countEtudiants = 0;
        Etudiant* etudiantActuel = actuel->listetudiants;
        while (etudiantActuel != nullptr) {
            countEtudiants++;
            etudiantActuel = etudiantActuel->apres;
        }

        if (countEtudiants > maxEtudiants) {
            maxEtudiants = countEtudiants;
            nomProfPlusEtudiants = actuel->nom;
            minAnciennetePourEgalite = actuel->anciennete;
        } else if (countEtudiants == maxEtudiants) {
            if (actuel->anciennete < minAnciennetePourEgalite) {
                nomProfPlusEtudiants = actuel->nom;
                minAnciennetePourEgalite = actuel->anciennete;
            }
        }
        actuel = actuel->suivant;
    }
    return "Professeur avec le plus d'étudiants: " + nomProfPlusEtudiants + " (" + std::to_string(maxEtudiants) + " étudiants)";
}

// --- Afficher le cours le plus demandé ---
std::string DossierProfesseur::afficherCoursPlusDemande() const {
    if (tete == nullptr) {
        return "Aucun professeur dans la liste, impossible de trouver le cours le moins demandé.";
    }

    std::map<std::string, int> coursDemandes;

    Professeur* actuel = tete;
    while (actuel != nullptr) {
        Cours* coursActuel = actuel->listecours;
        while (coursActuel != nullptr) {
            coursDemandes[coursActuel->sigle]++;
            coursActuel = coursActuel->suivant;
        }
        actuel = actuel->suivant;
    }

    if (coursDemandes.empty()) {
        return "Aucun cours trouvé dans la liste.";
    }

    std::string coursPlusDemande = "";
    int maxDemandes = std::numeric_limits<int>::min();
    int minAnciennetePourEgalite = std::numeric_limits<int>::max();

    for (const auto& pair : coursDemandes) {
        const std::string& sigle = pair.first;
        int nbDemandes = pair.second;

        int minAncienneteCeCours = std::numeric_limits<int>::max();
        Professeur* profRecherche = tete;
        while (profRecherche != nullptr) {
            Cours* coursRecherche = profRecherche->listecours;
            while (coursRecherche != nullptr) {
                if (coursRecherche->sigle == sigle) {
                    if (profRecherche->anciennete < minAncienneteCeCours) {
                        minAncienneteCeCours = profRecherche->anciennete;
                    }
                    break;
                }
                coursRecherche = coursRecherche->suivant;
            }
            profRecherche = profRecherche->suivant;
        }

        if (nbDemandes > maxDemandes) {
            maxDemandes = nbDemandes;
            coursPlusDemande = sigle;
            minAnciennetePourEgalite = minAncienneteCeCours;
        } else if (nbDemandes == maxDemandes) {
            if (minAncienneteCeCours < minAnciennetePourEgalite) {
                coursPlusDemande = sigle;
                minAnciennetePourEgalite = minAncienneteCeCours;
            }
        }
    }

    return "Cours le plus demandé: " + coursPlusDemande + " (" + std::to_string(maxDemandes) + " demandes)";
}

// --- Nombre de profs pour un cours ---
int DossierProfesseur::affichernbreProfPourUnCours(const std::string& coursdonne) const {
    int count = 0;
    Professeur* actuel = tete;
    while (actuel != nullptr) {
        Cours* coursActuel = actuel->listecours;
        while (coursActuel != nullptr) {
            if (coursActuel->sigle == coursdonne) {
                count++;
                break;
            }
            coursActuel = coursActuel->suivant;
        }
        actuel = actuel->suivant;
    }
    return count;
}

// --- Recopie ---
void DossierProfesseur::recopier(const std::string& nomFichierPF) const {
    std::ofstream fichier(nomFichierPF);
    if (!fichier.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier de recopie: " << nomFichierPF << std::endl;
        return;
    }

    Professeur* actuel = tete;
    while (actuel != nullptr) {
        fichier << actuel->nom << std::endl;
        fichier << actuel->anciennete << std::endl;

        Cours* coursActuel = actuel->listecours;
        while (coursActuel != nullptr) {
            fichier << coursActuel->sigle << std::endl;
            coursActuel = coursActuel->suivant;
        }
        fichier << "&" << std::endl;

        Etudiant* etudiantActuel = actuel->listetudiants;
        while (etudiantActuel != nullptr) {
            fichier << etudiantActuel->nom << std::endl;
            etudiantActuel = etudiantActuel->apres;
        }
        fichier << "&" << std::endl;

        actuel = actuel->suivant;
    }
    fichier.close();
    std::cout << "Liste chaînée recopiée dans " << nomFichierPF << std::endl;
}

// --- Affichage ---
void DossierProfesseur::afficherTousLesProfesseurs() const {
    std::cout << "\n--- Affichage de tous les professeurs ---" << std::endl;
    if (tete == nullptr) {
        std::cout << "La liste est vide." << std::endl;
        return;
    }

    Professeur* actuelProf = tete;
    while (actuelProf != nullptr) {
        std::cout << "Professeur: " << actuelProf->nom
                  << ", Ancienneté: " << actuelProf->anciennete << std::endl;

        std::cout << "  Cours souhaités: ";
        Cours* actuelCours = actuelProf->listecours;
        if (actuelCours == nullptr) {
            std::cout << "Aucun.";
        }
        while (actuelCours != nullptr) {
            std::cout << actuelCours->sigle << " ";
            actuelCours = actuelCours->suivant;
        }
        std::cout << std::endl;

        std::cout << "  Étudiants dirigés: ";
        Etudiant* actuelEtudiant = actuelProf->listetudiants;
        if (actuelEtudiant == nullptr) {
            std::cout << "Aucun.";
        }
        while (actuelEtudiant != nullptr) {
            std::cout << actuelEtudiant->nom << " ";
            actuelEtudiant = actuelEtudiant->apres;
        }
        std::cout << std::endl << std::endl;

        actuelProf = actuelProf->suivant;
    }
    std::cout << "---------------------------------------" << std::endl;
}
