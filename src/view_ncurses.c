/**
 * @file view_ncurses.c
 * @brief Implémentation du rendu ASCII via Ncurses.
 */

#include <ncurses.h>
#include <stdlib.h>
#include "view_ncurses.h"
#include "model.h"

void view_ncurses_init() {
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    timeout(0); // Lecture non-bloquante
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);  // Joueur
    init_pair(2, COLOR_RED, COLOR_BLACK);    // Ennemi
    init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Tir
}

void view_ncurses_shutdown() {
    endwin();
}

InputType view_ncurses_get_input() {
    int ch = getch();
    switch (ch) {
        case KEY_LEFT: return INPUT_LEFT;
        case KEY_RIGHT: return INPUT_RIGHT;
        case ' ': return INPUT_SHOOT;
        case 'q': case 'Q': return INPUT_QUIT;
        case 27: case 's': return INPUT_PAUSE; // ESC
        case 't': case 'T': return INPUT_SWITCH_MODE;
        default: return INPUT_NONE;
    }
}

void view_ncurses_render(const GameState* state) {
    erase();
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Echelle simple pour adapter les coordonnées 1000x600 au terminal
    float scale_x = (float)max_x / GAME_WIDTH;
    float scale_y = (float)max_y / GAME_HEIGHT;

    // Joueur
    attron(COLOR_PAIR(1));
    mvprintw((int)(state->pos.y * scale_y), (int)(state->pos.x * scale_x), "A");
    attroff(COLOR_PAIR(1));

    // Ennemis
    attron(COLOR_PAIR(2));
    for (int i = 0; i < state->enemy_count; i++) {
        if (state->enemies[i].alive) {
            mvprintw((int)(state->enemies[i].pos.y * scale_y), (int)(state->enemies[i].pos.x * scale_x), "M");
        }
    }
    attroff(COLOR_PAIR(2));

    // Tirs
    attron(COLOR_PAIR(3));
    for (int i = 0; i < MAX_SHOTS; i++) {
        if (state->shots[i].active) {
            mvprintw((int)(state->shots[i].pos.y * scale_y), (int)(state->shots[i].pos.x * scale_x), "|");
        }
    }
    attroff(COLOR_PAIR(3));

    // HUD
    mvprintw(0, 0, "SCORE: %d | VIES: %d | LVL: %d", state->score, state->lives, state->level);
    refresh();
}