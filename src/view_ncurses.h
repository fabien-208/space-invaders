/**
 * @file view_ncurses.h
 * @brief Gestion de l'affichage en mode terminal.
 */

#ifndef VIEW_NCURSES_H
#define VIEW_NCURSES_H

#include "model.h"

void view_ncurses_init();
void view_ncurses_shutdown();
InputType view_ncurses_get_input();
void view_ncurses_render(const GameState* state);

#endif