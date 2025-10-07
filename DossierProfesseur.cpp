#include "DossierProfesseur.h"
#include <fstream>
#include <sstream>
#include <map>
#include <limits>

// --- Implémentation des fonctions utilitaires privées ---

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
        teteEtudiants = teteEtudiants->suivant;
        delete temp;
    }
}

// --- Constructeur ---
DossierProfesseur::DossierProfesseur(const std::string& nomFichierPF) : tete(nullptr) {
    std::ifstream fichier(nomFichierPF);
    if (!fichier.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << nomFichierPF << std::endl;
        return;
    }

    std::string ligne;
    Professeur* dernierProf = nullptr;

    while (std::getline(fichier, ligne)) {
        if (ligne.empty()) continue;

        // 1. Lire les informations du professeur
        std::string nomProf = ligne;
        if (!std::getline(fichier, ligne)) break;
        int anciennete = 0;
        try {
            anciennete = std::stoi(ligne);
        } catch (...) {
            std::cerr << "Erreur de format : Ancienneté invalide pour " << nomProf << std::endl;
            continue;
        }

        // 2. Créer un nouveau professeur et l'ajouter à la liste
        Professeur* nouveauProf = new Professeur(nomProf, anciennete);
        if (tete == nullptr) {
            tete = nouveauProf;
        } else {
            dernierProf->suivant = nouveauProf;
        }
        dernierProf = nouveauProf;

        // 3. Lire la liste des cours souhaités (jusqu'au délimiteur '&')
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

        // 4. Lire la liste des étudiants dirigés (jusqu'au délimiteur '&')
        Etudiant* dernierEtudiant = nullptr;
        while (std::getline(fichier, ligne) && ligne != "&") {
            if (ligne.empty()) continue;
            Etudiant* nouvelEtudiant = new Etudiant(ligne);
            if (nouveauProf->listetudiants == nullptr) {
                nouveauProf->listetudiants = nouvelEtudiant;
            } else {
                dernierEtudiant->suivant = nouvelEtudiant;
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
        Professeur* aSupprimer = actuel;
        actuel = actuel->suivant; // Avancer avant de supprimer

        // Libérer les listes internes
        detruireListeCours(aSupprimer->listecours);
        detruireListeEtudiants(aSupprimer->listetudiants);

        // Supprimer le professeur lui-même
        delete aSupprimer;
    }
    tete = nullptr; // Assurer que la liste est bien vide
}

// --- Suppression d'un professeur ---
void DossierProfesseur::supprimerProfesseur(const std::string& nom) {
    Professeur* actuel = tete;
    Professeur* precedent = nullptr;
    bool trouve = false;

    while (actuel != nullptr) {
        if (actuel->nom == nom) {
            trouve = true;
            Professeur* aSupprimer = actuel;

            if (precedent == nullptr) { // Cas où le prof est en tête de liste
                tete = actuel->suivant;
                actuel = tete;
            } else { // Cas général
                precedent->suivant = actuel->suivant;
                actuel = precedent->suivant;
            }

            // Libérer la mémoire du professeur supprimé
            detruireListeCours(aSupprimer->listecours);
            detruireListeEtudiants(aSupprimer->listetudiants);
            delete aSupprimer;
        } else {
            // Avancer dans la liste
            precedent = actuel;
            actuel = actuel->suivant;
        }
    }

    if (trouve) {
        std::cout << "Le professeur '" << nom << "' a été supprimé." << std::endl;
    } else {
        std::cout << "Le professeur '" << nom << "' n'a pas été trouvé." << std::endl;
    }
}

// --- Recherche du professeur avec le plus d'étudiants ---
std::string DossierProfesseur::trouverProfAvecPlusDEtudiants() const {
    if (tete == nullptr) {
        return "La liste de professeurs est vide.";
    }

    Professeur* profMaxEtudiants = nullptr;
    int maxEtudiants = -1;

    Professeur* actuel = tete;
    while (actuel != nullptr) {
        // Compter les étudiants pour le professeur actuel
        int nbEtudiants = 0;
        Etudiant* etudiantActuel = actuel->listetudiants;
        while (etudiantActuel != nullptr) {
            nbEtudiants++;
            etudiantActuel = etudiantActuel->suivant;
        }

        // Mettre à jour si on trouve un nouveau maximum
        if (nbEtudiants > maxEtudiants) {
            maxEtudiants = nbEtudiants;
            profMaxEtudiants = actuel;
        }
        // Gérer le cas d'égalité : on choisit le prof le moins ancien
        else if (nbEtudiants == maxEtudiants) {
            if (profMaxEtudiants != nullptr && actuel->anciennete < profMaxEtudiants->anciennete) {
                profMaxEtudiants = actuel;
            }
        }
        actuel = actuel->suivant;
    }

    if (profMaxEtudiants != nullptr) {
        return "Le professeur avec le plus d'étudiants est : " + profMaxEtudiants->nom +
               " (" + std::to_string(maxEtudiants) + " étudiants).";
    }
    return "Aucun étudiant trouvé pour aucun professeur.";
}

// --- Recherche du cours le plus demandé ---
std::string DossierProfesseur::trouverCoursLePlusDemande() const {
    if (tete == nullptr) {
        return "La liste de professeurs est vide.";
    }

    // Étape 1: Compter les demandes pour chaque cours
    std::map<std::string, int> compteurCours;
    Professeur* actuel = tete;
    while (actuel != nullptr) {
        Cours* coursActuel = actuel->listecours;
        while (coursActuel != nullptr) {
            compteurCours[coursActuel->sigle]++;
            coursActuel = coursActuel->suivant;
        }
        actuel = actuel->suivant;
    }

    if (compteurCours.empty()) {
        return "Aucun cours n'est demandé.";
    }

    // Étape 2: Trouver le cours avec le maximum de demandes
    std::string coursMax = "";
    int maxDemandes = 0;

    for (const auto& pair : compteurCours) {
        if (pair.second > maxDemandes) {
            maxDemandes = pair.second;
            coursMax = pair.first;
        }
    }

    // Étape 3: Gérer les égalités en trouvant le prof le moins ancien
    int ancienneteMin = std::numeric_limits<int>::max();
    for (const auto& pair : compteurCours) {
        if (pair.second == maxDemandes) {
            // Pour ce cours, trouver le prof le moins ancien qui le demande
            Professeur* profCourant = tete;
            while (profCourant != nullptr) {
                Cours* coursCourant = profCourant->listecours;
                while (coursCourant != nullptr) {
                    if (coursCourant->sigle == pair.first) {
                        if (profCourant->anciennete < ancienneteMin) {
                            ancienneteMin = profCourant->anciennete;
                            coursMax = pair.first;
                        }
                        break; // On a trouvé le cours pour ce prof
                    }
                    coursCourant = coursCourant->suivant;
                }
                profCourant = profCourant->suivant;
            }
        }
    }

    return "Le cours le plus demandé est : " + coursMax +
           " (" + std::to_string(maxDemandes) + " demandes).";
}

// --- Comptage du nombre de professeurs pour un cours donné ---
int DossierProfesseur::compterProfsPourUnCours(const std::string& sigleCours) const {
    int compteur = 0;
    Professeur* actuel = tete;
    while (actuel != nullptr) {
        Cours* coursActuel = actuel->listecours;
        while (coursActuel != nullptr) {
            if (coursActuel->sigle == sigleCours) {
                compteur++;
                break; // On passe au prof suivant dès qu'on trouve le cours
            }
            coursActuel = coursActuel->suivant;
        }
        actuel = actuel->suivant;
    }
    return compteur;
}

// --- Sauvegarde de la liste dans un fichier ---
void DossierProfesseur::sauvegarderDansFichier(const std::string& nomFichierPF) const {
    std::ofstream fichier(nomFichierPF);
    if (!fichier.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier de sauvegarde " << nomFichierPF << std::endl;
        return;
    }

    Professeur* actuel = tete;
    while (actuel != nullptr) {
        // Écrire les infos du prof
        fichier << actuel->nom << std::endl;
        fichier << actuel->anciennete << std::endl;

        // Écrire la liste de cours
        Cours* coursActuel = actuel->listecours;
        while (coursActuel != nullptr) {
            fichier << coursActuel->sigle << std::endl;
            coursActuel = coursActuel->suivant;
        }
        fichier << "&" << std::endl;

        // Écrire la liste d'étudiants
        Etudiant* etudiantActuel = actuel->listetudiants;
        while (etudiantActuel != nullptr) {
            fichier << etudiantActuel->nom << std::endl;
            etudiantActuel = etudiantActuel->suivant;
        }
        fichier << "&" << std::endl;

        actuel = actuel->suivant;
    }
    fichier.close();
    std::cout << "La liste a été sauvegardée dans " << nomFichierPF << std::endl;
}

// --- Affichage du contenu pour débogage ---
void DossierProfesseur::afficherContenu() const {
    std::cout << "\n--- Contenu actuel de la liste des professeurs ---\n";
    if (tete == nullptr) {
        std::cout << "La liste est vide.\n";
        return;
    }

    Professeur* actuelProf = tete;
    int i = 1;
    while (actuelProf != nullptr) {
        std::cout << i++ << ". Professeur: " << actuelProf->nom
                  << " (Ancienneté: " << actuelProf->anciennete << ")\n";

        // Afficher les cours
        std::cout << "   - Cours souhaités: ";
        if (actuelProf->listecours == nullptr) {
            std::cout << "Aucun";
        } else {
            Cours* actuelCours = actuelProf->listecours;
            while (actuelCours != nullptr) {
                std::cout << actuelCours->sigle << (actuelCours->suivant ? ", " : "");
                actuelCours = actuelCours->suivant;
            }
        }
        std::cout << "\n";

        // Afficher les étudiants
        std::cout << "   - Étudiants dirigés: ";
        if (actuelProf->listetudiants == nullptr) {
            std::cout << "Aucun";
        } else {
            Etudiant* actuelEtudiant = actuelProf->listetudiants;
            while (actuelEtudiant != nullptr) {
                std::cout << actuelEtudiant->nom << (actuelEtudiant->suivant ? ", " : "");
                actuelEtudiant = actuelEtudiant->suivant;
            }
        }
        std::cout << "\n\n";

        actuelProf = actuelProf->suivant;
    }
    std::cout << "--- Fin de la liste ---\n";
}
