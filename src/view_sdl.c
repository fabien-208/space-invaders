/**
 * @file view_sdl.c
 * @brief Implémentation du rendu graphique (Pixel Art procédural).
 * @details Dessine les sprites pixel par pixel pour un look rétro authentique. Gère aussi le menu Launcher.
 */

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include <time.h> 
#include "view_sdl.h"
#include "controller.h" 
#include "model.h" 
#include "audio.h" 

#define GAME_WIDTH 1000
#define GAME_HEIGHT 600

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

#define MAX_STARS 100
typedef struct { float x, y, speed; int brightness; } Star;
static Star stars[MAX_STARS];

// --- DONNÉES SPRITES (BITMAPS 0/1) ---

#define SPRITE_PLAYER_W 11
#define SPRITE_PLAYER_H 8
static const int sprite_player[8][11] = { 
    {0,0,0,0,0,1,0,0,0,0,0}, {0,0,0,0,1,1,1,0,0,0,0}, {0,0,0,0,1,1,1,0,0,0,0}, {0,1,1,1,1,1,1,1,1,1,0}, 
    {1,1,1,1,1,1,1,1,1,1,1}, {1,1,1,1,1,1,1,1,1,1,1}, {1,0,1,1,1,0,1,1,1,0,1}, {1,0,1,0,0,0,0,0,1,0,1} 
};

#define SPRITE_ENEMY_W 11
#define SPRITE_ENEMY_H 8
static const int sprite_enemy_1[8][11] = { 
    {0,0,1,0,0,0,0,0,1,0,0}, {0,0,0,1,0,0,0,1,0,0,0}, {0,0,1,1,1,1,1,1,1,0,0}, {0,1,1,0,1,1,1,0,1,1,0}, 
    {1,1,1,1,1,1,1,1,1,1,1}, {1,0,1,1,1,1,1,1,1,0,1}, {1,0,1,0,0,0,0,0,1,0,1}, {0,0,0,1,1,0,1,1,0,0,0} 
};
static const int sprite_enemy_2[8][11] = { 
    {0,0,1,0,0,0,0,0,1,0,0}, {1,0,0,1,0,0,0,1,0,0,1}, {1,0,1,1,1,1,1,1,1,0,1}, {1,1,1,0,1,1,1,0,1,1,1}, 
    {1,1,1,1,1,1,1,1,1,1,1}, {0,1,1,1,1,1,1,1,1,1,0}, {0,0,1,0,0,0,0,0,1,0,0}, {0,1,0,0,0,0,0,0,0,1,0} 
};

#define SPRITE_UFO_W 16
#define SPRITE_UFO_H 7
static const int sprite_ufo[7][16] = { 
    {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0}, {0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0}, {0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0}, 
    {0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0}, {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, {0,0,1,1,1,0,0,1,1,0,0,1,1,1,0,0}, 
    {0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0} 
};

static const int sprite_explosion[8][11] = { 
    {1,0,0,0,1,0,1,0,0,0,1}, {0,1,0,0,0,1,0,0,0,1,0}, {0,0,1,0,0,0,0,0,1,0,0}, {0,0,0,1,1,1,1,1,0,0,0}, 
    {1,0,0,1,1,0,1,1,0,0,1}, {0,0,0,1,1,1,1,1,0,0,0}, {0,1,0,0,0,1,0,0,0,1,0}, {1,0,0,0,1,0,1,0,0,0,1} 
};

#define SPRITE_SHIELD_W 22
#define SPRITE_SHIELD_H 16
static const int sprite_shield_full[16][22] = { 
    {0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0}, {0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0}, 
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0}, {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0}, 
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, 
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, 
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, 
    {1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1}, {1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1}, 
    {1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1}, {1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1}, 
    {1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1}, {1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1} 
};
static const int sprite_shield_damaged[16][22] = { 
    {0,0,0,0,1,1,1,0,1,1,1,1,1,1,0,1,1,1,0,0,0,0}, {0,0,0,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,0,0,0}, 
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,0}, {0,1,1,1,0,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,0}, 
    {1,1,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1}, {1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1}, 
    {1,1,1,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1,1,1,1,1}, {1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,1,1}, 
    {1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1}, {1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,1,1,1,0,1,1,1}, 
    {1,1,1,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,0}, {1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,0,1,1,1,1}, 
    {1,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,1}, {1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1}, 
    {1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1}, {1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0} 
};
static const int sprite_shield_critical[16][22] = { 
    {0,0,0,0,1,0,1,0,0,0,1,1,0,0,0,1,0,1,0,0,0,0}, {0,0,0,0,1,0,0,1,0,0,1,1,0,0,1,0,0,1,0,0,0,0}, 
    {0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0}, {0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0}, 
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, {1,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,0,0,1}, 
    {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, {0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0}, 
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, {0,0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0}, 
    {1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1}, {0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0}, 
    {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1}, {0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0}, 
    {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1}, {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1} 
};

// --- POLICE ---
static const unsigned char font_data[39][5] = {
    {0b01110, 0b10001, 0b11111, 0b10001, 0b10001}, // A
    {0b11110, 0b10001, 0b11110, 0b10001, 0b11110}, // B
    {0b01110, 0b10001, 0b10000, 0b10001, 0b01110}, // C
    {0b11110, 0b10001, 0b10001, 0b10001, 0b11110}, // D
    {0b11111, 0b10000, 0b11110, 0b10000, 0b11111}, // E
    {0b11111, 0b10000, 0b11110, 0b10000, 0b10000}, // F
    {0b01111, 0b10000, 0b10111, 0b10001, 0b01110}, // G
    {0b10001, 0b10001, 0b11111, 0b10001, 0b10001}, // H
    {0b01110, 0b00100, 0b00100, 0b00100, 0b01110}, // I
    {0b00111, 0b00010, 0b00010, 0b10010, 0b01100}, // J
    {0b10001, 0b10010, 0b11100, 0b10010, 0b10001}, // K
    {0b10000, 0b10000, 0b10000, 0b10000, 0b11111}, // L
    {0b10001, 0b11011, 0b10101, 0b10001, 0b10001}, // M
    {0b10001, 0b11001, 0b10101, 0b10011, 0b10001}, // N
    {0b01110, 0b10001, 0b10001, 0b10001, 0b01110}, // O
    {0b11110, 0b10001, 0b11110, 0b10000, 0b10000}, // P
    {0b01110, 0b10001, 0b10001, 0b10010, 0b01101}, // Q
    {0b11110, 0b10001, 0b11110, 0b10010, 0b10001}, // R
    {0b01111, 0b10000, 0b01110, 0b00001, 0b11110}, // S
    {0b11111, 0b00100, 0b00100, 0b00100, 0b00100}, // T
    {0b10001, 0b10001, 0b10001, 0b10001, 0b01110}, // U
    {0b10001, 0b10001, 0b10001, 0b01010, 0b00100}, // V
    {0b10001, 0b10001, 0b10101, 0b11011, 0b10001}, // W
    {0b10001, 0b01010, 0b00100, 0b01010, 0b10001}, // X
    {0b10001, 0b10001, 0b01110, 0b00100, 0b00100}, // Y
    {0b11111, 0b00010, 0b00100, 0b01000, 0b11111}, // Z
    {0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // ESPACE
    {0b01110, 0b10001, 0b10001, 0b10001, 0b01110}, // 0
    {0b00100, 0b01100, 0b00100, 0b00100, 0b01110}, // 1
    {0b01110, 0b10001, 0b00010, 0b00100, 0b11111}, // 2
    {0b11110, 0b00001, 0b01110, 0b00001, 0b11110}, // 3
    {0b00010, 0b00110, 0b01010, 0b11111, 0b00010}, // 4
    {0b11111, 0b10000, 0b11110, 0b00001, 0b11110}, // 5
    {0b01110, 0b10000, 0b11110, 0b10001, 0b01110}, // 6
    {0b11111, 0b00001, 0b00010, 0b00100, 0b00100}, // 7
    {0b01110, 0b10001, 0b01110, 0b10001, 0b01110}, // 8
    {0b01110, 0b10001, 0b01111, 0b00001, 0b01110}, // 9
    {0b00000, 0b00000, 0b11111, 0b00000, 0b00000}, // '-'
    {0b00100, 0b00100, 0b11111, 0b00100, 0b00100}  // '+'
};

// --- FONCTIONS DE DESSIN ---

void draw_sprite(SDL_Renderer* ren, float x, float y, const int* sprite_data, int w, int h, float scale, int r, int g, int b) { 
    SDL_SetRenderDrawColor(ren, r, g, b, 255); 
    for(int row=0; row<h; row++) for(int col=0; col<w; col++) 
        if(sprite_data[row*w + col]) { 
            SDL_FRect p={x+(col*scale), y+(row*scale), scale, scale}; 
            SDL_RenderFillRect(ren, &p); 
        } 
}

void draw_sprite_alpha(SDL_Renderer* ren, float x, float y, const int* sprite_data, int w, int h, float scale, int r, int g, int b, int a) { 
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND); 
    SDL_SetRenderDrawColor(ren, r, g, b, a); 
    for(int row=0; row<h; row++) for(int col=0; col<w; col++) 
        if(sprite_data[row*w + col]) { 
            SDL_FRect p={x+(col*scale), y+(row*scale), scale, scale}; 
            SDL_RenderFillRect(ren, &p); 
        } 
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_NONE);
}

void draw_text(SDL_Renderer* ren, float x, float y, const char* text, float size) { 
    if (!ren || !text) return; 
    float cx = x; 
    for (int i=0; text[i]; i++) { 
        int idx = 26; 
        char c = toupper(text[i]); 
        if (c >= 'A' && c <= 'Z') idx = c - 'A'; 
        else if (c >= '0' && c <= '9') idx = 27 + (c - '0'); 
        else if (c == '-') idx = 37; 
        else if (c == '+') idx = 38; 

        for (int r=0; r<5; r++) 
            for (int cl=0; cl<5; cl++) 
                if ((font_data[idx][r] >> (4-cl)) & 1) { 
                    SDL_FRect p={cx+(cl*size), y+(r*size), size, size}; 
                    SDL_RenderFillRect(ren, &p); 
                } 
        cx += (6*size); 
    } 
}

void draw_text_centered(SDL_Renderer* ren, float y, const char* text, float size) { if (!ren || !text) return; int len = strlen(text); float w = len * 6 * size; draw_text(ren, (GAME_WIDTH - w)/2.0f, y, text, size); }
void draw_heart(SDL_Renderer* ren, float x, float y, int filled, float size) { static const unsigned char f[] = { 0b01010, 0b11111, 0b11111, 0b01110, 0b00100 }; static const unsigned char e[] = { 0b01010, 0b10101, 0b10001, 0b01010, 0b00100 }; const unsigned char* p = filled ? f : e; for(int r=0; r<5; r++) for(int c=0; c<5; c++) if((p[r]>>(4-c))&1) { SDL_FRect px={x+(c*size), y+(r*size), size, size}; SDL_RenderFillRect(ren, &px); } }
void draw_invader_icon(SDL_Renderer* ren, float x, float y, float s) { SDL_FRect rects[] = { {2*s,0,s,s}, {8*s,0,s,s}, {3*s,1*s,5*s,s}, {2*s,2*s,7*s,s}, {1*s,3*s,2*s,s}, {4*s,3*s,3*s,s}, {8*s,3*s,2*s,s}, {1*s,4*s,9*s,s}, {2*s,5*s,1*s,s}, {8*s,5*s,1*s,s}, {3*s,6*s,1*s,s}, {4*s,6*s,1*s,s}, {6*s,6*s,1*s,s}, {7*s,6*s,1*s,s} }; for(size_t i=0; i<sizeof(rects)/sizeof(SDL_FRect); i++) { SDL_FRect r=rects[i]; r.x+=x; r.y+=y; SDL_RenderFillRect(ren, &r); } }
void init_stars() { for(int i=0; i<MAX_STARS; i++) { stars[i].x=rand()%GAME_WIDTH; stars[i].y=rand()%GAME_HEIGHT; stars[i].speed=1+(rand()%5)*0.5f; stars[i].brightness=100+(rand()%155); } }

void view_sdl_init() { SDL_SetHint("SDL_RENDER_SCALE_QUALITY", "nearest"); if (!SDL_Init(SDL_INIT_VIDEO)) exit(1); window = SDL_CreateWindow("Space Invaders", GAME_WIDTH, GAME_HEIGHT, SDL_WINDOW_RESIZABLE); SDL_SetWindowResizable(window, true); renderer = SDL_CreateRenderer(window, NULL); SDL_SetRenderLogicalPresentation(renderer, GAME_WIDTH, GAME_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX); init_stars(); }

void view_sdl_shutdown() { 
    if (renderer) SDL_DestroyRenderer(renderer); 
    if (window) SDL_DestroyWindow(window); 
    SDL_Quit(); 
}

InputType view_sdl_get_input(const GameState* state) { 
    SDL_Event event; 
    while (SDL_PollEvent(&event)) { 
        if (event.type == SDL_EVENT_QUIT) return INPUT_QUIT; 
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.repeat == 0) { 
            if (event.key.key == SDLK_ESCAPE) return INPUT_PAUSE;
            if (event.key.key == SDLK_R) return INPUT_RESTART;
            if (event.key.key == SDLK_S) return INPUT_PAUSE;
            if (event.key.key == SDLK_Q) return INPUT_QUIT;
            if (event.key.key == SDLK_T) return INPUT_SWITCH_MODE;
            if (event.key.key == SDLK_B) return INPUT_SAVE;
            if (event.key.key == SDLK_C) return INPUT_LOAD;
            if (event.key.key == SDLK_1 || event.key.key == SDLK_KP_1) return INPUT_1;
            if (event.key.key == SDLK_2 || event.key.key == SDLK_KP_2) return INPUT_2;
            if (event.key.key == SDLK_3 || event.key.key == SDLK_KP_3) return INPUT_3;
            if (event.key.key == state->settings.key_shoot) return INPUT_SHOOT;
        } 
    } 
    const bool* k = SDL_GetKeyboardState(NULL); 
    if (k[SDL_GetScancodeFromKey(state->settings.key_left, NULL)]) return INPUT_LEFT;
    if (k[SDL_GetScancodeFromKey(state->settings.key_right, NULL)]) return INPUT_RIGHT;
    return INPUT_NONE; 
}

InputType view_sdl_menu_input(int load_mode) {
    (void)load_mode; 
    return INPUT_NONE;
}

void view_sdl_render(const GameState* state, int paused, int save_mode) {
    if (!renderer) return;
    SDL_SetRenderDrawColor(renderer, 5, 5, 20, 255); SDL_RenderClear(renderer);
    if (!paused && !state->game_over) { for(int i=0; i<MAX_STARS; i++) { stars[i].y += stars[i].speed; if(stars[i].y > GAME_HEIGHT) { stars[i].y=0; stars[i].x=rand()%GAME_WIDTH; } SDL_SetRenderDrawColor(renderer, 255, 255, 255, stars[i].brightness); SDL_FRect s = { stars[i].x, stars[i].y, 2, 2 }; SDL_RenderFillRect(renderer, &s); } }

    if (!state->game_over) {
        int anim = (SDL_GetTicks() / 500) % 2;
        float p_sc = (float)PLAYER_W / SPRITE_PLAYER_W;
        draw_sprite(renderer, state->pos.x, state->pos.y, (const int*)sprite_player, SPRITE_PLAYER_W, SPRITE_PLAYER_H, p_sc, 50, 255, 100);
        float e_sc = (float)ENEMY_W / SPRITE_ENEMY_W;
        for (int i=0; i<state->enemy_count; i++) { if (state->enemies[i].alive) { const int* sp = (anim == 0) ? (const int*)sprite_enemy_1 : (const int*)sprite_enemy_2; int g = (i%2==0) ? 50 : 80; int b = (i%2==0) ? 50 : 255; draw_sprite(renderer, state->enemies[i].pos.x, state->enemies[i].pos.y, sp, SPRITE_ENEMY_W, SPRITE_ENEMY_H, e_sc, 255, g, b); } }
        for (int i=0; i<MAX_EXPLOSIONS; i++) { if (state->explosions[i].active) { int g = (state->explosions[i].timer * 255) / 15; draw_sprite(renderer, state->explosions[i].x, state->explosions[i].y, (const int*)sprite_explosion, 11, 8, e_sc, 255, g, 0); } }
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); for (int i=0; i<MAX_SHOTS; i++) if (state->shots[i].active) { SDL_FRect s = { (float)state->shots[i].pos.x, (float)state->shots[i].pos.y, 4, 15 }; SDL_RenderFillRect(renderer, &s); }
        SDL_SetRenderDrawColor(renderer, 200, 200, 255, 255); for (int i=0; i<MAX_ENEMY_SHOTS; i++) if (state->enemy_shots[i].active) { SDL_FRect es = { (float)state->enemy_shots[i].pos.x, (float)state->enemy_shots[i].pos.y, 4, 15 }; SDL_RenderFillRect(renderer, &es); }
        
        float shield_scale = (float)SHIELD_W / SPRITE_SHIELD_W;
        for (int i=0; i<MAX_SHIELDS; i++) {
            if (state->shields[i].active && state->shields[i].health > 0) {
                int hp = state->shields[i].health;
                const int* current_sprite;
                if (hp > 7) current_sprite = (const int*)sprite_shield_full;
                else if (hp > 3) current_sprite = (const int*)sprite_shield_damaged;
                else current_sprite = (const int*)sprite_shield_critical;
                int r = (10 - hp) * 25; int g = hp * 25; int b = (hp > 3) ? 255 : 50; int a = 100 + (hp * 15); 
                
                if (r > 255) r = 255; 
                if (g > 255) g = 255; 
                if (a > 255) a = 255;

                draw_sprite_alpha(renderer, state->shields[i].pos.x, state->shields[i].pos.y, current_sprite, SPRITE_SHIELD_W, SPRITE_SHIELD_H, shield_scale, r, g, b, a);
            }
        }

        if (state->ufo.active) { float usc = (float)UFO_W / SPRITE_UFO_W; draw_sprite(renderer, state->ufo.x, state->ufo.y, (const int*)sprite_ufo, SPRITE_UFO_W, SPRITE_UFO_H, usc, 0, 255, 255); }
        SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255); for(int i = 0; i < 3; i++) draw_heart(renderer, 20 + (i * 40), 20, (i < state->lives), 6.0f);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); char buf[32]; snprintf(buf, 32, "LEVEL %d", state->level); draw_text_centered(renderer, 20, buf, 3); snprintf(buf, 32, "SCORE %d", state->score); draw_text(renderer, GAME_WIDTH - 200, 20, buf, 3);

        if (paused) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); SDL_FRect ov = {0, 0, GAME_WIDTH, GAME_HEIGHT}; SDL_RenderFillRect(renderer, &ov); SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            if (save_mode) {
                draw_text_centered(renderer, 150, "SAUVEGARDER", 8); draw_text_centered(renderer, 280, "CHOISIR EMPLACEMENT (1-3)", 3);
                for(int i=1; i<=3; i++) { int sc, lv; char txt[64]; if (model_get_slot_info(i, &sc, &lv)) snprintf(txt, 64, "[%d] LVL %d - %d PTS", i, lv, sc); else snprintf(txt, 64, "[%d] VIDE", i); draw_text_centered(renderer, 350 + (i-1)*50, txt, 4); }
            } else {
                draw_text_centered(renderer, 150, "PAUSE", 8); draw_text_centered(renderer, 280, "ESPACE - REPRENDRE", 3); draw_text_centered(renderer, 320, "B - SAUVEGARDER", 3); draw_text_centered(renderer, 360, "R - RECOMMENCER", 3); draw_text_centered(renderer, 400, "T - MODE TEXTE", 3); draw_text_centered(renderer, 440, "Q - QUITTER", 3);
            }
        }
    } else {
        SDL_SetRenderDrawColor(renderer, 30, 0, 0, 255); SDL_RenderClear(renderer); 
        if ((SDL_GetTicks()/400)%2) { SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); draw_text_centered(renderer, 100, "GAME OVER", 8); } 
        else { SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255); draw_text_centered(renderer, 100, "GAME OVER", 8); }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); char buf[64]; snprintf(buf, 64, "SCORE FINAL: %d", state->score); draw_text_centered(renderer, 250, buf, 5); draw_text_centered(renderer, 400, "PRESS R TO RESTART", 4); draw_text_centered(renderer, 550, "PRESS Q TO QUIT", 4);
    }
    SDL_RenderPresent(renderer);
}

void view_sdl_render_launcher_frame(int step, GameState* state, int rebinding_idx) {
    if (!renderer) return;
    SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255); SDL_RenderClear(renderer);
    for(int i=0; i<MAX_STARS; i++) { stars[i].y += stars[i].speed; if(stars[i].y > GAME_HEIGHT) stars[i].y=0; SDL_SetRenderDrawColor(renderer, 255, 255, 255, stars[i].brightness); SDL_FRect s = { stars[i].x, stars[i].y, 2, 2 }; SDL_RenderFillRect(renderer, &s); }

    float wx, wy, mx, my; SDL_GetMouseState(&wx, &wy); SDL_RenderCoordinatesFromWindow(renderer, wx, wy, &mx, &my);

    // --- HOME (0) ---
    if (step == 0) {
        draw_text_centered(renderer, 80, "SPACE INVADERS", 8);
        float msc = 5.0f; draw_sprite(renderer, (GAME_WIDTH/2) - 150, 180, (const int*)sprite_enemy_1, SPRITE_ENEMY_W, SPRITE_ENEMY_H, msc, 255, 50, 50);  
        draw_sprite(renderer, (GAME_WIDTH/2) + 100, 180, (const int*)sprite_enemy_2, SPRITE_ENEMY_W, SPRITE_ENEMY_H, msc, 255, 50, 200);
        float psc = 6.0f; draw_sprite(renderer, (GAME_WIDTH - (SPRITE_PLAYER_W*psc))/2, 180, (const int*)sprite_player, SPRITE_PLAYER_W, SPRITE_PLAYER_H, psc, 50, 255, 50);

        int h1 = (mx > 300 && mx < 700 && my > 300 && my < 380);
        SDL_SetRenderDrawColor(renderer, h1?50:20, h1?150:60, h1?50:20, 255); 
        SDL_FRect r1 = {300, 300, 400, 80}; SDL_RenderFillRect(renderer, &r1);
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); draw_text_centered(renderer, 325, "NOUVELLE PARTIE", 4);
        
        int h2 = (mx > 300 && mx < 700 && my > 400 && my < 480);
        SDL_SetRenderDrawColor(renderer, h2?50:20, h2?50:20, h2?150:60, 255); 
        SDL_FRect r2 = {300, 400, 400, 80}; SDL_RenderFillRect(renderer, &r2);
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); draw_text_centered(renderer, 425, "CHARGER PARTIE", 4);
        
        int h_param = (mx > 20 && mx < 220 && my > 520 && my < 570);
        SDL_SetRenderDrawColor(renderer, h_param?100:50, h_param?100:50, h_param?100:50, 255);
        SDL_FRect r_param = {20, 520, 200, 50}; SDL_RenderFillRect(renderer, &r_param);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); draw_text(renderer, 40, 535, "PARAMETRES", 3);

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); draw_text_centered(renderer, 580, "BY GEMINI & CHEF", 2);
    }
    // --- SLOTS (1) ---
    else if (step == 1) {
        draw_text_centered(renderer, 100, "CHOIX DU SLOT", 6);
        for(int i=1; i<=3; i++) {
            float by = 280 + (i-1)*60;
            int h = (mx > 200 && mx < 800 && my > by && my < by+50);
            SDL_SetRenderDrawColor(renderer, h?50:20, h?50:20, h?100:40, 255);
            SDL_FRect r = {200, by, 600, 50}; SDL_RenderFillRect(renderer, &r);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); SDL_RenderRect(renderer, &r);
            int sc, lv; char txt[64]; if(model_get_slot_info(i, &sc, &lv)) snprintf(txt, 64, "SLOT %d : LVL %d - %d PTS", i, lv, sc); else snprintf(txt, 64, "SLOT %d : VIDE", i);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); draw_text_centered(renderer, by+15, txt, 3);
        }
        int hret = (mx > 350 && mx < 650 && my > 480 && my < 530);
        SDL_SetRenderDrawColor(renderer, hret?150:60, hret?50:20, hret?50:20, 255); 
        SDL_FRect b = {350, 480, 300, 50}; SDL_RenderFillRect(renderer, &b);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); draw_text_centered(renderer, 495, "RETOUR", 3);
    }
    // --- VUE (2) ---
    else if (step == 2) {
        draw_text_centered(renderer, 50, "CHOISISSEZ VOTRE VUE", 4);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        int h_sdl = (mx < 500 && my < 520);
        if(h_sdl) SDL_SetRenderDrawColor(renderer, 40, 150, 40, 100); else SDL_SetRenderDrawColor(renderer, 20, 60, 20, 100);
        SDL_FRect l = {0, 150, 500, 350}; SDL_RenderFillRect(renderer, &l);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE); 
        draw_invader_icon(renderer, 200, 250, 8.0f); draw_text(renderer, 210, 400, "SDL", 5);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        if(!h_sdl && my < 520) SDL_SetRenderDrawColor(renderer, 150, 150, 40, 100); else SDL_SetRenderDrawColor(renderer, 60, 60, 20, 100);
        SDL_FRect r = {500, 150, 500, 350}; SDL_RenderFillRect(renderer, &r);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        draw_text(renderer, 700, 300, "TXT", 10);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); SDL_FRect sep = {495, 150, 10, 350}; SDL_RenderFillRect(renderer, &sep);
        int hret = (my > 520 && my < 570 && mx > 350 && mx < 650);
        SDL_SetRenderDrawColor(renderer, hret?150:60, hret?50:20, hret?50:20, 255); 
        SDL_FRect b = {350, 520, 300, 50}; SDL_RenderFillRect(renderer, &b);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); draw_text_centered(renderer, 535, "RETOUR", 3);
    }
    // --- PARAMETRES (3) ---
    else if (step == 3) {
        draw_text_centered(renderer, 30, "PARAMETRES", 6);
        char buf[64];

        // VOLUME
        snprintf(buf, 64, "VOLUME: %d%%", state->settings.volume);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        draw_text(renderer, 100, 120, buf, 4);
        int h_v_m = (mx > 600 && mx < 650 && my > 110 && my < 160);
        SDL_SetRenderDrawColor(renderer, h_v_m?150:50, 50, 50, 255); SDL_FRect vm = {600, 110, 50, 50}; SDL_RenderFillRect(renderer, &vm);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); draw_text(renderer, 615, 120, "-", 4);
        int h_v_p = (mx > 670 && mx < 720 && my > 110 && my < 160);
        SDL_SetRenderDrawColor(renderer, 50, h_v_p?150:50, 50, 255); SDL_FRect vp = {670, 110, 50, 50}; SDL_RenderFillRect(renderer, &vp);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); draw_text(renderer, 685, 120, "+", 4);

        // VITESSE
        snprintf(buf, 64, "VITESSE: %d", state->settings.player_speed);
        draw_text(renderer, 100, 190, buf, 4);
        int h_s_m = (mx > 600 && mx < 650 && my > 180 && my < 230);
        SDL_SetRenderDrawColor(renderer, h_s_m?150:50, 50, 50, 255); SDL_FRect sm = {600, 180, 50, 50}; SDL_RenderFillRect(renderer, &sm);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); draw_text(renderer, 615, 190, "-", 4);
        int h_s_p = (mx > 670 && mx < 720 && my > 180 && my < 230);
        SDL_SetRenderDrawColor(renderer, 50, h_s_p?150:50, 50, 255); SDL_FRect sp = {670, 180, 50, 50}; SDL_RenderFillRect(renderer, &sp);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); draw_text(renderer, 685, 190, "+", 4);

        // TOUCHES
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 30, 30, 50, 150);
        SDL_FRect bg_ctrl = {50, 260, 900, 240}; SDL_RenderFillRect(renderer, &bg_ctrl);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        draw_text_centered(renderer, 280, "CONTROLES (CLIQUER POUR CHANGER)", 3);

        draw_text(renderer, 100, 330, "GAUCHE:", 3);
        const char* name_l = SDL_GetKeyName(state->settings.key_left);
        int h_k_l = (mx > 400 && mx < 800 && my > 320 && my < 360);
        SDL_SetRenderDrawColor(renderer, (rebinding_idx==0)?200:(h_k_l?80:40), 40, 40, 255);
        SDL_FRect rkl = {400, 320, 400, 40}; SDL_RenderFillRect(renderer, &rkl);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        draw_text(renderer, 420, 330, (rebinding_idx == 0) ? "APPUYEZ..." : name_l, 3);

        draw_text(renderer, 100, 390, "DROITE:", 3);
        const char* name_r = SDL_GetKeyName(state->settings.key_right);
        int h_k_r = (mx > 400 && mx < 800 && my > 380 && my < 420);
        SDL_SetRenderDrawColor(renderer, (rebinding_idx==1)?200:(h_k_r?80:40), 40, 40, 255);
        SDL_FRect rkr = {400, 380, 400, 40}; SDL_RenderFillRect(renderer, &rkr);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        draw_text(renderer, 420, 390, (rebinding_idx == 1) ? "APPUYEZ..." : name_r, 3);

        draw_text(renderer, 100, 450, "TIR:", 3);
        const char* name_s = SDL_GetKeyName(state->settings.key_shoot);
        int h_k_s = (mx > 400 && mx < 800 && my > 440 && my < 480);
        SDL_SetRenderDrawColor(renderer, (rebinding_idx==2)?200:(h_k_s?80:40), 40, 40, 255);
        SDL_FRect rks = {400, 440, 400, 40}; SDL_RenderFillRect(renderer, &rks);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        draw_text(renderer, 420, 450, (rebinding_idx == 2) ? "APPUYEZ..." : name_s, 3);

        int hret = (mx > 350 && mx < 650 && my > 530 && my < 580);
        SDL_SetRenderDrawColor(renderer, hret?150:60, hret?50:20, hret?50:20, 255); 
        SDL_FRect b = {350, 530, 300, 50}; SDL_RenderFillRect(renderer, &b);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); draw_text_centered(renderer, 545, "RETOUR", 3);
    }
    
    SDL_RenderPresent(renderer);
}

ViewMode view_sdl_show_launcher(GameState* state) {
    if (!SDL_Init(SDL_INIT_VIDEO)) return VIEW_MODE_NCURSES;
    
    SDL_Window* win = SDL_CreateWindow("SPACE INVADERS", GAME_WIDTH, GAME_HEIGHT, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* ren = SDL_CreateRenderer(win, NULL);
    if (!win || !ren) return VIEW_MODE_NCURSES;
    
    SDL_Renderer* old_renderer = renderer; 
    renderer = ren; 
    
    SDL_SetRenderLogicalPresentation(ren, GAME_WIDTH, GAME_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    init_stars();
    
    audio_init();
    audio_set_volume(state->settings.volume);

    ViewMode choice = VIEW_MODE_MENU; 
    int step = 0; 
    int rebinding_idx = -1;

    while (choice == VIEW_MODE_MENU) {
        float wx, wy, mx, my; SDL_GetMouseState(&wx, &wy); SDL_RenderCoordinatesFromWindow(ren, wx, wy, &mx, &my);
        SDL_Event ev; 
        
        while (SDL_PollEvent(&ev)) { 
            if (ev.type==SDL_EVENT_QUIT) {
                choice = VIEW_MODE_QUIT; 
            }
            
            if (rebinding_idx != -1 && ev.type == SDL_EVENT_KEY_DOWN) {
                if (ev.key.key != SDLK_ESCAPE) {
                    if (rebinding_idx == 0) state->settings.key_left = ev.key.key;
                    if (rebinding_idx == 1) state->settings.key_right = ev.key.key;
                    if (rebinding_idx == 2) state->settings.key_shoot = ev.key.key;
                }
                rebinding_idx = -1; 
            }
            else if (ev.type==SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (step == 0) { 
                    if (mx > 300 && mx < 700 && my > 300 && my < 380) step = 2; 
                    else if (mx > 300 && mx < 700 && my > 400 && my < 480) step = 1; 
                    else if (mx > 20 && mx < 220 && my > 520 && my < 570) step = 3; 
                }
                else if (step == 1) { 
                    for(int i=1; i<=3; i++) {
                        if (mx > 200 && mx < 800 && my > 280 + (i-1)*60 && my < 330 + (i-1)*60) if(model_load_slot(state, i)) step = 2;
                    }
                    if (mx > 350 && mx < 650 && my > 480 && my < 530) step = 0; 
                }
                else if (step == 2) { 
                    if (my > 520 && my < 570 && mx > 350 && mx < 650) step = 0; 
                    else if (my < 500) {
                        if (mx < 500) choice = VIEW_MODE_SDL; else choice = VIEW_MODE_NCURSES;
                    }
                }
                else if (step == 3) {
                    if (rebinding_idx == -1) {
                        if (mx > 600 && mx < 650 && my > 110 && my < 160) { state->settings.volume -= 10; audio_set_volume(state->settings.volume); }
                        if (mx > 670 && mx < 720 && my > 110 && my < 160) { state->settings.volume += 10; audio_set_volume(state->settings.volume); }
                        if (mx > 600 && mx < 650 && my > 180 && my < 230) { if(state->settings.player_speed > 1) state->settings.player_speed--; }
                        if (mx > 670 && mx < 720 && my > 180 && my < 230) { if(state->settings.player_speed < 20) state->settings.player_speed++; }
                        
                        if (mx > 400 && mx < 800 && my > 320 && my < 360) rebinding_idx = 0;
                        if (mx > 400 && mx < 800 && my > 380 && my < 420) rebinding_idx = 1;
                        if (mx > 400 && mx < 800 && my > 440 && my < 480) rebinding_idx = 2;

                        if (mx > 350 && mx < 650 && my > 530 && my < 580) step = 0;
                    }
                }
            } 
        }
        
        if (choice == VIEW_MODE_QUIT) break;

        view_sdl_render_launcher_frame(step, state, rebinding_idx);
        SDL_Delay(16);
    }
    
    audio_shutdown();
    renderer = old_renderer; 
    SDL_DestroyRenderer(ren); 
    SDL_DestroyWindow(win); 
    SDL_Quit(); 

    return choice;
}