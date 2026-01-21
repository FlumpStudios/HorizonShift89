// Horizon Shift 89
// A Game Boy homebrew shooter
// Player defends the center line from enemies attacking from above and below

#include <gb/gb.h>
#include <gbdk/console.h>
#include <stdio.h>
#include <rand.h>
#include "game.h"
#include "sprites.h"
#include "script.h"

// --- Global Variables ---
GameState game_state;
Player player;
Entity enemies[MAX_ENEMIES];
Entity bullets[MAX_BULLETS];
uint16_t score;
uint8_t lives;
uint8_t level;
uint16_t frame_count;
uint8_t spawn_timer;
uint8_t prev_level;

// --- Title Screen ---
void show_title(void) {
    cls();
    printf("\n\n\n");
    printf("   HORIZON SHIFT 89\n");
    printf("\n");
    printf("   ----------------\n");
    printf("\n");
    printf("   LEFT/RIGHT: Move\n");
    printf("   UP/DOWN: Aim\n");
    printf("   A: Jump\n");
    printf("   B: Shoot\n");
    printf("   Double-tap: Dash\n");
    printf("\n");
    printf("   Defend the line!\n");
    printf("\n\n");
    printf("   Press START");

    waitpad(J_START);
    waitpadup();
}

// --- Game Over Screen ---
void show_gameover(void) {
    cls();
    printf("\n\n\n\n\n");
    printf("     GAME OVER\n");
    printf("\n");
    printf("     Score: %u\n", score);
    printf("\n\n");
    printf("   Press START");

    waitpad(J_START);
    waitpadup();
}

// --- Wave Complete Screen ---
void show_wave_complete(void) {
    gotoxy(3, 8);
    printf("WAVE COMPLETE!");
}

// --- Initialize Game ---
void init_game(void) {
    // Reset game variables
    score = 0;
    lives = 3;
    level = 1;
    prev_level = 1;
    frame_count = 0;
    spawn_timer = 60;  // Spawn first enemy after 1 second

    // Initialize subsystems
    init_player();
    init_enemies();
    init_bullets();
    init_enemy_bullets();
    reset_center_line();
    init_wave_system();

    // Load sprite data (9 tiles: player up/down, enemy, bullet, shooter, enemy bullet, zigzag, asteroid, star)
    set_sprite_data(0, 9, sprite_data);

    // Enable sprites
    SHOW_SPRITES;

    // Clear screen and draw HUD
    cls();
    draw_center_line();
    update_hud();

    // Initialize starfield background (after cls so stars aren't erased)
    init_starfield();

    game_state = STATE_PLAYING;
}

// --- Clear All Active Enemies and Bullets ---
void clear_all_entities(void) {
    // Clear all enemies
    for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            enemies[i].active = 0;
            move_sprite(enemies[i].sprite_id, 0, 0);
        }
    }
    // Clear all player bullets
    for (uint8_t i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].active = 0;
            move_sprite(bullets[i].sprite_id, 0, 0);
        }
    }
    // Clear all enemy bullets
    for (uint8_t i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (enemy_bullets[i].active) {
            enemy_bullets[i].active = 0;
            move_sprite(enemy_bullets[i].sprite_id, 0, 0);
        }
    }
}

// --- Main Update Loop ---
void update_game(void) {
    frame_count++;

    // Spawn enemies periodically using wave spawn rate
    spawn_timer--;
    if (spawn_timer == 0) {
        spawn_enemy();
        // Get spawn rate from wave scripts
        uint8_t wave_index = current_wave - 1;
        if (wave_index >= MAX_WAVES) {
            wave_index = MAX_WAVES - 1;
        }
        spawn_timer = wave_scripts[wave_index].spawn_rate;
    }

    // Update game entities
    update_player();
    update_enemies();
    update_bullets();
    update_enemy_bullets();

    // Update starfield twinkling
    update_starfield();

    // Check for collisions
    check_collisions();

    // Check game over
    if (lives == 0) {
        game_state = STATE_GAMEOVER;
    }

    // Check wave completion
    if (wave_kills >= wave_kills_required) {
        // Wave complete! Clear all enemies and bullets
        clear_all_entities();
        wave_complete_timer = 90;  // ~1.5 seconds at 60fps
        game_state = STATE_WAVE_COMPLETE;
    }
}

// --- Render ---
void render_game(void) {
    render_player();
    render_enemies();
    render_bullets();
    render_enemy_bullets();
}

// --- Main ---
void main(void) {
    // Disable interrupts during setup
    disable_interrupts();

    // Set up display
    DISPLAY_ON;

    enable_interrupts();

    // Main game loop
    while(1) {
        switch(game_state) {
            case STATE_TITLE:
                show_title();
                // Seed random AFTER title screen - user timing adds entropy
                initrand(DIV_REG);
                init_game();
                break;

            case STATE_PLAYING:
                update_game();
                render_game();
                break;

            case STATE_GAMEOVER:
                // Hide all sprites
                for(uint8_t i = 0; i < 40; i++) {
                    move_sprite(i, 0, 0);
                }
                show_gameover();
                game_state = STATE_TITLE;
                break;

            case STATE_PAUSED:
                // Show paused text
                printf("\n\n\n\n\n\n\n      PAUSED");
                waitpad(J_START);
                waitpadup();
                // Restore screen
                cls();
                draw_center_line();
                update_hud();
                game_state = STATE_PLAYING;
                break;

            case STATE_WAVE_COMPLETE:
                // Show wave complete message on first frame
                if (wave_complete_timer == 90) {
                    show_wave_complete();
                }

                // Count down the timer
                wave_complete_timer--;

                // When timer expires, advance to next wave
                if (wave_complete_timer == 0) {
                    advance_wave();
                    reset_center_line();
                    spawn_timer = 60;  // Give player a moment before spawns resume

                    // Restore screen
                    cls();
                    draw_center_line();
                    update_hud();

                    // Re-initialize starfield
                    init_starfield();

                    game_state = STATE_PLAYING;
                }
                break;
        }

        // Wait for VBlank (60fps timing)
        wait_vbl_done();
    }
}
