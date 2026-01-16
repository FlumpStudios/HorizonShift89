// Bullet module for Horizon Shift 89
// Player bullets that travel up or down

#include <gb/gb.h>
#include "game.h"

void init_bullets(void) {
    for (uint8_t i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
        bullets[i].sprite_id = SPRITE_BULLET_START + i;
        // Hide sprite
        move_sprite(bullets[i].sprite_id, 0, 0);
    }
}

void fire_bullet(int16_t x, int16_t y, Direction dir) {
    // Find inactive bullet slot
    for (uint8_t i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].active = 1;
            bullets[i].x = x << 8;
            bullets[i].y = y << 8;
            bullets[i].dir = dir;

            // Set velocity based on direction
            if (dir == DIR_UP) {
                bullets[i].vy = -BULLET_SPEED;
            } else {
                bullets[i].vy = BULLET_SPEED;
            }
            bullets[i].vx = 0;

            // Set sprite tile (tile 3 for bullets)
            set_sprite_tile(bullets[i].sprite_id, 3);

            break;  // Only fire one bullet
        }
    }
}

void update_bullets(void) {
    for (uint8_t i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            // Move bullet
            bullets[i].y += bullets[i].vy << 8;

            // Check if bullet went off screen
            int16_t bullet_screen_y = bullets[i].y >> 8;
            if (bullet_screen_y < -8 || bullet_screen_y > SCREEN_HEIGHT + 8) {
                bullets[i].active = 0;
                move_sprite(bullets[i].sprite_id, 0, 0);
            }
        }
    }
}

void render_bullets(void) {
    for (uint8_t i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            // Add OAM offset (+8 x, +16 y)
            move_sprite(bullets[i].sprite_id,
                       (bullets[i].x >> 8) + 8,
                       (bullets[i].y >> 8) + 16);
        }
    }
}
