// Collision detection for Horizon Shift 89

#include <gb/gb.h>
#include "game.h"
#include "script.h"

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
    // Check player bullet vs enemy collisions
    for (uint8_t b = 0; b < MAX_BULLETS; b++) {
        if (!bullets[b].active) continue;

        for (uint8_t e = 0; e < MAX_ENEMIES; e++) {
            if (!enemies[e].active) continue;
            if (enemies[e].knockback) continue;  // Knockback enemies are invulnerable

            // For shooter and asteroid enemies, can hit from any direction
            // For normal enemies, must match direction
            uint8_t can_hit = 0;
            if (enemies[e].type == ENEMY_SHOOTER || enemies[e].type == ENEMY_ASTEROID) {
                can_hit = 1;  // Shooters and asteroids can be hit by any bullet
            } else {
                // Normal enemies: bullet going up hits enemies from top, etc.
                if ((bullets[b].dir == DIR_UP && enemies[e].dir == DIR_DOWN) ||
                    (bullets[b].dir == DIR_DOWN && enemies[e].dir == DIR_UP)) {
                    can_hit = 1;
                }
            }

            if (can_hit && check_collision(&bullets[b], &enemies[e])) {
                // Destroy both bullet and enemy
                bullets[b].active = 0;
                move_sprite(bullets[b].sprite_id, 0, 0);

                enemies[e].active = 0;
                move_sprite(enemies[e].sprite_id, 0, 0);

                // Track wave kills
                wave_kills++;

                // Add score (more for special enemies)
                if (enemies[e].type == ENEMY_SHOOTER) {
                    score += 25;
                } else if (enemies[e].type == ENEMY_ASTEROID) {
                    score += 20;
                } else if (enemies[e].type == ENEMY_ZIGZAG) {
                    score += 15;
                } else {
                    score += 10;
                }
                update_hud();

                break;  // Bullet can only hit one enemy
            }
        }
    }

    // Get player position (player.x is already screen coords)
    // Include facing offset with extra pixel separation to prevent cross-side collisions
    int8_t facing_offset = (player.facing == DIR_UP) ? -4 : 3;
    uint8_t player_x = player.x;
    uint8_t player_y = CENTER_LINE_Y + player.y_offset + facing_offset;

    // Check enemy bullet vs player collisions
    for (uint8_t b = 0; b < MAX_ENEMY_BULLETS; b++) {
        if (!enemy_bullets[b].active) continue;

        // Get enemy bullet position
        uint8_t bullet_x = enemy_bullets[b].x >> 8;
        uint8_t bullet_y = enemy_bullets[b].y >> 8;

        // Simple AABB collision
        if (player_x < bullet_x + HITBOX_SIZE &&
            player_x + HITBOX_SIZE > bullet_x &&
            player_y < bullet_y + HITBOX_SIZE &&
            player_y + HITBOX_SIZE > bullet_y) {

            // Player hit by enemy bullet
            enemy_bullets[b].active = 0;
            move_sprite(enemy_bullets[b].sprite_id, 0, 0);

            if (lives > 0) {
                lives--;
                update_hud();
            }
        }
    }

    // Check enemy vs player collisions
    for (uint8_t e = 0; e < MAX_ENEMIES; e++) {
        if (!enemies[e].active) continue;
        if (enemies[e].knockback) continue;  // Already in knockback, skip

        // Get enemy position
        uint8_t enemy_x = enemies[e].x >> 8;
        uint8_t enemy_y = enemies[e].y >> 8;

        // Simple AABB collision
        if (player_x < enemy_x + HITBOX_SIZE &&
            player_x + HITBOX_SIZE > enemy_x &&
            player_y < enemy_y + HITBOX_SIZE &&
            player_y + HITBOX_SIZE > enemy_y) {

            if (player.is_dashing) {
                // Dash-kill! Set enemy to knockback state
                enemies[e].knockback = 1;
                enemies[e].vx = 0;
                // Send enemy flying back the way it came
                if (enemies[e].dir == DIR_DOWN) {
                    enemies[e].vy = -KNOCKBACK_SPEED;  // Fly up
                } else {
                    enemies[e].vy = KNOCKBACK_SPEED;   // Fly down
                }
                // Switch to lighter palette
                set_sprite_prop(enemies[e].sprite_id, S_PALETTE);
            } else {
                // Normal collision - player loses life
                enemies[e].active = 0;
                move_sprite(enemies[e].sprite_id, 0, 0);

                if (lives > 0) {
                    lives--;
                    update_hud();
                }
            }
        }
    }

    // Check knockback enemy vs normal enemy collisions
    for (uint8_t k = 0; k < MAX_ENEMIES; k++) {
        if (!enemies[k].active || !enemies[k].knockback) continue;

        uint8_t kb_x = enemies[k].x >> 8;
        uint8_t kb_y = enemies[k].y >> 8;

        for (uint8_t e = 0; e < MAX_ENEMIES; e++) {
            if (!enemies[e].active || enemies[e].knockback) continue;  // Skip inactive and other knockback enemies
            if (k == e) continue;  // Don't check against self

            uint8_t enemy_x = enemies[e].x >> 8;
            uint8_t enemy_y = enemies[e].y >> 8;

            // Simple AABB collision
            if (kb_x < enemy_x + HITBOX_SIZE &&
                kb_x + HITBOX_SIZE > enemy_x &&
                kb_y < enemy_y + HITBOX_SIZE &&
                kb_y + HITBOX_SIZE > enemy_y) {

                // Knockback enemy hit a normal enemy - destroy both with 3x bonus
                enemies[e].active = 0;
                move_sprite(enemies[e].sprite_id, 0, 0);

                enemies[k].active = 0;
                enemies[k].knockback = 0;
                set_sprite_prop(enemies[k].sprite_id, 0);  // Reset palette
                move_sprite(enemies[k].sprite_id, 0, 0);

                // Track wave kills (both enemies count)
                wave_kills += 2;

                // 3x score bonus for the hit enemy
                if (enemies[e].type == ENEMY_SHOOTER) {
                    score += 75;  // 25 * 3
                } else if (enemies[e].type == ENEMY_ASTEROID) {
                    score += 60;  // 20 * 3
                } else if (enemies[e].type == ENEMY_ZIGZAG) {
                    score += 45;  // 15 * 3
                } else {
                    score += 30;  // 10 * 3
                }
                update_hud();

                break;  // Knockback enemy can only hit one enemy
            }
        }
    }
}
