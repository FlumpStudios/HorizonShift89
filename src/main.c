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
uint8_t countdown_timer;
uint8_t controls_swapped;  // 0 = normal (A=jump, B=shoot), 1 = swapped
uint8_t splash_timer;      // Timer for splash screen phases

// --- Splash Screen ---
void show_splash(void) {
    // Splash screen phases:
    // 0-30: fade in (white to normal)
    // 31-120: hold
    // 121-150: fade out (normal to white)

    if (splash_timer == 0) {
        // Initial setup - start with white screen
        BGP_REG = 0xFF;  // All white
        cls();
        gotoxy(3, 8);
        printf("Flump Studios");
    }

    splash_timer++;

    // Fade in (frames 1-30)
    if (splash_timer <= 10) {
        BGP_REG = 0xFF;  // All white
    } else if (splash_timer <= 20) {
        BGP_REG = 0xFE;  // Slightly darker
    } else if (splash_timer <= 30) {
        BGP_REG = 0xF9;  // More contrast
    } else if (splash_timer <= 40) {
        BGP_REG = 0xE4;  // Normal palette
    }
    // Hold (frames 41-110)
    else if (splash_timer <= 110) {
        BGP_REG = 0xE4;  // Normal palette
    }
    // Fade out (frames 111-150)
    else if (splash_timer <= 120) {
        BGP_REG = 0xF9;
    } else if (splash_timer <= 130) {
        BGP_REG = 0xFE;
    } else if (splash_timer <= 140) {
        BGP_REG = 0xFF;  // All white
    } else if (splash_timer >= 150) {
        // Done - go to title
        BGP_REG = 0xE4;  // Reset to normal
        splash_timer = 0;
        game_state = STATE_TITLE;
    }
}

// --- Title Screen ---
void show_title(void) {
    uint8_t menu_selection = 0;  // 0 = Start, 1 = Switch controls
    uint8_t joy, prev_joy_title = 0;

    cls();
    gotoxy(1, 3);
    printf("Horizon Shift '89");
    gotoxy(1, 5);
    printf("-----------------");

    // Draw menu options
    gotoxy(1, 9);
    printf("> Start");
    gotoxy(1, 11);
    printf("  Switch controls");

    // Show current control scheme
    gotoxy(3, 14);
    if (controls_swapped) {
        printf("A:Shoot  B:Jump");
    } else {
        printf("A:Jump   B:Shoot");
    }

    while(1) {
        joy = joypad();
        uint8_t joy_pressed = joy & ~prev_joy_title;

        // Navigate menu
        if (joy_pressed & J_UP) {
            if (menu_selection > 0) {
                menu_selection--;
            }
        }
        if (joy_pressed & J_DOWN) {
            if (menu_selection < 1) {
                menu_selection++;
            }
        }

        // Update cursor
        gotoxy(1, 9);
        printf(menu_selection == 0 ? ">" : " ");
        gotoxy(1, 11);
        printf(menu_selection == 1 ? ">" : " ");

        // Select option
        if ((joy_pressed & J_START) || (joy_pressed & J_A)) {
            if (menu_selection == 0) {
                // Start game
                break;
            } else {
                // Toggle controls
                controls_swapped = !controls_swapped;
                // Update display
                gotoxy(3, 14);
                if (controls_swapped) {
                    printf("A:Shoot  B:Jump");
                } else {
                    printf("A:Jump   B:Shoot");
                }
            }
        }

        prev_joy_title = joy;
        wait_vbl_done();
    }

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

    // Load sprite data (10 tiles: player up/down, enemy, bullet, shooter, enemy bullet, zigzag, asteroid, star, diver)
    set_sprite_data(0, 10, sprite_data);

    // Set up sprite palettes
    OBP0_REG = 0xE4;  // Normal: black, dark gray, light gray, white
    OBP1_REG = 0x90;  // Lighter (knockback): black, light gray, white, white

    // Enable sprites
    SHOW_SPRITES;

    // Clear screen and draw HUD
    cls();
    draw_center_line();
    update_hud();

    // Initialize starfield background (after cls so stars aren't erased)
    init_starfield();

    // Start countdown sequence
    countdown_timer = 240;  // 4 seconds total (60fps * 4)
    game_state = STATE_COUNTDOWN;
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
        wave_complete_timer = 150;  // ~2.5 seconds at 60fps
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

    // Initialize controls
    controls_swapped = 0;
    splash_timer = 0;
    game_state = STATE_SPLASH;

    enable_interrupts();

    // Main game loop
    while(1) {
        switch(game_state) {
            case STATE_SPLASH:
                show_splash();
                break;

            case STATE_TITLE:
                show_title();
                // Seed random AFTER title screen - user timing adds entropy
                initrand(DIV_REG);
                init_game();
                break;

            case STATE_COUNTDOWN:
                // Display countdown: 3, 2, 1, GET READY!
                // 240-181: "3", 180-121: "2", 120-61: "1", 60-1: "GET READY!"
                if (countdown_timer == 240) {
                    gotoxy(8, 7);
                    printf("  3  ");
                } else if (countdown_timer == 180) {
                    gotoxy(8, 7);
                    printf("  2  ");
                } else if (countdown_timer == 120) {
                    gotoxy(8, 7);
                    printf("  1  ");
                } else if (countdown_timer == 60) {
                    gotoxy(6, 7);
                    printf("GET READY!");
                }

                countdown_timer--;

                if (countdown_timer == 0) {
                    // Clear the countdown text and start game
                    gotoxy(6, 7);
                    printf("          ");
                    game_state = STATE_PLAYING;
                }
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
                if (wave_complete_timer == 150) {
                    show_wave_complete();
                }

                // After 1 second, show the next wave number
                if (wave_complete_timer == 90) {
                    gotoxy(3, 8);
                    printf("   Wave %u     ", current_wave + 1);
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
