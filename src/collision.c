// Collision detection for Horizon Shift 89

#include <gb/gb.h>
#include "game.h"

#define HITBOX_SIZE 6  // Collision box size in pixels

// Simple AABB collision check
uint8_t check_collision(Entity* a, Entity* b) {
    uint8_t ax = a->x >> 8;
    uint8_t ay = a->y >> 8;
    uint8_t bx = b->x >> 8;
    uint8_t by = b->y >> 8;

    // Check if bounding boxes overlap
    if (ax < bx + HITBOX_SIZE &&
        ax + HITBOX_SIZE > bx &&
        ay < by + HITBOX_SIZE &&
        ay + HITBOX_SIZE > by) {
        return 1;
    }
    return 0;
}

void check_collisions(void) {
    // Check bullet vs enemy collisions
    for (uint8_t b = 0; b < MAX_BULLETS; b++) {
        if (!bullets[b].active) continue;

        for (uint8_t e = 0; e < MAX_ENEMIES; e++) {
            if (!enemies[e].active) continue;

            // Only check collisions if bullet and enemy are on same side
            // Bullet going up hits enemies from top, bullet going down hits enemies from bottom
            if ((bullets[b].dir == DIR_UP && enemies[e].dir == DIR_DOWN) ||
                (bullets[b].dir == DIR_DOWN && enemies[e].dir == DIR_UP)) {

                if (check_collision(&bullets[b], &enemies[e])) {
                    // Destroy both bullet and enemy
                    bullets[b].active = 0;
                    move_sprite(bullets[b].sprite_id, 0, 0);

                    enemies[e].active = 0;
                    move_sprite(enemies[e].sprite_id, 0, 0);

                    // Add score
                    score += 10;
                    update_hud();

                    break;  // Bullet can only hit one enemy
                }
            }
        }
    }
}
