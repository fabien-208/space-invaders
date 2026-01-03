/**
 * @file audio.c
 * @brief Moteur audio utilisant SDL3 Audio Stream.
 * @details Gère le chargement WAV, la conversion de format et le mixage anti-lag.
 */

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "audio.h"

static SDL_AudioStream *stream = NULL;
static Uint8 *wav_buffers[3] = {NULL, NULL, NULL}; 
static Uint32 wav_lengths[3] = {0, 0, 0}; 
static int is_audio_initialized = 0;

// Seuil de lag (44100Hz * 2 channels * 2 bytes * 0.1s)
// Si le buffer contient plus que ça, on ignore les nouveaux sons pour éviter le retard.
#define MAX_AUDIO_LAG 17640

/**
 * @brief Charge un fichier WAV et le convertit au format du périphérique audio.
 */
void load_sound(int index, const char* path) {
    SDL_AudioSpec wav_spec;
    Uint8 *wav_data = NULL;
    Uint32 wav_len = 0;

    if (!SDL_LoadWAV(path, &wav_spec, &wav_data, &wav_len)) {
        printf(" Warning: Impossible de charger %s : %s\n", path, SDL_GetError());
        return;
    }

    SDL_AudioSpec device_spec;
    if (SDL_GetAudioStreamFormat(stream, &device_spec, NULL)) {
        SDL_ConvertAudioSamples(&wav_spec, wav_data, wav_len, 
                                &device_spec, &wav_buffers[index], (int*)&wav_lengths[index]);
    }
    SDL_free(wav_data);
}

int audio_init() {
    if (is_audio_initialized) return 1;

    if (!SDL_WasInit(SDL_INIT_AUDIO)) {
        if (!SDL_Init(SDL_INIT_AUDIO)) return 0;
    }

    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL, NULL, NULL);
    if (!stream) return 0;
    
    SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(stream));
    
    load_sound(0, "assets/shoot.wav");
    load_sound(1, "assets/enemy_die.wav");
    load_sound(2, "assets/game_over.wav");

    is_audio_initialized = 1;
    return 1;
}

void audio_play(SoundType type) {
    if (!stream || !is_audio_initialized) return;
    
    // ANTI-LAG : Si trop de son en attente, on drop
    if (SDL_GetAudioStreamQueued(stream) > MAX_AUDIO_LAG) {
        return; 
    }

    int index = (int)type;
    if (index < 0 || index > 2) return;

    if (wav_buffers[index] && wav_lengths[index] > 0) {
        SDL_PutAudioStreamData(stream, wav_buffers[index], wav_lengths[index]);
    }
}

void audio_set_volume(int volume) {
    if (!stream) return;
    if (volume < 0) volume = 0;
    if (volume > 100) volume = 100;
    
    float gain = (float)volume / 100.0f;
    SDL_SetAudioStreamGain(stream, gain);
}

void audio_update() { }

void audio_shutdown() {
    if (!is_audio_initialized) return;
    for(int i=0; i<3; i++) {
        if (wav_buffers[i]) {
            SDL_free(wav_buffers[i]);
            wav_buffers[i] = NULL;
            wav_lengths[i] = 0;
        }
    }
    if (stream) {
        SDL_DestroyAudioStream(stream);
        stream = NULL;
    }
    is_audio_initialized = 0;
}