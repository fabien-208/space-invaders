/**
 * @file model.c
 * @brief Implémentation de la logique du jeu.
 * @details Gère les déplacements, collisions, IA des ennemis et persistance des données.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "model.h"

// Vitesses de base
#define ENEMY_SPEED 2
#define PROJECTILE_SPEED 7

/**
 * @brief Vérifie la collision AABB (Axis-Aligned Bounding Box) entre deux rectangles.
 */
int check_collision(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2) {
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

/**
 * @brief Initialise ou réinitialise une partie.
 * @param state Pointeur vers l'état du jeu.
 */
void model_init(GameState* state) {
    state->score = 0; 
    state->lives = 3; 
    state->level = 1; 
    state->game_over = 0;
    state->pos.x = GAME_WIDTH / 2; 
    state->pos.y = GAME_HEIGHT - 60; 
    
    // Initialisation des paramètres par défaut (uniquement au premier lancement)
    if (state->settings.player_speed == 0) {
        state->settings.volume = 50;
        state->settings.player_speed = 6;
        state->settings.key_left = SDLK_LEFT;
        state->settings.key_right = SDLK_RIGHT;
        state->settings.key_shoot = SDLK_SPACE;
    }

    // Nettoyage des entités
    for(int i=0; i<MAX_SHOTS; i++) state->shots[i].active = 0;
    for(int i=0; i<MAX_ENEMY_SHOTS; i++) state->enemy_shots[i].active = 0;
    
    // Placement des boucliers
    int shield_spacing = GAME_WIDTH / 5;
    for(int i=0; i<MAX_SHIELDS; i++) {
        state->shields[i].active = 1; 
        state->shields[i].health = 10;
        state->shields[i].pos.x = shield_spacing * (i + 1) - (SHIELD_W/2);
        state->shields[i].pos.y = GAME_HEIGHT - 120;
    }
    
    for(int i=0; i<MAX_EXPLOSIONS; i++) state->explosions[i].active = 0;
    state->ufo.active = 0; state->ufo.timer = 0;
}

void model_move_left(GameState* state) { 
    state->pos.x -= state->settings.player_speed; 
    if (state->pos.x < 0) state->pos.x = 0; 
}

void model_move_right(GameState* state) { 
    state->pos.x += state->settings.player_speed; 
    if (state->pos.x > GAME_WIDTH - PLAYER_W) state->pos.x = GAME_WIDTH - PLAYER_W; 
}

void model_shoot(GameState* state) {
    for (int i = 0; i < MAX_SHOTS; i++) {
        if (!state->shots[i].active) {
            state->shots[i].active = 1; 
            state->shots[i].pos.x = state->pos.x + (PLAYER_W / 2); 
            state->shots[i].pos.y = state->pos.y; 
            break; 
        }
    }
}

/**
 * @brief Met à jour la logique du jeu (Appelé à chaque frame).
 * @details Gère les tirs, les mouvements ennemis et toutes les collisions.
 */
void model_update(GameState* state) {
    if (state->game_over) return;
    
    // --- 1. Mise à jour des tirs Joueur ---
    for (int i = 0; i < MAX_SHOTS; i++) { 
        if (state->shots[i].active) { 
            state->shots[i].pos.y -= PROJECTILE_SPEED; 
            if (state->shots[i].pos.y < 0) state->shots[i].active = 0; 
        } 
    }
    
    // --- 2. Mise à jour des tirs Ennemis ---
    for (int i = 0; i < MAX_ENEMY_SHOTS; i++) {
        if (state->enemy_shots[i].active) {
            state->enemy_shots[i].pos.y += PROJECTILE_SPEED;
            
            // Sortie d'écran
            if (state->enemy_shots[i].pos.y > GAME_HEIGHT) state->enemy_shots[i].active = 0;
            
            // Collision avec Joueur
            if (check_collision(state->enemy_shots[i].pos.x, state->enemy_shots[i].pos.y, 4, 10, state->pos.x, state->pos.y, PLAYER_W, PLAYER_H)) {
                state->enemy_shots[i].active = 0; 
                state->lives--; 
                if(state->lives <= 0) state->game_over = 1;
            }
            
            // Collision avec Boucliers
            for(int k=0; k<MAX_SHIELDS; k++) {
                if(state->shields[k].active && state->shields[k].health > 0) {
                    if(check_collision(state->enemy_shots[i].pos.x, state->enemy_shots[i].pos.y, 4, 10, state->shields[k].pos.x, state->shields[k].pos.y, SHIELD_W, SHIELD_H)) {
                        state->enemy_shots[i].active = 0; 
                        state->shields[k].health--;
                    }
                }
            }
        }
    }
    
    // --- 3. Déplacement des Ennemis ---
    int hit_edge = 0; 
    int lowest_enemy_y = 0;
    
    for (int i = 0; i < state->enemy_count; i++) {
        if (state->enemies[i].alive) {
            state->enemies[i].pos.x += (ENEMY_SPEED * state->enemy_direction);
            
            // Détection des bords
            if (state->enemies[i].pos.x <= 0 || state->enemies[i].pos.x >= GAME_WIDTH - ENEMY_W) hit_edge = 1;
            if (state->enemies[i].pos.y > lowest_enemy_y) lowest_enemy_y = state->enemies[i].pos.y;
            
            // Probabilité de tir ennemi
            if ((rand() % 2000) < 2) { 
                for(int k=0; k<MAX_ENEMY_SHOTS; k++) {
                    if(!state->enemy_shots[k].active) {
                        state->enemy_shots[k].active = 1; 
                        state->enemy_shots[k].pos.x = state->enemies[i].pos.x + ENEMY_W/2; 
                        state->enemy_shots[k].pos.y = state->enemies[i].pos.y + ENEMY_H; 
                        break;
                    }
                }
            }
        }
    }
    
    // Descente des ennemis si bord touché
    if (hit_edge) { 
        state->enemy_direction *= -1; 
        for (int i = 0; i < state->enemy_count; i++) state->enemies[i].pos.y += 20; 
    }
    
    // Game Over si les ennemis touchent le bas
    if (lowest_enemy_y > state->pos.y - 50) { 
        state->lives = 0; 
        state->game_over = 1; 
    }
    
    // --- 4. Collisions Tirs Joueur vs Ennemis/UFO/Boucliers ---
    for (int i = 0; i < MAX_SHOTS; i++) {
        if (!state->shots[i].active) continue;
        
        // Contre Ennemis
        for (int j = 0; j < state->enemy_count; j++) {
            if (state->enemies[j].alive) {
                if (check_collision(state->shots[i].pos.x, state->shots[i].pos.y, 4, 10, state->enemies[j].pos.x, state->enemies[j].pos.y, ENEMY_W, ENEMY_H)) {
                    state->enemies[j].alive = 0; 
                    state->shots[i].active = 0; 
                    state->score += 10;
                    
                    // Spawn explosion
                    for(int k=0; k<MAX_EXPLOSIONS; k++) { 
                        if(!state->explosions[k].active) { 
                            state->explosions[k].active = 1; 
                            state->explosions[k].type = 0; 
                            state->explosions[k].timer = 10; 
                            state->explosions[k].x = state->enemies[j].pos.x; 
                            state->explosions[k].y = state->enemies[j].pos.y; 
                            break; 
                        } 
                    } 
                    break; 
                }
            }
        }
        
        // Contre UFO
        if(state->shots[i].active && state->ufo.active) {
            if(check_collision(state->shots[i].pos.x, state->shots[i].pos.y, 4, 10, state->ufo.x, state->ufo.y, UFO_W, UFO_H)) { 
                state->ufo.active = 0; 
                state->shots[i].active = 0; 
                state->score += 100; 
            }
        }
        
        // Contre Boucliers (Friendly fire)
        if(state->shots[i].active) {
            for(int k=0; k<MAX_SHIELDS; k++) {
                if(state->shields[k].active && state->shields[k].health > 0) {
                    if(check_collision(state->shots[i].pos.x, state->shots[i].pos.y, 4, 10, state->shields[k].pos.x, state->shields[k].pos.y, SHIELD_W, SHIELD_H)) { 
                        state->shots[i].active = 0; 
                        state->shields[k].health--; 
                    }
                }
            }
        }
    }
    
    // --- 5. Gestion UFO ---
    if (!state->ufo.active) { 
        if (rand() % 1000 < 2) { 
            state->ufo.active = 1; 
            state->ufo.x = -UFO_W; 
            state->ufo.y = 40; 
            state->ufo.direction = 1; 
        } 
    } else { 
        state->ufo.x += 3 * state->ufo.direction; 
        if (state->ufo.x > GAME_WIDTH + 50) state->ufo.active = 0; 
    }
    
    // --- 6. Timer Explosions ---
    for(int i=0; i<MAX_EXPLOSIONS; i++) { 
        if(state->explosions[i].active) { 
            state->explosions[i].timer--; 
            if(state->explosions[i].timer <= 0) state->explosions[i].active = 0; 
        } 
    }
}

// --- SYSTÈME DE SAUVEGARDE (BINAIRE) ---

void model_init_save_system() {
    #ifdef _WIN32
        _mkdir("saves");
    #else
        mkdir("saves", 0777); 
    #endif
}

int model_save_slot(const GameState* state, int slot) {
    char filename[64];
    sprintf(filename, "saves/save_%d.bin", slot);
    FILE* f = fopen(filename, "wb");
    if (!f) { 
        model_init_save_system(); 
        f = fopen(filename, "wb"); 
        if (!f) return 0; 
    }
    fwrite(state, sizeof(GameState), 1, f);
    fclose(f);
    return 1;
}

int model_load_slot(GameState* state, int slot) {
    char filename[64];
    sprintf(filename, "saves/save_%d.bin", slot);
    FILE* f = fopen(filename, "rb");
    if (!f) return 0;
    fread(state, sizeof(GameState), 1, f);
    fclose(f);
    return 1;
}

int model_get_slot_info(int slot, int* score, int* level) {
    char filename[64];
    sprintf(filename, "saves/save_%d.bin", slot);
    FILE* f = fopen(filename, "rb");
    if (!f) return 0; 
    
    GameState temp;
    if (fread(&temp, sizeof(GameState), 1, f) == 1) {
        *score = temp.score;
        *level = temp.level;
        fclose(f);
        return 1;
    }
    fclose(f);
    return 0;
}