#include <stdio.h>      // Pour les entrées/sorties standard
#include <stdlib.h>     // Pour malloc, free, rand, etc.
#include <ctype.h>      // Pour la fonction toupper()
#include <time.h>       // Pour initialiser la graine de génération aléatoire
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"   //définir les couleurs qui vont être utiliser
#define BLUE "\x1B[34m"
#define RESET "\x1B[0m" // enlever ka coloration
// Déclaration du plateau de jeu 3x3 et des symboles pour le joueur et l'ordinateur
char plateau[3][3];        // Matrice représentant le plateau de jeu

const char JOUEUR = 'X';    // Symbole du joueur humain
const char ORDINATEUR = 'O';// Symbole de l'ordinateur
int i, j;

// Structure pour enregistrer les coups joués (liste chaînée)
typedef struct choix {
    int x;
    int y;
    struct choix *next;     // Pointeur vers le prochain mouvement
} choix;

choix *tete = NULL;         // Début de la liste chaînée des mouvements

// Les fonctions
void reinitialiserPlateau();
void afficherPlateau();
int verifierEspacesLibres();
void ajoutermouvement(int x, int y);
void suppmouvement();
int demarrerJeu();
void coupJoueur();
void coupOrdinateur();
char verifierGagnant();
void afficherGagnant(char gagnant);
void afficherMenu();
void afficherRegles();

// Fonction principale : boucle du menu principal
int main() {
    int choix;

    while (1) {
        afficherMenu(); // Affiche le menu principal
        printf(BLUE"Entrez votre choix : "RESET);
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                demarrerJeu(); // Lancer le jeu
                break;
            case 2:
                system("cls || clear");
                afficherRegles(); // Afficher les regles dujeu
                break;
            case 3:
                system("cls || clear");
                printf("\n\n\n");
                printf(GREEN "     ======              Sortie du jeu. Merci d'avoir joue !                      ======\n"RESET);
                return 0; // Fin du programme
            default:
                printf("Choix invalide. Veuillez entrer une option valide (1-3).\n");
        }
    }
}

// Lancer une partie de Tic-Tac-Toe
int demarrerJeu() {
    printf("\n\n\n");
    printf("    ======      Le jeu commence... Profitez de votre partie !         ======\n");

    char gagnant = ' ';
    char reponse = ' ';

    do {
        gagnant = ' ';
        reponse = ' ';
        reinitialiserPlateau();

        // Boucle de jeu : plusieurs  coups
        while (gagnant == ' ' && verifierEspacesLibres() != 0) {
            afficherPlateau();  // Afficher le plateau actuel
            coupJoueur();       // Tour du joueur
            gagnant = verifierGagnant();

            // Si le joueur gagne ou plus de cases disponibles, arrêter
            if (gagnant != ' ' || verifierEspacesLibres() == 0)
                break;

            coupOrdinateur();   // Tour de l’ordinateur
            gagnant = verifierGagnant();
        }

        afficherPlateau();         // Afficher le plateau final
        afficherGagnant(gagnant);  // Afficher le résultat

        // Demander si on veut rejouer
        printf("\n Voulez-vous jouer encore ? (Y/N): ");
        scanf(" %c", &reponse);
        reponse = toupper(reponse); // Convertir en majuscule

    } while (reponse == 'Y');

    printf("    Merci d'avoir joué!           ");
    suppmouvement(); // Libérer la mémoire utilisée pour les mouvements
    return 0;
}

// Affiche les règles du jeu
void afficherRegles() {
    printf(RED"\n============== Regles du Tic-Tac-Toe ==============\n");
    printf("Le jeu se joue sur une grille de 3 cases sur 3.\n");
    printf("Vous etes 'X' et l'ordinateur est 'O'.\n");
    printf("A tour de role, vous placez votre symbole dans une case vide.\n");
    printf("Le premier a aligner trois symboles horizontalement,\n");
    printf("verticalement ou en diagonale gagne la partie.\n");
    printf("Si toutes les cases sont remplies sans gagnant, c est une egalite.\n");
    printf("====================================================\n");

    // Pause pour laisser le joueur lire les règles
    printf("\nAppuyez sur Entree pour revenir au menu..."RESET);
    getchar(); // Pour vider le buffer
    getchar(); // Attend que l'utilisateur appuie sur Entrée
}


// Affiche le menu principal
void afficherMenu() {
    system("cls || clear"); // Nettoie l'écran (Windows/Linux)

    // Affichage centré approximatif (ajusté à une largeur de 80 caractères)
    printf("\n\n\n");
    printf(BLUE"                      ===============================\n");
    printf("                      |        TIC - TAC - TOE      |\n");
    printf("                      ===============================\n");
    printf("                      |                              |\n");
    printf("                      |   (1) Demarrer le jeu        |\n");
    printf("                      |   (2) Regles du jeu          |\n");
    printf("                      |   (3) Quitter                |\n");
    printf("                      |                              |\n");
    printf("                      ===============================\n"RESET);
}


// Vide toutes les cases du plateau
void reinitialiserPlateau() {
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            plateau[i][j] = ' '; // Espace vide
        }
    }
}

// Affiche le plateau actuel avec des séparateurs
void afficherPlateau() {
    printf("\n");
    printf(" %c | %c | %c ", plateau[0][0], plateau[0][1], plateau[0][2]);
    printf("\n---|---|---\n");
    printf(" %c | %c | %c ", plateau[1][0], plateau[1][1], plateau[1][2]);
    printf("\n---|---|---\n");
    printf(" %c | %c | %c ", plateau[2][0], plateau[2][1], plateau[2][2]);
    printf("\n");
}

// Compte le nombre de cases encore disponibles
int verifierEspacesLibres() {
    int espacesLibres = 0;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (plateau[i][j] == ' ') {
                espacesLibres++;
            }
        }
    }
    return espacesLibres;
}

// Ajoute un mouvement à la liste chaînée (historique)
void ajoutermouvement(int x, int y) {
    // Allouer de la mémoire pour un nouveau nœud de type choix
    choix *nouveau = malloc(sizeof(choix));
    if (nouveau == NULL) {
        // Vérification de l’allocation mémoire
        printf("Erreur d'allocation mémoire.\n");
        exit(EXIT_FAILURE); // Arrêt du programme si échec
    }

    // Affectation des coordonnées au nouveau mouvement
    nouveau->x = x;
    nouveau->y = y;

    // Le nouveau nœud pointe vers l'ancienne tête de la liste
    nouveau->next = tete;

    // Mise à jour de la tête de la liste avec le nouveau nœud
    tete = nouveau;
}

// Libère toute la mémoire de la liste chaînée
void suppmouvement() {
    choix *parcour = tete, *next;

    // Parcours de la liste chaînée
    while (parcour != NULL) {
        next = parcour->next; // Avant de supprimer le nœud pointé par parcour, on sauvegarde l’adresse du prochain élément pour pouvoir y accéder ensuite.
        free(parcour);        // Libère la mémoire du nœud courant
        parcour = next;       // Passe au nœud suivant
    }

    // Réinitialise la tête de liste à NULL pour signaler qu'elle est vide
    tete = NULL;
}

// Gère le tour du joueur
void coupJoueur() {
    int x, y;

    do {
        // Demande à l'utilisateur la ligne (1 à 3)
        printf("Entrez le numero de ligne (1-3): ");
        scanf("%d", &x);
        x--; // Convertir en index de tableau (0 à 2)

        // Demande la colonne (1 à 3)
        printf("Entrez le numero de colonne (1-3): ");
        scanf("%d", &y);
        y--; // Convertir en index de tableau (0 à 2)

        // Vérifie si la case est déjà occupée
        if (plateau[x][y] != ' ') {
            printf("Mouvement non valide!\n");
        } else {
            // Si la case est vide, on joue le coup
            plateau[x][y] = JOUEUR;      // Place le symbole du joueur
            ajoutermouvement(x, y);      // Enregistre le mouvement dans la liste
            break; // Sortie de la boucle
        }
    } while (1); // Boucle jusqu'à un coup valide
}


// Gère le tour de l’ordinateur

void coupOrdinateur() {
    // 1. L'ordinateur vérifie s’il peut gagner en un coup
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            if (plateau[x][y] == ' ') { // Si la case est vide
                plateau[x][y] = ORDINATEUR; // Joue temporairement ici
                if (verifierGagnant() == ORDINATEUR) {
                    ajoutermouvement(x, y); // Enregistre le coup gagnant
                    return; // Il joue ce coup gagnant et termine son tour
                }
                plateau[x][y] = ' '; // Sinon, annule le coup (backtracking)
            }
        }
    }

    // 2. Sinon, il bloque le joueur s’il peut gagner au prochain coup
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            if (plateau[x][y] == ' ') { // Case vide
                plateau[x][y] = JOUEUR; // Simule un coup du joueur
                if (verifierGagnant() == JOUEUR) {
                    // Si ce coup fait gagner le joueur, l’ordinateur bloque ici
                    plateau[x][y] = ORDINATEUR;
                    ajoutermouvement(x, y);
                    return;
                }
                plateau[x][y] = ' '; // Annule la simulation
            }
        }
    }

    // 3. Sinon, l'ordinateur joue dans une case vide aléatoire
    int x, y;
    srand(time(0)); // Initialisation de la graine aléatoire
    do {
        x = rand() % 3;
        y = rand() % 3;
    } while (plateau[x][y] != ' ');

    plateau[x][y] = ORDINATEUR;
    ajoutermouvement(x, y);
}

// Vérifie s’il y a un gagnant
char verifierGagnant() {
    // Vérifie les lignes
    for (i = 0; i < 3; i++) {
        if (plateau[i][0] == plateau[i][1] &&plateau[i][0] == plateau[i][2] &&plateau[i][0] != ' ') {
            return plateau[i][0];
        }
    }

    // Vérifie les colonnes
    for (i = 0; i < 3; i++) {
        if (plateau[0][i] == plateau[1][i] &&plateau[0][i] == plateau[2][i] &&plateau[0][i] != ' ') {
            return plateau[0][i];
        }
    }

    // Vérifie la diagonale principale
    if (plateau[0][0] == plateau[1][1] &&plateau[0][0] == plateau[2][2] &&plateau[0][0] != ' ') {
        return plateau[0][0];
    }

    // Vérifie la diagonale secondaire
    if (plateau[0][2] == plateau[1][1] &&plateau[0][2] == plateau[2][0] &&plateau[0][2] != ' ') {
        return plateau[0][2];
    }

    return ' ';
}

// Affiche le gagnant ou s’il y a égalité
void afficherGagnant(char gagnant) {
    if (gagnant == JOUEUR) {
        printf("VOUS GAGNEZ , Bravo ");
    } else if (gagnant == ORDINATEUR) {
        printf("VOUS PERDEZ , Dommage");
    } else {
        printf("PARTIE NULLE!");
    }
}
