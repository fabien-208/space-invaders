/**
 * @file controller.h
 * @brief Chef d'orchestre du jeu.
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "model.h"

// Lance la boucle principale du jeu
void controller_run(GameState* state, ViewMode mode);

#endif