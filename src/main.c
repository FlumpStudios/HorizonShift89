// Horizon Shift 89
// A Game Boy homebrew shooter
// Player defends the center line from enemies attacking from above and below

#include <gb/gb.h>
#include <gbdk/console.h>
#include <stdio.h>
#include <rand.h>
#include "game.h"
#include "sprites.h"

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
    printf("   A: Shoot\n");
    printf("\n");
    printf("   Defend the line!\n");
    printf("   Enemies attack from\n");
    printf("   above AND below.\n");
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

// --- Initialize Game ---
void init_game(void) {
    // Reset game variables
    score = 0;
    lives = 3;
    level = 1;
    frame_count = 0;
    spawn_timer = 60;  // Spawn first enemy after 1 second

    // Initialize subsystems
    init_player();
    init_enemies();
    init_bullets();

    // Load sprite data
    set_sprite_data(0, 4, sprite_data);

    // Enable sprites
    SHOW_SPRITES;

    // Clear screen and draw HUD
    cls();
    draw_center_line();
    update_hud();

    game_state = STATE_PLAYING;
}

// --- Main Update Loop ---
void update_game(void) {
    frame_count++;

    // Spawn enemies periodically
    spawn_timer--;
    if (spawn_timer == 0) {
        spawn_enemy();
        // Faster spawning at higher levels
        spawn_timer = 90 - (level * 10);
        if (spawn_timer < 30) spawn_timer = 30;
    }

    // Update game entities
    update_player();
    update_enemies();
    update_bullets();

    // Check for collisions
    check_collisions();

    // Check game over
    if (lives == 0) {
        game_state = STATE_GAMEOVER;
    }

    // Level up every 500 points
    if (score >= level * 500 && level < 9) {
        level++;
    }
}

// --- Render ---
void render_game(void) {
    render_player();
    render_enemies();
    render_bullets();
}

// --- Main ---
void main(void) {
    // Initialize random seed
    initrand(DIV_REG);

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
                // Simple pause - wait for start
                if (joypad() & J_START) {
                    waitpadup();
                    game_state = STATE_PLAYING;
                }
                break;
        }

        // Wait for VBlank (60fps timing)
        wait_vbl_done();
    }
}
