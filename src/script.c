// Wave script system for Horizon Shift 89
// Configurable wave-based enemy spawning

#include <gb/gb.h>
#include <rand.h>
#include "game.h"
#include "script.h"

// --- Wave State Variables ---
uint8_t current_wave;
uint8_t wave_kills;
uint8_t wave_kills_required;
uint8_t wave_complete_timer;

// --- Wave Scripts Configuration ---
// enemy_mask bits: 0=NORMAL, 1=SHOOTER, 2=ZIGZAG, 3=ASTEROID, 4=DIVER
const WaveConfig wave_scripts[MAX_WAVES] = {
    { .enemy_mask = 0x11, .spawn_rate = 90 },  // Wave 1: Normal + Diver (testing)
    { .enemy_mask = 0x05, .spawn_rate = 85 },  // Wave 2: Normal + Zigzag
    { .enemy_mask = 0x03, .spawn_rate = 80 },  // Wave 3: Normal + Shooter
    { .enemy_mask = 0x07, .spawn_rate = 75 },  // Wave 4: Normal + Shooter + Zigzag
    { .enemy_mask = 0x11, .spawn_rate = 70 },  // Wave 5: Normal + Diver
    { .enemy_mask = 0x0F, .spawn_rate = 65 },  // Wave 6: All except Diver
    { .enemy_mask = 0x1F, .spawn_rate = 60 },  // Wave 7: All types
    { .enemy_mask = 0x1F, .spawn_rate = 55 },  // Wave 8: All types, faster
    { .enemy_mask = 0x1F, .spawn_rate = 50 },  // Wave 9: All types, faster
    { .enemy_mask = 0x1F, .spawn_rate = 45 }   // Wave 10: All types, fastest
};

// Initialize the wave system at game start
void init_wave_system(void) {
    current_wave = 1;
    wave_kills = 0;
    wave_kills_required = get_wave_kills_required(current_wave);
    wave_complete_timer = 0;
}

// Advance to the next wave
void advance_wave(void) {
    if (current_wave < MAX_WAVES) {
        current_wave++;
    }
    wave_kills = 0;
    wave_kills_required = get_wave_kills_required(current_wave);
}

// Get kills required for a wave (wave + 4)
uint8_t get_wave_kills_required(uint8_t wave) {
    return wave + 4;
}

// Get a random enemy type allowed for the current wave
uint8_t get_random_enemy_for_wave(uint8_t wave) {
    uint8_t wave_index = wave - 1;
    if (wave_index >= MAX_WAVES) {
        wave_index = MAX_WAVES - 1;
    }

    uint8_t mask = wave_scripts[wave_index].enemy_mask;

    // Count how many enemy types are allowed
    uint8_t allowed_count = 0;
    uint8_t allowed_types[5];

    for (uint8_t i = 0; i < 5; i++) {
        if (mask & (1 << i)) {
            allowed_types[allowed_count] = i;
            allowed_count++;
        }
    }

    // If no types allowed (shouldn't happen), default to NORMAL
    if (allowed_count == 0) {
        return ENEMY_NORMAL;
    }

    // Pick a random allowed type
    uint8_t roll = rand() % allowed_count;
    return allowed_types[roll];
}
