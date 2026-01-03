/**
 * @file main.c
 * @brief Point d'entrée du jeu Space Invaders.
 * @details Gère les arguments de la ligne de commande (-n, -s) et lance le contrôleur.
 * @date 2026
 */

#include <string.h>
#include "controller.h"
#include "model.h"

/**
 * @brief Fonction principale.
 * @param argc Nombre d'arguments.
 * @param argv Tableau des arguments.
 * @return 0 si succès.
 */
int main(int argc, char* argv[]) {
    // État par défaut : on lance le Menu Launcher
    ViewMode mode = VIEW_MODE_MENU; 
    GameState state = {0};

    // Analyse des arguments pour forcer un mode spécifique
    if (argc > 1) {
        if (strcmp(argv[1], "-n") == 0) {
            mode = VIEW_MODE_NCURSES; // Force le mode Texte
        } else if (strcmp(argv[1], "-s") == 0) {
            mode = VIEW_MODE_SDL;     // Force le mode Graphique
        }
    }

    // Lancement de la boucle principale du jeu
    controller_run(&state, mode);

    return 0;
}