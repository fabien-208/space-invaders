/**
 * @file model.h
 * @brief Définition des structures de données et de la logique métier.
 * @details Contient l'état du jeu (GameState) et les prototypes de manipulation des données.
 */

#ifndef MODEL_H
#define MODEL_H

#include <stdio.h> 
#include <SDL3/SDL.h> // Nécessaire pour SDL_Keycode

// --- CONSTANTES DU JEU ---
#define GAME_WIDTH 1000
#define GAME_HEIGHT 600

// Dimensions des entités
#define PLAYER_W 40
#define PLAYER_H 20
#define ENEMY_W 30
#define ENEMY_H 30
#define SHIELD_W 60
#define SHIELD_H 40
#define UFO_W 40
#define UFO_H 20

// Limites des tableaux statiques
#define MAX_SHOTS 10
#define MAX_ENEMY_SHOTS 10
#define MAX_EXPLOSIONS 20
#define MAX_SHIELDS 4

/**
 * @brief Types d'entrées abstraites (indépendantes du clavier/souris).
 */
typedef enum {
    INPUT_NONE,
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_SHOOT,
    INPUT_QUIT,
    INPUT_PAUSE,
    INPUT_RESTART,
    INPUT_SWITCH_MODE, // Bascule SDL <-> Ncurses
    INPUT_SAVE,
    INPUT_LOAD,
    INPUT_1, INPUT_2, INPUT_3 // Sélection des slots
} InputType;

/**
 * @brief Modes d'affichage et états du Launcher.
 */
typedef enum {
    VIEW_MODE_QUIT = -1,
    VIEW_MODE_MENU = 0,
    VIEW_MODE_NCURSES,
    VIEW_MODE_SDL,
    VIEW_MODE_SDL_LOAD 
} ViewMode;

// --- STRUCTURES ---

typedef struct { float x, y; } Position;
typedef struct { Position pos; int active; } Shot;
typedef struct { Position pos; int active; int alive; } Enemy;
typedef struct { int active; int type; int timer; float x, y; } Explosion;
typedef struct { Position pos; int active; int health; } Shield;
typedef struct { float x, y; int active; int timer; int direction; } Ufo;

/**
 * @brief Paramètres configurables par le joueur.
 */
typedef struct {
    int volume;         // 0 à 100
    int player_speed;   // Vitesse de déplacement (1-20)
    SDL_Keycode key_left;
    SDL_Keycode key_right;
    SDL_Keycode key_shoot;
} GameSettings;

/**
 * @brief État complet du jeu (Sauvegardable tel quel).
 */
typedef struct {
    Position pos;           // Position du joueur
    int score; 
    int lives; 
    int level; 
    int game_over;          // 0: Jeu, 1: Game Over, 2: Quitter
    Shot shots[MAX_SHOTS];
    Shot enemy_shots[MAX_ENEMY_SHOTS];
    int enemy_count; 
    int enemy_direction;    // 1: Droite, -1: Gauche
    Enemy enemies[50];
    Explosion explosions[MAX_EXPLOSIONS];
    Shield shields[MAX_SHIELDS];
    Ufo ufo;
    GameSettings settings;  // Préférences utilisateur
} GameState;

// --- PROTOTYPES ---

void model_init(GameState* state);
void model_update(GameState* state);
void model_move_left(GameState* state);
void model_move_right(GameState* state);
void model_shoot(GameState* state);

// Gestion des sauvegardes
void model_init_save_system(); 
int model_save_slot(const GameState* state, int slot);
int model_load_slot(GameState* state, int slot);
int model_get_slot_info(int slot, int* score, int* level);

#endif