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
#define MAX_ENEMY_BULLETS   4
#define PLAYER_SPEED        2
#define BULLET_SPEED        4
#define ENEMY_BULLET_SPEED  3
#define ENEMY_SPEED_MIN     1
#define ENEMY_SPEED_MAX     2
#define SHOOTER_SPEED       1
#define SHOOTER_FIRE_RATE   90  // Frames between shots
#define ZIGZAG_SPEED        2   // Horizontal oscillation speed
#define ZIGZAG_FLIP_RATE    20  // Frames between direction flips

// --- Jump Constants ---
#define JUMP_VELOCITY       6
#define GRAVITY             1
#define MAX_JUMP_HEIGHT     32

// --- Dash Constants ---
#define DASH_SPEED          8
#define DASH_DURATION       8
#define DASH_COOLDOWN       20
#define DOUBLE_TAP_WINDOW   12
#define KNOCKBACK_SPEED     6   // Speed enemies fly back when dash-killed

// --- Sprite Constants ---
#define SPRITE_PLAYER       0
#define SPRITE_BULLET_START 1
#define SPRITE_ENEMY_START  5       // Sprites 5-12 for enemies
#define SPRITE_ENEMY_BULLET_START 13  // Sprites 13-16 for enemy bullets

// --- Tile Constants ---
#define TILE_PLAYER_UP      0
#define TILE_PLAYER_DOWN    1
#define TILE_ENEMY          2
#define TILE_BULLET         3
#define TILE_SHOOTER        4
#define TILE_ENEMY_BULLET   5
#define TILE_ZIGZAG         6
#define TILE_ASTEROID       7
#define TILE_STAR           8

// --- Starfield Constants ---
#define NUM_STARS           20
#define TWINKLE_RATE        8   // Frames between twinkle updates

// --- Center Line Constants ---
#define CENTER_LINE_TILES   20

// --- Direction ---
typedef enum {
    DIR_UP = 0,
    DIR_DOWN = 1
} Direction;

// --- Enemy Types ---
typedef enum {
    ENEMY_NORMAL = 0,
    ENEMY_SHOOTER = 1,
    ENEMY_ZIGZAG = 2,
    ENEMY_ASTEROID = 3
} EnemyType;

// --- Game States ---
typedef enum {
    STATE_TITLE,
    STATE_COUNTDOWN,
    STATE_PLAYING,
    STATE_GAMEOVER,
    STATE_PAUSED,
    STATE_WAVE_COMPLETE
} GameState;

// --- Entity Structure ---
typedef struct {
    uint16_t x;         // X position (fixed point 8.8 for smooth movement)
    uint16_t y;         // Y position (fixed point 8.8)
    int8_t  vx;         // X velocity
    int8_t  vy;         // Y velocity
    uint8_t active;     // Is entity active?
    uint8_t sprite_id;  // Hardware sprite index
    Direction dir;      // Direction facing (for enemies: which side they come from)
    uint8_t type;       // EnemyType (for enemies only)
    uint8_t shoot_timer; // Frames until next shot (for shooter enemies)
    uint8_t knockback;  // Is enemy in knockback state? (hit by dashing player)
} Entity;

// --- Player Structure ---
typedef struct {
    int16_t x;              // X position on center line
    int8_t y_offset;        // Y offset from center line (for jumping)
    int8_t vy;              // Vertical velocity
    Direction facing;       // UP or DOWN
    uint8_t sprite_id;
    uint8_t shoot_cooldown; // Frames until can shoot again
    uint8_t is_jumping;     // Currently in a jump?
    uint8_t is_dashing;     // Currently dashing?
    int8_t dash_dir;        // Dash direction (-1 left, +1 right)
    uint8_t dash_timer;     // Frames remaining in dash
    uint8_t dash_cooldown;  // Frames until can dash again
    uint8_t last_lr_time;   // Frame counter for double-tap detection
    uint8_t last_lr_dir;    // Last L/R direction pressed (for double-tap)
} Player;

// --- Global Game State ---
extern GameState game_state;
extern Player player;
extern Entity enemies[MAX_ENEMIES];
extern Entity bullets[MAX_BULLETS];
extern Entity enemy_bullets[MAX_ENEMY_BULLETS];
extern uint16_t score;
extern uint8_t lives;
extern uint8_t level;
extern uint16_t frame_count;
extern uint8_t center_line_state[CENTER_LINE_TILES];

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
void player_jump(void);
void player_dash(int8_t dir);

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

// enemy_bullet.c
void init_enemy_bullets(void);
void update_enemy_bullets(void);
void render_enemy_bullets(void);
void fire_enemy_bullet(uint16_t x, uint16_t y, Direction dir);

// collision.c
void check_collisions(void);
uint8_t check_collision(Entity* a, Entity* b);

// utils.c
void draw_center_line(void);
void update_hud(void);
void reset_center_line(void);
void destroy_center_line_section(uint8_t x, uint8_t radius);
uint8_t get_center_line_tile_at(uint8_t x);

// starfield.c
void init_starfield(void);
void update_starfield(void);

#endif
