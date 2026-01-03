# --- COULEURS ET ESTHÉTIQUE ---
GREEN  := \033[1;32m
CYAN   := \033[1;36m
YELLOW := \033[1;33m
RED    := \033[1;31m
RESET  := \033[0m

# --- CONFIGURATION DU PROJET ---
EXEC = jeu
SRC_DIR = src
BUILD_DIR = build

# Chemins SDL3 (Ton installation locale)
SDL_DIR = 3rdParty/SDL3-3.2.24
SDL_INCLUDE = -I$(SDL_DIR)/include
SDL_LIB = -L$(SDL_DIR)/build -Wl,-rpath,'$$ORIGIN/$(SDL_DIR)/build' -lSDL3

# Compilateur et Flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g $(SDL_INCLUDE)
LDFLAGS = $(SDL_LIB) -lncurses -lm

# Fichiers
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# --- RÈGLES PRINCIPALES ---

.PHONY: all clean run run-ncurses run-sdl run-soft valgrind help

# Règle par défaut : Compile tout.
all: $(EXEC)
	@echo "$(GREEN)✅ Compilation terminée avec succès !$(RESET)"
	@echo "Tapez $(YELLOW)make help$(RESET) pour voir les commandes."

# Édition de liens (Création de l'exécutable)
$(EXEC): $(OBJS)
	@echo "$(YELLOW)🔗 Édition des liens...$(RESET)"
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compilation des fichiers sources (.c -> .o)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@echo "$(CYAN)🔨 Compilation de $<...$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

# --- COMMANDES DE LANCEMENT ---

run: $(EXEC)
	@echo "$(GREEN) Lancement du Launcher...$(RESET)"
	@./$(EXEC)

run-ncurses: $(EXEC)
	@echo "$(GREEN)  Lancement forcé en mode TEXTE (Ncurses)...$(RESET)"
	@./$(EXEC) -n

run-sdl: $(EXEC)
	@echo "$(GREEN) Lancement forcé en mode GRAPHIQUE (SDL)...$(RESET)"
	@./$(EXEC) -s

run-soft: $(EXEC)
	@echo "$(GREEN) Lancement en mode Rendu Logiciel (Safe Mode)...$(RESET)"
	@SDL_RENDER_DRIVER=software ./$(EXEC)

valgrind: $(EXEC)
	@echo "$(YELLOW) Analyse mémoire en cours (Valgrind)...$(RESET)"
	@valgrind --leak-check=full --track-origins=yes ./$(EXEC)

# --- UTILITAIRES ---

clean:
	@echo "$(RED)🧹 Nettoyage des fichiers compilés...$(RESET)"
	@rm -rf $(BUILD_DIR) $(EXEC)
	@echo "$(GREEN) Nettoyage terminé.$(RESET)"

help:
	@echo ""
	@echo "$(CYAN)---  SPACE INVADERS - COMMANDES MAKEFILE  ---$(RESET)"
	@echo ""
	@echo "  $(YELLOW)make$(RESET)              : Compile le projet entier."
	@echo "  $(YELLOW)make run$(RESET)          : Lance le jeu (Menu Launcher)."
	@echo "  $(YELLOW)make run-soft$(RESET)     : Lance le jeu en mode compatibilité (pour ton PC)."
	@echo "  $(YELLOW)make run-ncurses$(RESET)  : Force le lancement en mode Texte."
	@echo "  $(YELLOW)make run-sdl$(RESET)      : Force le lancement en mode Graphique."
	@echo "  $(YELLOW)make clean$(RESET)        : Supprime l'exécutable et les fichiers .o."
	@echo "  $(YELLOW)make help$(RESET)         : Affiche ce message."
	@echo ""