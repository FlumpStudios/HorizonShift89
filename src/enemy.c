// Enemy module for Horizon Shift 89
// Enemies spawn from top and bottom, move toward center line

#include <gb/gb.h>
#include <rand.h>
#include "game.h"
#include "script.h"

void init_enemies(void) {
    for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
        enemies[i].x = 0;
        enemies[i].y = 0;
        enemies[i].vx = 0;
        enemies[i].vy = 0;
        enemies[i].type = ENEMY_NORMAL;
        enemies[i].shoot_timer = 0;
        enemies[i].knockback = 0;
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

            // Get enemy type based on current wave
            EnemyType enemy_type = get_random_enemy_for_wave(current_wave);

            if (enemy_type == ENEMY_ASTEROID) {
                // Spawn asteroid enemy (falls straight toward center)
                enemies[i].type = ENEMY_ASTEROID;

                // Random X position
                enemies[i].x = ((rand() % (SCREEN_WIDTH - 16)) + 8) << 8;

                // Randomly spawn from top or bottom
                if (rand() & 1) {
                    enemies[i].y = 0;
                    enemies[i].dir = DIR_DOWN;
                    enemies[i].vy = 2 + (rand() & 1);  // Speed 2-3
                } else {
                    enemies[i].y = (SCREEN_HEIGHT) << 8;
                    enemies[i].dir = DIR_UP;
                    enemies[i].vy = -(2 + (rand() & 1));  // Speed 2-3
                }

                enemies[i].vx = 0;  // No horizontal movement
                enemies[i].shoot_timer = 0;

                set_sprite_tile(enemies[i].sprite_id, TILE_ASTEROID);
            } else if (enemy_type == ENEMY_SHOOTER) {
                // Spawn shooter enemy (horizontal mover)
                enemies[i].type = ENEMY_SHOOTER;

                // Spawn from left or right edge
                if (rand() & 1) {
                    // Spawn from left
                    enemies[i].x = 0;
                    enemies[i].vx = SHOOTER_SPEED;
                } else {
                    // Spawn from right
                    enemies[i].x = (SCREEN_WIDTH) << 8;
                    enemies[i].vx = -SHOOTER_SPEED;
                }

                // Random Y position in top or bottom half (not on center line)
                if (rand() & 1) {
                    // Top half - will fire DOWN
                    enemies[i].y = ((rand() % 40) + 16) << 8;
                    enemies[i].dir = DIR_DOWN;
                } else {
                    // Bottom half - will fire UP
                    enemies[i].y = ((rand() % 40) + 88) << 8;
                    enemies[i].dir = DIR_UP;
                }

                enemies[i].vy = 0;
                enemies[i].shoot_timer = SHOOTER_FIRE_RATE / 2;  // First shot sooner

                // Set sprite tile for shooter
                set_sprite_tile(enemies[i].sprite_id, TILE_SHOOTER);
            } else if (enemy_type == ENEMY_ZIGZAG) {
                // Spawn zigzag enemy
                enemies[i].type = ENEMY_ZIGZAG;

                // Random X position (center area so it has room to zigzag)
                enemies[i].x = ((rand() % (SCREEN_WIDTH - 32)) + 16) << 8;

                // Randomly spawn from top or bottom
                if (rand() & 1) {
                    enemies[i].y = 0;
                    enemies[i].dir = DIR_DOWN;
                    enemies[i].vy = ENEMY_SPEED_MIN;
                } else {
                    enemies[i].y = (SCREEN_HEIGHT) << 8;
                    enemies[i].dir = DIR_UP;
                    enemies[i].vy = -ENEMY_SPEED_MIN;
                }

                // Start moving left or right randomly
                enemies[i].vx = (rand() & 1) ? ZIGZAG_SPEED : -ZIGZAG_SPEED;
                enemies[i].shoot_timer = ZIGZAG_FLIP_RATE;

                set_sprite_tile(enemies[i].sprite_id, TILE_ZIGZAG);
            } else {
                // Spawn normal enemy (vertical mover)
                enemies[i].type = ENEMY_NORMAL;

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
                enemies[i].shoot_timer = 0;

                // Set sprite tile for normal enemy
                set_sprite_tile(enemies[i].sprite_id, TILE_ENEMY);
            }

            break;  // Only spawn one enemy
        }
    }
}

void update_enemies(void) {
    for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            // Handle knockback state - enemy flies off screen
            if (enemies[i].knockback) {
                enemies[i].y += enemies[i].vy << 8;
                uint8_t enemy_screen_y = enemies[i].y >> 8;

                // Check if off screen
                if (enemy_screen_y > SCREEN_HEIGHT + 16 || enemy_screen_y < 8) {
                    enemies[i].active = 0;
                    enemies[i].knockback = 0;
                    // Reset to normal palette
                    set_sprite_prop(enemies[i].sprite_id, 0);
                    move_sprite(enemies[i].sprite_id, 0, 0);
                }
                continue;  // Skip normal movement logic
            }

            if (enemies[i].type == ENEMY_SHOOTER) {
                // Shooter enemy - moves horizontally
                enemies[i].x += enemies[i].vx << 8;

                uint8_t enemy_screen_x = enemies[i].x >> 8;

                // Check if shooter went off screen
                if (enemy_screen_x > SCREEN_WIDTH + 8) {
                    enemies[i].active = 0;
                    move_sprite(enemies[i].sprite_id, 0, 0);
                    continue;
                }

                // Handle shooting
                if (enemies[i].shoot_timer > 0) {
                    enemies[i].shoot_timer--;
                } else {
                    // Fire bullet toward center line
                    fire_enemy_bullet(enemies[i].x, enemies[i].y, enemies[i].dir);
                    enemies[i].shoot_timer = SHOOTER_FIRE_RATE;
                }
            } else if (enemies[i].type == ENEMY_ZIGZAG) {
                // Zigzag enemy - moves vertically AND horizontally until it hits the line
                if (enemies[i].vy != 0) {
                    // Still approaching the center line
                    enemies[i].y += enemies[i].vy << 8;
                    enemies[i].x += enemies[i].vx << 8;

                    uint8_t enemy_screen_x = enemies[i].x >> 8;
                    uint8_t enemy_screen_y = enemies[i].y >> 8;

                    // Flip horizontal direction periodically
                    if (enemies[i].shoot_timer > 0) {
                        enemies[i].shoot_timer--;
                    } else {
                        enemies[i].vx = -enemies[i].vx;
                        enemies[i].shoot_timer = ZIGZAG_FLIP_RATE;
                    }

                    // Bounce off screen edges
                    if (enemy_screen_x < 8) {
                        enemies[i].x = 8 << 8;
                        enemies[i].vx = ZIGZAG_SPEED;
                    } else if (enemy_screen_x > SCREEN_WIDTH - 8) {
                        enemies[i].x = (SCREEN_WIDTH - 8) << 8;
                        enemies[i].vx = -ZIGZAG_SPEED;
                    }

                    // Check if reached center line - stop on the line
                    if (enemies[i].dir == DIR_DOWN) {
                        if (enemy_screen_y >= CENTER_LINE_Y - 4) {
                            enemies[i].y = (CENTER_LINE_Y - 4) << 8;
                            enemies[i].vy = 0;
                            enemies[i].vx = 0;
                        }
                    } else {
                        if (enemy_screen_y <= CENTER_LINE_Y + 4) {
                            enemies[i].y = (CENTER_LINE_Y + 4) << 8;
                            enemies[i].vy = 0;
                            enemies[i].vx = 0;
                        }
                    }
                }
                // When vy == 0, zigzag just sits on the line (no movement)
            } else if (enemies[i].type == ENEMY_ASTEROID) {
                // Asteroid enemy - falls straight toward center line
                enemies[i].y += enemies[i].vy << 8;

                uint8_t enemy_screen_x = enemies[i].x >> 8;
                uint8_t enemy_screen_y = enemies[i].y >> 8;

                // Check if reached center line
                if (enemies[i].dir == DIR_DOWN) {
                    if (enemy_screen_y >= CENTER_LINE_Y - 4) {
                        // Asteroid hit the line - destroy sections (1-2 tiles radius)
                        destroy_center_line_section(enemy_screen_x, 1 + (rand() & 1));
                        enemies[i].active = 0;
                        move_sprite(enemies[i].sprite_id, 0, 0);
                        // No life lost from asteroid hitting line
                    }
                } else {
                    if (enemy_screen_y <= CENTER_LINE_Y + 4) {
                        // Asteroid hit the line - destroy sections
                        destroy_center_line_section(enemy_screen_x, 1 + (rand() & 1));
                        enemies[i].active = 0;
                        move_sprite(enemies[i].sprite_id, 0, 0);
                        // No life lost from asteroid hitting line
                    }
                }

                // Check if asteroid went off screen
                if (enemy_screen_y > SCREEN_HEIGHT + 8 || enemy_screen_y < 8) {
                    enemies[i].active = 0;
                    move_sprite(enemies[i].sprite_id, 0, 0);
                }
            } else {
                // Normal enemy - moves vertically toward center line, then patrols horizontally
                if (enemies[i].vy != 0) {
                    // Still approaching the center line
                    enemies[i].y += enemies[i].vy << 8;

                    uint8_t enemy_screen_y = enemies[i].y >> 8;

                    if (enemies[i].dir == DIR_DOWN) {
                        // Coming from top - when hits line, start moving right
                        if (enemy_screen_y >= CENTER_LINE_Y - 4) {
                            enemies[i].y = (CENTER_LINE_Y - 4) << 8;
                            enemies[i].vy = 0;
                            enemies[i].vx = ENEMY_SPEED_MIN;  // Start moving right
                        }
                    } else {
                        // Coming from bottom - when hits line, start moving right
                        if (enemy_screen_y <= CENTER_LINE_Y + 4) {
                            enemies[i].y = (CENTER_LINE_Y + 4) << 8;
                            enemies[i].vy = 0;
                            enemies[i].vx = ENEMY_SPEED_MIN;  // Start moving right
                        }
                    }
                } else {
                    // Patrolling the center line horizontally
                    enemies[i].x += enemies[i].vx << 8;

                    uint8_t enemy_screen_x = enemies[i].x >> 8;

                    // Bounce off screen edges
                    if (enemy_screen_x < 8) {
                        enemies[i].x = 8 << 8;
                        enemies[i].vx = ENEMY_SPEED_MIN;  // Move right
                    } else if (enemy_screen_x > SCREEN_WIDTH - 8) {
                        enemies[i].x = (SCREEN_WIDTH - 8) << 8;
                        enemies[i].vx = -ENEMY_SPEED_MIN;  // Move left
                    }
                }
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
