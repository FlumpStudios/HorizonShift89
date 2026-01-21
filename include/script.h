#ifndef SCRIPT_H
#define SCRIPT_H

#include <gb/gb.h>
#include <stdint.h>

// --- Wave System Constants ---
#define MAX_WAVES 10

// --- Wave Configuration Structure ---
// enemy_mask bits: 0=NORMAL, 1=SHOOTER, 2=ZIGZAG, 3=ASTEROID
typedef struct {
    uint8_t enemy_mask;   // Bitmask of allowed enemy types
    uint8_t spawn_rate;   // Frames between spawns
} WaveConfig;

// --- Wave State (extern declarations) ---
extern uint8_t current_wave;
extern uint8_t wave_kills;
extern uint8_t wave_kills_required;
extern uint8_t wave_complete_timer;

// --- Wave Scripts Array ---
extern const WaveConfig wave_scripts[MAX_WAVES];

// --- Function Prototypes ---
void init_wave_system(void);
void advance_wave(void);
uint8_t get_wave_kills_required(uint8_t wave);
uint8_t get_random_enemy_for_wave(uint8_t wave);

#endif
