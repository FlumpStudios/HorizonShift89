// Player module for Horizon Shift 89
// Handles player movement, jumping, dashing, and shooting

#include <gb/gb.h>
#include "game.h"

// Track previous joypad state for edge detection
static uint8_t prev_joy = 0;
static uint8_t tap_timer = 0;

void init_player(void) {
    player.x = SCREEN_WIDTH / 2;
    player.y_offset = 0;
    player.vy = 0;
    player.facing = DIR_UP;
    player.sprite_id = SPRITE_PLAYER;
    player.shoot_cooldown = 0;
    player.is_jumping = 0;
    player.is_dashing = 0;
    player.dash_dir = 0;
    player.dash_timer = 0;
    player.dash_cooldown = 0;
    player.last_lr_time = 0;
    player.last_lr_dir = 0;

    move_sprite(player.sprite_id, player.x + 8, CENTER_LINE_Y + 16);
    set_sprite_tile(player.sprite_id, 0);
}

void player_jump(void) {
    if (!player.is_jumping) {
        player.is_jumping = 1;
        // Jump direction based on facing
        if (player.facing == DIR_UP) {
            player.vy = -JUMP_VELOCITY;
        } else {
            player.vy = JUMP_VELOCITY;
        }
    }
}

void player_dash(int8_t dir) {
    if (!player.is_dashing && player.dash_cooldown == 0) {
        player.is_dashing = 1;
        player.dash_dir = dir;
        player.dash_timer = DASH_DURATION;
        player.dash_cooldown = DASH_COOLDOWN;
    }
}

void update_player(void) {
    uint8_t joy = joypad();
    uint8_t joy_pressed = joy & ~prev_joy;  // Newly pressed buttons

    // Decrement cooldowns
    if (player.shoot_cooldown > 0) player.shoot_cooldown--;
    if (player.dash_cooldown > 0) player.dash_cooldown--;
    if (tap_timer > 0) tap_timer--;

    // --- Dashing ---
    if (player.is_dashing) {
        // Move quickly in dash direction
        player.x += player.dash_dir * DASH_SPEED;

        // Clamp to screen bounds
        if (player.x < 8) player.x = 8;
        if (player.x > SCREEN_WIDTH - 8) player.x = SCREEN_WIDTH - 8;

        player.dash_timer--;
        if (player.dash_timer == 0) {
            player.is_dashing = 0;
        }
    } else {
        // --- Normal Movement (left/right) ---
        if (joy & J_LEFT) {
            // Check for double-tap dash
            if ((joy_pressed & J_LEFT) && player.last_lr_dir == 1 && tap_timer > 0) {
                player_dash(-1);
            } else if (joy_pressed & J_LEFT) {
                player.last_lr_dir = 1;
                tap_timer = DOUBLE_TAP_WINDOW;
            }

            // Normal movement
            if (!player.is_dashing) {
                if (player.x > 8 + PLAYER_SPEED) {
                    player.x -= PLAYER_SPEED;
                } else {
                    player.x = 8;
                }
            }
        }

        if (joy & J_RIGHT) {
            // Check for double-tap dash
            if ((joy_pressed & J_RIGHT) && player.last_lr_dir == 2 && tap_timer > 0) {
                player_dash(1);
            } else if (joy_pressed & J_RIGHT) {
                player.last_lr_dir = 2;
                tap_timer = DOUBLE_TAP_WINDOW;
            }

            // Normal movement
            if (!player.is_dashing) {
                if (player.x < SCREEN_WIDTH - 8 - PLAYER_SPEED) {
                    player.x += PLAYER_SPEED;
                } else {
                    player.x = SCREEN_WIDTH - 8;
                }
            }
        }
    }

    // --- Flip direction (up/down) ---
    if (!player.is_jumping) {
        if ((joy & J_UP) && player.facing != DIR_UP) {
            player.facing = DIR_UP;
            set_sprite_tile(player.sprite_id, 0);
        }
        if ((joy & J_DOWN) && player.facing != DIR_DOWN) {
            player.facing = DIR_DOWN;
            set_sprite_tile(player.sprite_id, 1);
        }
    }

    // --- Jump (A button) ---
    if ((joy_pressed & J_A) && !player.is_jumping) {
        player_jump();
    }

    // --- Jump physics ---
    if (player.is_jumping) {
        player.y_offset += player.vy;

        // Apply gravity (toward center line)
        if (player.facing == DIR_UP) {
            player.vy += GRAVITY;
            // Landed back on line?
            if (player.y_offset >= 0) {
                player.y_offset = 0;
                player.vy = 0;
                player.is_jumping = 0;
            }
            // Cap jump height
            if (player.y_offset < -MAX_JUMP_HEIGHT) {
                player.y_offset = -MAX_JUMP_HEIGHT;
            }
        } else {
            player.vy -= GRAVITY;
            // Landed back on line?
            if (player.y_offset <= 0) {
                player.y_offset = 0;
                player.vy = 0;
                player.is_jumping = 0;
            }
            // Cap jump height
            if (player.y_offset > MAX_JUMP_HEIGHT) {
                player.y_offset = MAX_JUMP_HEIGHT;
            }
        }
    }

    // --- Shooting (B button) ---
    if ((joy & J_B) && player.shoot_cooldown == 0) {
        player_shoot();
        player.shoot_cooldown = 15;
    }

    // --- Pause (Start button) ---
    if (joy & J_START) {
        waitpadup();
        game_state = STATE_PAUSED;
    }

    prev_joy = joy;
}

void player_shoot(void) {
    int16_t bullet_y;

    if (player.facing == DIR_UP) {
        bullet_y = CENTER_LINE_Y + player.y_offset - 8;
    } else {
        bullet_y = CENTER_LINE_Y + player.y_offset + 8;
    }

    fire_bullet(player.x, bullet_y, player.facing);
}

void render_player(void) {
    // Position sprite with y_offset for jumping
    move_sprite(player.sprite_id, player.x + 8, CENTER_LINE_Y + player.y_offset + 16);
}
