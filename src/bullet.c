// Bullet module for Horizon Shift 89
// Player bullets and enemy bullets

#include <gb/gb.h>
#include "game.h"

// Global enemy bullets array
Entity enemy_bullets[MAX_ENEMY_BULLETS];

void init_bullets(void) {
    for (uint8_t i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
        bullets[i].x = 0;
        bullets[i].y = 0;
        bullets[i].vx = 0;
        bullets[i].vy = 0;
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

            // Check if bullet went off screen (unsigned, so check wrap-around)
            uint16_t bullet_y = bullets[i].y >> 8;
            if (bullet_y > SCREEN_HEIGHT + 8) {
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

// --- Enemy Bullets ---

void init_enemy_bullets(void) {
    for (uint8_t i = 0; i < MAX_ENEMY_BULLETS; i++) {
        enemy_bullets[i].active = 0;
        enemy_bullets[i].x = 0;
        enemy_bullets[i].y = 0;
        enemy_bullets[i].vx = 0;
        enemy_bullets[i].vy = 0;
        enemy_bullets[i].sprite_id = SPRITE_ENEMY_BULLET_START + i;
        move_sprite(enemy_bullets[i].sprite_id, 0, 0);
    }
}

void fire_enemy_bullet(uint16_t x, uint16_t y, Direction dir) {
    for (uint8_t i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (!enemy_bullets[i].active) {
            enemy_bullets[i].active = 1;
            enemy_bullets[i].x = x;
            enemy_bullets[i].y = y;
            enemy_bullets[i].dir = dir;

            if (dir == DIR_UP) {
                enemy_bullets[i].vy = -ENEMY_BULLET_SPEED;
            } else {
                enemy_bullets[i].vy = ENEMY_BULLET_SPEED;
            }
            enemy_bullets[i].vx = 0;

            set_sprite_tile(enemy_bullets[i].sprite_id, TILE_ENEMY_BULLET);
            break;
        }
    }
}

void update_enemy_bullets(void) {
    for (uint8_t i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (enemy_bullets[i].active) {
            enemy_bullets[i].y += enemy_bullets[i].vy << 8;

            uint16_t bullet_y = enemy_bullets[i].y >> 8;
            if (bullet_y > SCREEN_HEIGHT + 8) {
                enemy_bullets[i].active = 0;
                move_sprite(enemy_bullets[i].sprite_id, 0, 0);
            }
        }
    }
}

void render_enemy_bullets(void) {
    for (uint8_t i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (enemy_bullets[i].active) {
            move_sprite(enemy_bullets[i].sprite_id,
                       (enemy_bullets[i].x >> 8) + 8,
                       (enemy_bullets[i].y >> 8) + 16);
        }
    }
}
