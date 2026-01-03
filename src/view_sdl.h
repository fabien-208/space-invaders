/**
 * @file view_sdl.h
 * @brief Gestion de l'affichage graphique via SDL3.
 */

#ifndef VIEW_SDL_H
#define VIEW_SDL_H

#include "model.h"

void view_sdl_init();
void view_sdl_shutdown();

// Récupère les entrées en tenant compte des réglages personnalisés
InputType view_sdl_get_input(const GameState* state); 

InputType view_sdl_menu_input(int load_mode);
void view_sdl_render(const GameState* state, int paused, int save_mode);

// Affiche le Launcher (Menu principal)
ViewMode view_sdl_show_launcher(GameState* state);

#endif