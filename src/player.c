// Player module for Horizon Shift 89
// Handles player movement, flipping, and shooting

#include <gb/gb.h>
#include "game.h"


void init_player(void) {
    player.x = SCREEN_WIDTH / 2;  // Center of screen (pixel coordinates)
    player.facing = DIR_UP;
    player.sprite_id = SPRITE_PLAYER;
    player.shoot_cooldown = 0;

    // Position sprite on center line
    move_sprite(player.sprite_id, player.x + 8, CENTER_LINE_Y + 16);

    // Set initial sprite tile (facing up)
    set_sprite_tile(player.sprite_id, 0);
}

void update_player(void) {
    uint8_t joy = joypad();

    // Movement (left/right on center line)
    if (joy & J_LEFT) {
        if (player.x > 8 + PLAYER_SPEED) {
            player.x -= PLAYER_SPEED;
        } else {
            player.x = 8;
        }
    }
    if (joy & J_RIGHT) {
        if (player.x < SCREEN_WIDTH - 8 - PLAYER_SPEED) {
            player.x += PLAYER_SPEED;
        } else {
            player.x = SCREEN_WIDTH - 8;
        }
    }

    // Flip direction (up/down)
    if ((joy & J_UP) && player.facing != DIR_UP) {
        player.facing = DIR_UP;
        set_sprite_tile(player.sprite_id, 0);
    }
    if ((joy & J_DOWN) && player.facing != DIR_DOWN) {
        player.facing = DIR_DOWN;
        set_sprite_tile(player.sprite_id, 1);
    }

    // Shooting (A button)
    if (player.shoot_cooldown > 0) {
        player.shoot_cooldown--;
    }
    if ((joy & J_A) && player.shoot_cooldown == 0) {
        player_shoot();
        player.shoot_cooldown = 15;  // Fire rate limit
    }

    // Pause (Start button)
    if (joy & J_START) {
        waitpadup();
        game_state = STATE_PAUSED;
    }
}

void player_flip(void) {
    if (player.facing == DIR_UP) {
        player.facing = DIR_DOWN;
        // Flip sprite vertically (use tile 1 for down-facing)
        set_sprite_tile(player.sprite_id, 1);
    } else {
        player.facing = DIR_UP;
        set_sprite_tile(player.sprite_id, 0);
    }
}

void player_shoot(void) {
    int16_t bullet_y;

    if (player.facing == DIR_UP) {
        bullet_y = CENTER_LINE_Y - 8;  // Shoot upward
    } else {
        bullet_y = CENTER_LINE_Y + 8;  // Shoot downward
    }

    fire_bullet(player.x, bullet_y, player.facing);
}

void render_player(void) {
    // Move sprite to player position
    // Add 8 to x and 16 to y for OAM offset
    move_sprite(player.sprite_id, player.x + 8, CENTER_LINE_Y + 16);
}
