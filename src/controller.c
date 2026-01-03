/**
 * @file controller.c
 * @brief Boucle principale et gestion des états.
 * @details Gère la synchro (framerate), les inputs et le basculement SDL/Ncurses.
 */

#define _POSIX_C_SOURCE 199309L
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>
#include <SDL3/SDL.h>

#include "controller.h"
#include "view_ncurses.h" 
#include "view_sdl.h"     
#include "audio.h" 
#include "model.h"

#ifndef _WIN32
    #include <unistd.h>
#endif

#define GAME_WIDTH 1000

// --- FONCTIONS UTILITAIRES NCURSES ---

void draw_box_centered(int h, int w) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int y = (max_y/2) - (h/2);
    int x = (max_x/2) - (w/2);

    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + w, ACS_URCORNER);
    mvaddch(y + h, x, ACS_LLCORNER);
    mvaddch(y + h, x + w, ACS_LRCORNER);
    for (int i = 1; i < w; i++) {
        mvaddch(y, x + i, ACS_HLINE);
        mvaddch(y + h, x + i, ACS_HLINE);
    }
    for (int i = 1; i < h; i++) {
        mvaddch(y + i, x, ACS_VLINE);
        mvaddch(y + i, x + w, ACS_VLINE);
    }
}

void draw_big_text_centered(int y_offset, const char* text) {
    // Mini police ASCII art pour le mode texte
    const char* letters[][5] = { 
        {" GGG ", "G    ", "G  GG", "G   G", " GGG "}, 
        {"  A  ", " A A ", "AAAAA", "A   A", "A   A"}, 
        {"M   M", "MM MM", "M M M", "M   M", "M   M"}, 
        {"EEEEE", "E    ", "EEE  ", "E    ", "EEEEE"}, 
        {" OOO ", "O   O", "O   O", "O   O", " OOO "}, 
        {"V   V", "V   V", "V   V", " V V ", "  V  "}, 
        {"RRRR ", "R   R", "RRRR ", "R R  ", "R  RR"} 
    }; 
    int max_y, max_x; 
    getmaxyx(stdscr, max_y, max_x); 
    int start_x = (max_x - (strlen(text) * 6)) / 2; 
    int start_y = (max_y / 2) + y_offset; 
    
    for (int i = 0; text[i] != '\0'; i++) { 
        int idx = -1; 
        if (text[i] == 'G') idx = 0; else if (text[i] == 'A') idx = 1; 
        else if (text[i] == 'M') idx = 2; else if (text[i] == 'E') idx = 3; 
        else if (text[i] == 'O') idx = 4; else if (text[i] == 'V') idx = 5; 
        else if (text[i] == 'R') idx = 6; 
        
        if (idx != -1) { 
            for (int r = 0; r < 5; r++) { 
                mvprintw(start_y + r, start_x + (i * 6), "%s", letters[idx][r]); 
            } 
        } 
    } 
}

void spawn_wave(GameState* s) {
    s->enemy_count = 30;
    s->enemy_direction = 1;
    for (int i = 0; i < s->enemy_count; i++) {
        s->enemies[i].active = 1;
        s->enemies[i].alive = 1;
        s->enemies[i].pos.x = (GAME_WIDTH - 540) / 2 + (i % 10) * 60;
        s->enemies[i].pos.y = 50 + (i / 10) * 50;
    }
    for (int i = 0; i < MAX_ENEMY_SHOTS; i++) s->enemy_shots[i].active = 0;
}

void controller_handle_input(GameState* s, InputType i, ViewMode m) {
    switch (i) {
        case INPUT_LEFT: model_move_left(s); break;
        case INPUT_RIGHT: model_move_right(s); break;
        case INPUT_SHOOT: 
            model_shoot(s); 
            if (m == VIEW_MODE_SDL) audio_play(SOUND_SHOOT); 
            break;
        case INPUT_QUIT: s->game_over = 2; break;
        default: break;
    }
}

void show_ncurses_pause_menu(GameState* state, ViewMode* mode, int* paused) {
    nodelay(stdscr, FALSE);
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int box_w = 40, box_h = 10;
    draw_box_centered(box_h, box_w);
    mvprintw((max_y / 2) - (box_h / 2) + 2, (max_x / 2) - 2, "PAUSE");
    mvprintw((max_y / 2) - (box_h / 2) + 4, (max_x / 2) - 18, "[ESPACE] Reprendre");
    mvprintw((max_y / 2) - (box_h / 2) + 5, (max_x / 2) - 18, "[R]      Recommencer");
    mvprintw((max_y / 2) - (box_h / 2) + 6, (max_x / 2) - 18, "[T]      Mode SDL");
    mvprintw((max_y / 2) - (box_h / 2) + 7, (max_x / 2) - 18, "[Q]      Quitter");
    refresh();

    while (1) {
        int ch = getch();
        if (ch == ' ' || ch == 27) { *paused = 0; break; }
        else if (ch == 'q' || ch == 'Q') { state->game_over = 2; *paused = 0; break; }
        else if (ch == 'r' || ch == 'R') { model_init(state); spawn_wave(state); *paused = 0; break; }
        else if (ch == 't' || ch == 'T') {
            view_ncurses_shutdown();
            *mode = VIEW_MODE_SDL;
            view_sdl_init();
            audio_init();
            *paused = 0;
            return;
        }
    }
    nodelay(stdscr, TRUE);
}

void show_ncurses_game_over_menu(GameState* state, ViewMode* mode) {
    nodelay(stdscr, FALSE);
    clear();
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    attron(COLOR_PAIR(2) | A_BOLD | A_BLINK);
    draw_big_text_centered(-5, "GAME OVER");
    attroff(COLOR_PAIR(2) | A_BOLD | A_BLINK);

    attron(A_BOLD);
    mvprintw((max_y / 2) + 2, (max_x / 2) - 10, "SCORE FINAL : %d", state->score);
    attroff(A_BOLD);
    mvprintw((max_y / 2) + 5, (max_x / 2) - 15, "[R] Recommencer");
    mvprintw((max_y / 2) + 6, (max_x / 2) - 15, "[T] Mode SDL");
    mvprintw((max_y / 2) + 7, (max_x / 2) - 15, "[Q] Quitter");
    refresh();

    while (1) {
        int ch = getch();
        if (ch == 'q' || ch == 'Q') { state->game_over = 2; break; }
        else if (ch == 'r' || ch == 'R') { model_init(state); spawn_wave(state); break; }
        else if (ch == 't' || ch == 'T') {
            view_ncurses_shutdown();
            *mode = VIEW_MODE_SDL;
            view_sdl_init();
            audio_init();
            model_init(state);
            spawn_wave(state);
            break;
        }
    }
    nodelay(stdscr, TRUE);
}

// --- BOUCLE PRINCIPALE ---

void controller_run(GameState* state, ViewMode mode) {
    srand(time(NULL));
    model_init(state);
    model_init_save_system(); 
    
    if (mode == VIEW_MODE_MENU) {
        mode = view_sdl_show_launcher(state);
    }

    if (mode == VIEW_MODE_QUIT) {
        SDL_Quit(); 
        return; 
    }

    if (mode == VIEW_MODE_SDL) { 
        if (!SDL_WasInit(SDL_INIT_VIDEO)) {
             if(!SDL_Init(SDL_INIT_VIDEO)) exit(1);
        }
        view_sdl_init(); 
        audio_init(); 
        audio_set_volume(state->settings.volume);
    } 
    else { 
        view_ncurses_init(); 
    }
    
    if (state->score == 0 && state->level == 1 && state->enemy_count == 0) { spawn_wave(state); }
    
    int paused = 0;
    int in_menu = 0; 
    int save_menu_open = 0;
    int game_over_sound_played = 0;

    while (1) {
        if (state->game_over == 2) break;
        if (mode == VIEW_MODE_SDL) audio_update();

        InputType input;
        if (mode == VIEW_MODE_SDL && in_menu) input = view_sdl_menu_input(0);
        else input = (mode == VIEW_MODE_SDL) ? view_sdl_get_input(state) : view_ncurses_get_input();

        if (input == INPUT_QUIT) break;
        
        if (input == INPUT_SWITCH_MODE) {
            if (mode == VIEW_MODE_SDL) { audio_shutdown(); view_sdl_shutdown(); mode = VIEW_MODE_NCURSES; view_ncurses_init(); } 
            else { view_ncurses_shutdown(); mode = VIEW_MODE_SDL; view_sdl_init(); audio_init(); }
            continue; 
        }

        if (state->game_over == 1) {
            if (mode == VIEW_MODE_NCURSES) { 
                show_ncurses_game_over_menu(state, &mode); 
                if (state->game_over == 2) break; 
                game_over_sound_played = 0; continue; 
            } 
            else { 
                if (!game_over_sound_played) { audio_play(SOUND_GAME_OVER); game_over_sound_played = 1; } 
                if (input == INPUT_RESTART) { model_init(state); spawn_wave(state); game_over_sound_played = 0; } 
                view_sdl_render(state, paused, save_menu_open); 
                SDL_Delay(16); continue; 
            }
        }

        if (input == INPUT_PAUSE) { paused = !paused; if (mode == VIEW_MODE_NCURSES && paused) { show_ncurses_pause_menu(state, &mode, &paused); if (state->game_over == 2) break; } }

        if (mode == VIEW_MODE_SDL && paused) {
            if (save_menu_open) { 
                if (input == INPUT_1) { model_save_slot(state, 1); save_menu_open=0; paused=0; } 
                if (input == INPUT_2) { model_save_slot(state, 2); save_menu_open=0; paused=0; } 
                if (input == INPUT_3) { model_save_slot(state, 3); save_menu_open=0; paused=0; } 
            } 
            else { 
                if (input == INPUT_SHOOT) paused = 0; 
                if (input == INPUT_RESTART) { model_init(state); spawn_wave(state); paused = 0; } 
                if (input == INPUT_SAVE) save_menu_open = 1; 
            }
        }

        if (!paused && !state->game_over) {
            controller_handle_input(state, input, mode);
            int old_score = state->score;
            model_update(state);
            if (state->score > old_score && mode == VIEW_MODE_SDL) audio_play(SOUND_ENEMY_DIE);
            int active = 0; for(int i=0; i<state->enemy_count; i++) if(state->enemies[i].alive) active++;
            if(active == 0) { state->level++; spawn_wave(state); }
        }

        if (mode == VIEW_MODE_SDL) { 
            view_sdl_render(state, paused, save_menu_open); 
            SDL_Delay(16); 
        } 
        else { 
            if (!paused) { 
                view_ncurses_render(state); 
#ifdef _WIN32 
                _sleep(30); 
#else 
                struct timespec ts={0,30000000}; 
                nanosleep(&ts,NULL); 
#endif 
            } 
        }
    }
    
    if (mode == VIEW_MODE_SDL) { audio_shutdown(); view_sdl_shutdown(); } else view_ncurses_shutdown();
}