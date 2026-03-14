// Explosion effect pool for Horizon Shift 89
// 2 reusable sprites animate through 3 frames when an enemy is destroyed.

#include <gb/gb.h>
#include "game.h"

Explosion explosions[MAX_EXPLOSIONS];

void init_explosions(void) {
    for (uint8_t i = 0; i < MAX_EXPLOSIONS; i++) {
        explosions[i].active = 0;
        explosions[i].timer = 0;
        move_sprite(SPRITE_EXPLOSION_START + i, 0, 0);
    }
}

void trigger_explosion(uint8_t x, uint8_t y) {
    // Find a free slot; if all busy, overwrite slot 0
    uint8_t slot = 0;
    for (uint8_t i = 0; i < MAX_EXPLOSIONS; i++) {
        if (!explosions[i].active) {
            slot = i;
            break;
        }
    }
    explosions[slot].x = x;
    explosions[slot].y = y;
    explosions[slot].timer = EXPLOSION_FRAME_DURATION * 3;
    explosions[slot].active = 1;
    set_sprite_tile(SPRITE_EXPLOSION_START + slot, TILE_EXPLOSION_0);
}

void update_explosions(void) {
    for (uint8_t i = 0; i < MAX_EXPLOSIONS; i++) {
        if (!explosions[i].active) continue;

        explosions[i].timer--;

        if (explosions[i].timer == 0) {
            explosions[i].active = 0;
            move_sprite(SPRITE_EXPLOSION_START + i, 0, 0);
        } else {
            // Advance animation frame based on remaining time
            uint8_t elapsed = EXPLOSION_FRAME_DURATION * 3 - explosions[i].timer;
            uint8_t frame = elapsed / EXPLOSION_FRAME_DURATION;
            set_sprite_tile(SPRITE_EXPLOSION_START + i, TILE_EXPLOSION_0 + frame);
        }
    }
}

void render_explosions(void) {
    for (uint8_t i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i].active) {
            move_sprite(SPRITE_EXPLOSION_START + i,
                        explosions[i].x + 8,
                        explosions[i].y + 16);
        }
    }
}
