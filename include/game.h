#ifndef GAME_H
#define GAME_H

#include <gb/gb.h>
#include <stdint.h>

// --- Screen Constants ---
#define SCREEN_WIDTH        160
#define SCREEN_HEIGHT       144
#define CENTER_LINE_Y       72      // Middle of screen (144/2)

// --- Game Constants ---
#define MAX_ENEMIES         8
#define MAX_BULLETS         4
#define PLAYER_SPEED        2
#define BULLET_SPEED        4
#define ENEMY_SPEED_MIN     1
#define ENEMY_SPEED_MAX     2

// --- Sprite Constants ---
#define SPRITE_PLAYER       0
#define SPRITE_BULLET_START 1
#define SPRITE_ENEMY_START  5       // Sprites 5-12 for enemies

// --- Direction ---
typedef enum {
    DIR_UP = 0,
    DIR_DOWN = 1
} Direction;

// --- Game States ---
typedef enum {
    STATE_TITLE,
    STATE_PLAYING,
    STATE_GAMEOVER,
    STATE_PAUSED
} GameState;

// --- Entity Structure ---
typedef struct {
    int16_t x;          // X position (fixed point 8.8 for smooth movement)
    int16_t y;          // Y position (fixed point 8.8)
    int8_t  vx;         // X velocity
    int8_t  vy;         // Y velocity
    uint8_t active;     // Is entity active?
    uint8_t sprite_id;  // Hardware sprite index
    Direction dir;      // Direction facing (for enemies: which side they come from)
} Entity;

// --- Player Structure ---
typedef struct {
    int16_t x;              // X position on center line
    Direction facing;       // UP or DOWN
    uint8_t sprite_id;
    uint8_t shoot_cooldown; // Frames until can shoot again
} Player;

// --- Global Game State ---
extern GameState game_state;
extern Player player;
extern Entity enemies[MAX_ENEMIES];
extern Entity bullets[MAX_BULLETS];
extern uint16_t score;
extern uint8_t lives;
extern uint8_t level;
extern uint16_t frame_count;

// --- Function Prototypes ---
// main.c
void init_game(void);
void update_game(void);
void render_game(void);

// player.c
void init_player(void);
void update_player(void);
void render_player(void);
void player_shoot(void);
void player_flip(void);

// enemy.c
void init_enemies(void);
void update_enemies(void);
void render_enemies(void);
void spawn_enemy(void);

// bullet.c
void init_bullets(void);
void update_bullets(void);
void render_bullets(void);
void fire_bullet(int16_t x, int16_t y, Direction dir);

// collision.c
void check_collisions(void);
uint8_t check_collision(Entity* a, Entity* b);

// utils.c
void draw_center_line(void);
void update_hud(void);

#endif
