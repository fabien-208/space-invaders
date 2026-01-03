
---

# 👾 Space Invaders (Projet C Avancé)

Une réimplémentation moderne et robuste du classique *Space Invaders*, développée en C (Standard C11). Ce projet met en avant une architecture logicielle rigoureuse (MVC), la gestion de la mémoire sans fuites, et la portabilité grâce à l'utilisation conjointe de **SDL3** (Graphique) et **Ncurses** (Terminal).

---

## 📋 Fonctionnalités

* **Double Mode d'Affichage :** Basculez instantanément entre le mode Graphique (SDL3) et le mode Texte (Ncurses) en appuyant sur une touche.
* **Architecture MVC :** Séparation stricte des données (Modèle), de l'affichage (Vues) et de la logique (Contrôleur).
* **Système Audio Natif :** Gestion du son via SDL3 Audio Stream (sans dépendance lourde externe).
* **Sauvegarde & Chargement :** 3 emplacements de sauvegarde persistants (fichiers binaires).
* **Menu Paramètres :** Interface graphique pour régler le volume, la vitesse du joueur et reconfigurer les touches.
* **Compatibilité WSL :** Mode de rendu logiciel intégré pour les environnements sans accélération matérielle.

---

## 🛠️ Prérequis et Dépendances

Le projet est conçu pour être autonome concernant SDL3 (incluse localement), mais nécessite les outils de compilation standards.

### Système

* **OS :** Linux (Ubuntu, Debian, Fedora...) ou Windows via WSL/WSL2.
* **Compilateur :** `gcc` (supportant C11).
* **Build System :** `make`.

### Librairies Système

Vous devez installer les bibliothèques de développement pour Ncurses et les outils de build de base.

**Sur Debian / Ubuntu / WSL :**

```bash
sudo apt update
sudo apt install build-essential libncurses-dev libncursesw5-dev cmake

```

*Note : La librairie **SDL3** est fournie dans le dossier `3rdParty/`. Elle est liée statiquement ou dynamiquement au projet via le `Makefile`.*

---

## 🚀 Compilation

Le projet utilise un **Makefile** avancé pour automatiser la compilation.

1. **Compilation complète :**
```bash
make

```


*Ceci génère l'exécutable `jeu` dans le dossier racine.*
2. **Nettoyage (suppression des fichiers objets et de l'exécutable) :**
```bash
make clean

```



---

## 🎮 Exécution

Plusieurs modes de lancement sont disponibles via le Makefile :

### 1. Lancement Standard (Recommandé)

Lance le jeu avec le Launcher graphique (Menu principal).

```bash
make run

```


### 2. Forcer le Mode Texte (Ncurses)

Lance directement le jeu dans le terminal, sans initialiser la fenêtre graphique.

```bash
make run-ncurses

```

---

## ⌨️ Commandes et Contrôles

Les contrôles sont configurables dans le menu **PARAMETRES**, mais voici les réglages par défaut :

### En Jeu (Mode Graphique & Texte)

| Action | Touche (Clavier) |
| --- | --- |
| **Gauche** | `Flèche Gauche` (Configurable) |
| **Droite** | `Flèche Droite` (Configurable) |
| **Tirer** | `Espace` (Configurable) |
| **Pause** | `Echap` ou `S` |
| **Changer de Vue** | `T` (Bascule SDL <-> Ncurses) |
| **Sauvegarder** | `B` (Ouvre le menu save en pause) |
| **Charger** | `C` |
| **Recommencer** | `R` |
| **Quitter** | `Q` |

### Dans les Menus

* **Souris :** Utilisée pour cliquer sur les boutons (Nouvelle Partie, Paramètres, Slots de sauvegarde).
* **Clavier :** Les touches `1`, `2`, `3` permettent de sélectionner les slots de sauvegarde rapidement.

---

## 🏗️ Architecture Logicielle (MVC)

Le projet respecte scrupuleusement le patron de conception **Modèle-Vue-Contrôleur** pour assurer la modularité et la maintenabilité du code.

### 1. Le Modèle (`src/model.c`, `src/model.h`)

C'est le "cerveau" du jeu. Il ne sait pas comment le jeu est affiché.

* **Responsabilités :** Contient toutes les données (positions, scores, état des ennemis, vie).
* **Logique :** Gère les collisions, les déplacements, la physique des tirs et la logique de sauvegarde binaire.
* **Indépendance :** Aucune inclusion de librairie graphique (SDL/Ncurses) n'est faite ici (sauf pour les types de touches).

### 2. Les Vues (`src/view_sdl.c`, `src/view_ncurses.c`)

C'est la "vitrine" du jeu. Elles observent le modèle et l'affichent.

* **View SDL :** Gère la création de la fenêtre, le rendu des sprites (dessinés pixel par pixel via code), les animations et le son (`src/audio.c`).
* **View Ncurses :** Gère l'affichage en caractères ASCII dans le terminal.
* **Polymorphisme :** Le contrôleur peut appeler l'une ou l'autre vue sans changer la logique du jeu.

### 3. Le Contrôleur (`src/controller.c`)

C'est le "chef d'orchestre".

* **Boucle Principale :** Gère le temps (frame rate), récupère les entrées utilisateur (clavier/souris).
* **Routage :** Envoie les commandes au Modèle (ex: "Déplacer le joueur") et demande à la Vue active de se rafraîchir.
* **Gestion des États :** Gère les transitions entre le Menu, le Jeu, la Pause et le Game Over.

---

## 📂 Structure du Projet

```text
├── 3rdParty/        # Dépendances externes (SDL3 inclus localement)
├── assets/          # Fichiers audio (.wav)
├── build/           # Fichiers objets (.o) générés lors de la compilation
├── saves/           # Fichiers de sauvegarde (.bin) générés par le jeu
├── src/             # Code source
│   ├── audio.c      # Gestionnaire de son (SDL3 Stream)
│   ├── controller.c # Boucle de jeu et gestion des événements
│   ├── main.c       # Point d'entrée
│   ├── model.c      # Logique métier et données
│   ├── view_sdl.c   # Rendu graphique
│   └── view_ncurses.c # Rendu textuel
├── Makefile         # Script de compilation automatisé
└── README.md        # Documentation

```

---

## 🔧 Dépannage (Troubleshooting)

**Problème :** `make: *** [Makefile:37: jeu] Error 1` (Erreur de lien GLIBC sur un autre PC).
**Solution :** L'exécutable ou la librairie SDL3 a été compilé sur une machine différente. Il faut recompiler SDL3 localement :

```bash
cd 3rdParty/SDL3-3.2.24
rm -rf build && mkdir build && cd build
cmake ..
make -j$(nproc)
cd ../../..
make clean && make

```

**Problème :** Le jeu est lent ou les graphismes sont "hachés" (grille noire).

