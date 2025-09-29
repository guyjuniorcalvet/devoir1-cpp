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
DossierProfesseur::DossierProfesseur(const char* nomFichierPF) : tete(nullptr) {
    std::ifstream fichier(nomFichierPF);
    if (!fichier.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier PF: " << nomFichierPF << std::endl;
        return;
    }

    std::string ligne;
    // Ignorer la ligne d'en-tête
    std::getline(fichier, ligne);
    if (ligne.find("--- START OF FILE FP.txt ---") == std::string::npos) {
        // Si l'en-tête n'est pas là, on replace le curseur au début
        fichier.seekg(0);
    }

    Professeur* dernierProf = nullptr;

    while (std::getline(fichier, ligne)) {
        // La première ligne lue est le nom du professeur
        if (ligne.empty() || ligne == "&") {
            continue; // Ignorer les lignes vides ou les séparateurs inattendus
        }
        std::string nomProf = ligne;

        // La ligne suivante est l'ancienneté
        if (!std::getline(fichier, ligne)) break; // Fin de fichier prématurée
        int anciennete;
        try {
            anciennete = std::stoi(ligne);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Erreur de conversion pour l'ancienneté du prof " << nomProf << ". Ligne: '" << ligne << "'" << std::endl;
            continue; // Passer au prof suivant
        }

        Professeur* nouveauProf = new Professeur(nomProf, anciennete);
        if (tete == nullptr) {
            tete = nouveauProf;
        } else {
            dernierProf->suivant = nouveauProf;
        }
        dernierProf = nouveauProf;

        // Lecture des cours jusqu'au premier '&'
        Cours* dernierCours = nullptr;
        while (std::getline(fichier, ligne) && ligne != "&") {
            if (ligne.empty()) continue;
            Cours* nouveauCours = new Cours(ligne);
            if (nouveauProf->listecours == nullptr) {
                nouveauProf->listecours = nouveauCours;
            } else {
                dernierCours->suivant = nouveauCours;
            }
            dernierCours = nouveauCours;
        }

        // Lecture des étudiants jusqu'au prochain '&' ou la fin du fichier
        Etudiant* dernierEtudiant = nullptr;
        while (std::getline(fichier, ligne) && ligne != "&") {
            if (ligne.empty()) continue;
            Etudiant* nouvelEtudiant = new Etudiant(ligne);
            if (nouveauProf->listetudiants == nullptr) {
                nouveauProf->listetudiants = nouvelEtudiant;
            } else {
                dernierEtudiant->apres = nouvelEtudiant;
            }
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
            if (precedent == nullptr) { // C'est le premier professeur de la liste
                tete = actuel->suivant;
            } else {
                precedent->suivant = actuel->suivant;
            }
            
            // Sauvegarder le pointeur vers le prochain professeur avant de supprimer
            Professeur* aSupprimer = actuel;
            actuel = actuel->suivant; // Avancer avant de supprimer
            
            // Détruire les sous-listes et le professeur
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

std::string DossierProfesseur::afficherLeProfPlusEtudiant() const {
    if (tete == nullptr) {
        return "Aucun professeur dans la liste.";
    }

    std::string nomProfPlusEtudiants = "";
    int maxEtudiants = -1;
    int minAnciennetePourEgalite = std::numeric_limits<int>::max(); // Plus grande valeur possible

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
            // En cas d'égalité, choisir le moins ancien
            if (actuel->anciennete < minAnciennetePourEgalite) {
                nomProfPlusEtudiants = actuel->nom;
                minAnciennetePourEgalite = actuel->anciennete;
            }
        }
        actuel = actuel->suivant;
    }
    return "Professeur avec le plus d'étudiants: " + nomProfPlusEtudiants + " (" + std::to_string(maxEtudiants) + " étudiants)";
}


std::string DossierProfesseur::afficherCoursPlusDemande() const {
    if (tete == nullptr) {
        return "Aucun professeur dans la liste, impossible de trouver le cours le plus demandé.";
    }

    // Utilisation d'une map pour compter les occurrences de chaque cours
    // La clé de la map est le sigle du cours.
    // La valeur est une paire {nombre_de_demandes, anciennete_du_premier_prof_demandeur}
    std::map<std::string, std::pair<int, int>> coursDemandes;

    Professeur* actuel = tete;
    while (actuel != nullptr) {
        Cours* coursActuel = actuel->listecours;
        while (coursActuel != nullptr) {
            // Si le cours n'est pas encore dans la map, l'ajouter
            if (coursDemandes.find(coursActuel->sigle) == coursDemandes.end()) {
                coursDemandes[coursActuel->sigle] = {1, actuel->anciennete};
            } else {
                // Si le cours existe déjà, incrémenter le compteur
                coursDemandes[coursActuel->sigle].first++;
                // Si le nombre de demandes est le même, mettre à jour l'ancienneté si plus petite
                // (critère "premier prof le moins ancien")
                if (actuel->anciennete < coursDemandes[coursActuel->sigle].second) {
                    coursDemandes[coursActuel->sigle].second = actuel->anciennete;
                }
            }
            coursActuel = coursActuel->suivant;
        }
        actuel = actuel->suivant;
    }

    if (coursDemandes.empty()) {
        return "Aucun cours trouvé dans la liste.";
    }

    std::string coursPlusDemande = "";
    int maxDemandes = -1;
    int minAnciennetePourEgalite = std::numeric_limits<int>::max();

    // Parcourir la map pour trouver le cours le plus demandé
    for (const auto& pair : coursDemandes) {
        const std::string& sigle = pair.first;
        int nbDemandes = pair.second.first;
        int anciennetePremierProf = pair.second.second;

        if (nbDemandes > maxDemandes) {
            maxDemandes = nbDemandes;
            coursPlusDemande = sigle;
            minAnciennetePourEgalite = anciennetePremierProf;
        } else if (nbDemandes == maxDemandes) {
            // En cas d'égalité du nombre de demandes, prendre celui du prof le moins ancien
            if (anciennetePremierProf < minAnciennetePourEgalite) {
                coursPlusDemande = sigle;
                minAnciennetePourEgalite = anciennetePremierProf;
            }
        }
    }
    return "Cours le plus demandé: " + coursPlusDemande + " (" + std::to_string(maxDemandes) + " demandes)";
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


void DossierProfesseur::recopier(const char* nomFichierPF) const {
    std::ofstream fichier(nomFichierPF);
    if (!fichier.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier de recopie: " << nomFichierPF << std::endl;
        return;
    }
    
    // Écrit l'en-tête du fichier comme l'original
    fichier << "--- START OF FILE FP.txt ---" << std::endl;

    Professeur* actuel = tete;
    while (actuel != nullptr) {
        fichier << actuel->nom << std::endl;
        fichier << actuel->anciennete << std::endl;

        Cours* coursActuel = actuel->listecours;
        while (coursActuel != nullptr) {
            fichier << coursActuel->sigle << std::endl;
            coursActuel = coursActuel->suivant;
        }
        fichier << "&" << std::endl; // Séparateur cours/étudiants

        Etudiant* etudiantActuel = actuel->listetudiants;
        while (etudiantActuel != nullptr) {
            fichier << etudiantActuel->nom << std::endl;
            etudiantActuel = etudiantActuel->apres;
        }
        
        // Séparateur professeur, sauf pour le dernier
        if (actuel->suivant != nullptr) {
            fichier << "&" << std::endl;
        }
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