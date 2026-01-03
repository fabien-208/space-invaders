/**
 * @file audio.h
 * @brief Gestion du son.
 * @details Interface pour l'initialisation, la lecture et le nettoyage du système audio.
 */

#ifndef AUDIO_H
#define AUDIO_H

typedef enum {
    SOUND_SHOOT,
    SOUND_ENEMY_DIE,
    SOUND_GAME_OVER
} SoundType;

int audio_init();
void audio_shutdown();
void audio_play(SoundType type); 
void audio_update(); 
void audio_set_volume(int volume); // 0-100

#endif