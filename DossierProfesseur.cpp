#include "DossierProfesseur.h"
#include <map> // Pour le comptage des cours

// --- Fonctions utilitaires privées pour la destruction des sous-listes ---

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

// --- Constructeur ---
DossierProfesseur::DossierProfesseur(const std::string& nomFichierPF) : tete(nullptr) {
    std::ifstream fichier(nomFichierPF);
    if (!fichier.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier PF: " << nomFichierPF << std::endl;
        return;
    }

    std::string ligne;
    Professeur* dernierProf = nullptr;

    while (std::getline(fichier, ligne)) {
        std::cout << "[DEBUG] Nom professeur lu: '" << ligne << "'" << std::endl;
        std::string nomProf = ligne;
        while (true) {
            // Lire le nom du professeur
            if (!std::getline(fichier, ligne)) break;
            std::string nomProf = ligne;

            // Lire l'ancienneté
            if (!std::getline(fichier, ligne)) break;
            int anciennete = 0;
            try {
                anciennete = std::stoi(ligne);
            } catch (...) {
                std::cerr << "Erreur de lecture de l'ancienneté pour " << nomProf << std::endl;
                continue;
            }

            Professeur* nouveauProf = new Professeur(nomProf, anciennete);
            if (tete == nullptr) tete = nouveauProf;
            else dernierProf->suivant = nouveauProf;
            dernierProf = nouveauProf;

            // Lire les cours jusqu'à &
            Cours* dernierCours = nullptr;
            while (std::getline(fichier, ligne) && ligne != "&") {
                Cours* nouveauCours = new Cours(ligne);
                if (nouveauProf->listecours == nullptr) nouveauProf->listecours = nouveauCours;
                else dernierCours->suivant = nouveauCours;
                dernierCours = nouveauCours;
            }

            // Lire les étudiants jusqu'à &
            Etudiant* dernierEtudiant = nullptr;
            while (std::getline(fichier, ligne) && ligne != "&") {
                Etudiant* nouvelEtudiant = new Etudiant(ligne);
                if (nouveauProf->listetudiants == nullptr) nouveauProf->listetudiants = nouvelEtudiant;
                else dernierEtudiant->apres = nouvelEtudiant;
                dernierEtudiant = nouvelEtudiant;
            }
        }
    }
    fichier.close();
}


// --- Destructeur ---
DossierProfesseur::~DossierProfesseur() {
    Professeur* actuel = tete;
    while (actuel != nullptr) {
        Professeur* suivantProf = actuel->suivant;
        
        // Détruire les listes de cours et d'étudiants du professeur actuel
        detruireListeCours(actuel->listecours);
        detruireListeEtudiants(actuel->listetudiants);

        delete actuel; // Supprimer le professeur
        actuel = suivantProf;
    }
    tete = nullptr; // S'assurer que la tête est nulle après la destruction
}

// --- Méthodes publiques ---

void DossierProfesseur::supprimer(const std::string& name) {
    Professeur* actuel = tete;
    Professeur* precedent = nullptr;

    while (actuel != nullptr) {
        if (actuel->nom == name) {
            if (precedent == nullptr) { // Suppression de la tête de liste
                tete = actuel->suivant;
            } else {
                precedent->suivant = actuel->suivant;
            }
            
            // Libérer la mémoire des sous-listes avant de supprimer le professeur
            detruireListeCours(actuel->listecours);
            detruireListeEtudiants(actuel->listetudiants);
            delete actuel; // Libérer la mémoire du professeur
            
            std::cout << "Professeur " << name << " supprimé." << std::endl;
            return; // Sortir de la fonction après la suppression
        } else {
            precedent = actuel;
            actuel = actuel->suivant;
        }
    }
    
    // Si on arrive ici, le professeur n'a pas été trouvé
    std::cout << "Professeur " << name << " non trouvé." << std::endl;
}

std::string DossierProfesseur::afficherLeProfMoinsEtudiant() const {
    if (tete == nullptr) {
        return "Aucun professeur dans la liste.";
    }

    std::string nomProfMoinsEtudiants = "";
    int minEtudiants = std::numeric_limits<int>::max();
    int minAnciennetePourEgalite = std::numeric_limits<int>::max();

    Professeur* actuel = tete;
    while (actuel != nullptr) {
        int countEtudiants = 0;
        Etudiant* etudiantActuel = actuel->listetudiants;
        while (etudiantActuel != nullptr) {
            countEtudiants++;
            etudiantActuel = etudiantActuel->apres;
        }

        if (countEtudiants < minEtudiants) {
            minEtudiants = countEtudiants;
            nomProfMoinsEtudiants = actuel->nom;
            minAnciennetePourEgalite = actuel->anciennete;
        } else if (countEtudiants == minEtudiants) {
            if (actuel->anciennete < minAnciennetePourEgalite) {
                nomProfMoinsEtudiants = actuel->nom;
                minAnciennetePourEgalite = actuel->anciennete;
            }
        }
        actuel = actuel->suivant;
    }
    return "Professeur avec le moins d'étudiants: " + nomProfMoinsEtudiants + " (" + std::to_string(minEtudiants) + " étudiants)";
}


std::string DossierProfesseur::afficherCoursMoinsDemande() const {
    if (tete == nullptr) {
        return "Aucun professeur dans la liste, impossible de trouver le cours le moins demandé.";
    }

    std::map<std::string, std::pair<int, int>> coursDemandes;

    Professeur* actuel = tete;
    while (actuel != nullptr) {
        Cours* coursActuel = actuel->listecours;
        while (coursActuel != nullptr) {
            coursDemandes[coursActuel->sigle].first++;
            coursActuel = coursActuel->suivant;
        }
        actuel = actuel->suivant;
    }

    if (coursDemandes.empty()) {
        return "Aucun cours trouvé dans la liste.";
    }

    std::string coursMoinsDemande = "";
    int minDemandes = std::numeric_limits<int>::max();
    int minAnciennetePourEgalite = std::numeric_limits<int>::max();

    for (const auto& pair : coursDemandes) {
        const std::string& sigle = pair.first;
        int nbDemandes = pair.second.first;
        
        int minAncienneteCeCours = std::numeric_limits<int>::max();
        Professeur* profRecherche = tete;
        while(profRecherche != nullptr){
            Cours* coursRecherche = profRecherche->listecours;
            while(coursRecherche != nullptr){
                if(coursRecherche->sigle == sigle){
                    if(profRecherche->anciennete < minAncienneteCeCours){
                        minAncienneteCeCours = profRecherche->anciennete;
                    }
                }
                coursRecherche = coursRecherche->suivant;
            }
            profRecherche = profRecherche->suivant;
        }

        if (nbDemandes < minDemandes) {
            minDemandes = nbDemandes;
            coursMoinsDemande = sigle;
            minAnciennetePourEgalite = minAncienneteCeCours;
        } else if (nbDemandes == minDemandes) {
            if (minAncienneteCeCours < minAnciennetePourEgalite) {
                coursMoinsDemande = sigle;
                minAnciennetePourEgalite = minAncienneteCeCours;
            }
        }
    }
    return "Cours le moins demandé: " + coursMoinsDemande + " (" + std::to_string(minDemandes) + " demandes)";
}


int DossierProfesseur::affichernbreProfPourUnCours(const std::string& coursdonne) const {
    int count = 0;
    Professeur* actuel = tete;
    while (actuel != nullptr) {
        Cours* coursActuel = actuel->listecours;
        while (coursActuel != nullptr) {
            if (coursActuel->sigle == coursdonne) {
                count++;
                break; // Un professeur ne compte qu'une fois même s'il demande le cours plusieurs fois
            }
            coursActuel = coursActuel->suivant;
        }
        actuel = actuel->suivant;
    }
    return count;
}


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

// --- Fonction d'affichage pour le débogage ---
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