// Enemy module for Horizon Shift 89
// Enemies spawn from top and bottom, move toward center line

#include <gb/gb.h>
#include <rand.h>
#include "game.h"

void init_enemies(void) {
    for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
        enemies[i].sprite_id = SPRITE_ENEMY_START + i;
        // Hide sprite
        move_sprite(enemies[i].sprite_id, 0, 0);
    }
}

void spawn_enemy(void) {
    // Find inactive enemy slot
    for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].active = 1;

            // Random X position
            enemies[i].x = ((rand() % (SCREEN_WIDTH - 16)) + 8) << 8;

            // Randomly spawn from top or bottom
            if (rand() & 1) {
                // Spawn from top
                enemies[i].y = 0 << 8;
                enemies[i].dir = DIR_DOWN;  // Moving down toward center
                enemies[i].vy = ENEMY_SPEED_MIN + (rand() % (level));
                if (enemies[i].vy > ENEMY_SPEED_MAX) {
                    enemies[i].vy = ENEMY_SPEED_MAX;
                }
            } else {
                // Spawn from bottom
                enemies[i].y = (SCREEN_HEIGHT) << 8;
                enemies[i].dir = DIR_UP;  // Moving up toward center
                enemies[i].vy = -(ENEMY_SPEED_MIN + (rand() % (level)));
                if (enemies[i].vy < -ENEMY_SPEED_MAX) {
                    enemies[i].vy = -ENEMY_SPEED_MAX;
                }
            }

            enemies[i].vx = 0;

            // Set sprite tile (tile 2 for enemies)
            set_sprite_tile(enemies[i].sprite_id, 2);

            break;  // Only spawn one enemy
        }
    }
}

void update_enemies(void) {
    for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            // Move toward center line
            enemies[i].y += enemies[i].vy << 8;

            // Check if enemy reached center line (player loses life)
            int16_t enemy_screen_y = enemies[i].y >> 8;

            if (enemies[i].dir == DIR_DOWN) {
                // Coming from top
                if (enemy_screen_y >= CENTER_LINE_Y - 4) {
                    // Enemy hit the line!
                    enemies[i].active = 0;
                    move_sprite(enemies[i].sprite_id, 0, 0);

                    if (lives > 0) {
                        lives--;
                        update_hud();
                    }
                }
            } else {
                // Coming from bottom
                if (enemy_screen_y <= CENTER_LINE_Y + 4) {
                    // Enemy hit the line!
                    enemies[i].active = 0;
                    move_sprite(enemies[i].sprite_id, 0, 0);

                    if (lives > 0) {
                        lives--;
                        update_hud();
                    }
                }
            }

            // Check if enemy went off screen (shouldn't happen normally)
            if (enemy_screen_y < -8 || enemy_screen_y > SCREEN_HEIGHT + 8) {
                enemies[i].active = 0;
                move_sprite(enemies[i].sprite_id, 0, 0);
            }
        }
    }
}

void render_enemies(void) {
    for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            // Add OAM offset (+8 x, +16 y)
            move_sprite(enemies[i].sprite_id,
                       (enemies[i].x >> 8) + 8,
                       (enemies[i].y >> 8) + 16);
        }
    }
}
