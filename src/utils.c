// Utility functions for Horizon Shift 89
// HUD display and center line drawing

#include <gb/gb.h>
#include <gbdk/console.h>
#include <stdio.h>
#include "game.h"

// Center line state: 0 = intact, 1 = destroyed
uint8_t center_line_state[CENTER_LINE_TILES];

// Reset all center line tiles to intact (called at game start and level up)
void reset_center_line(void) {
    for (uint8_t i = 0; i < CENTER_LINE_TILES; i++) {
        center_line_state[i] = 0;
    }
}

// Destroy center line sections around impact point
void destroy_center_line_section(uint8_t x, uint8_t radius) {
    // Convert screen x position to tile index (screen is 160px, 20 tiles of 8px each)
    uint8_t tile_index = x / 8;

    // Destroy tiles within radius
    for (int8_t offset = -(int8_t)radius; offset <= (int8_t)radius; offset++) {
        int8_t target = (int8_t)tile_index + offset;
        if (target >= 0 && target < CENTER_LINE_TILES) {
            center_line_state[target] = 1;
        }
    }

    // Redraw the center line to show destruction
    draw_center_line();
}

// Check if a tile at screen position x is destroyed
uint8_t get_center_line_tile_at(uint8_t x) {
    uint8_t tile_index = x / 8;
    if (tile_index >= CENTER_LINE_TILES) {
        tile_index = CENTER_LINE_TILES - 1;
    }
    return center_line_state[tile_index];
}

// Draw the center line using background tiles
void draw_center_line(void) {
    // The center line is at tile row 9 (72 pixels / 8 pixels per tile)
    gotoxy(0, 9);
    for (uint8_t i = 0; i < CENTER_LINE_TILES; i++) {
        if (center_line_state[i] == 0) {
            printf("-");
        } else {
            printf(" ");
        }
    }
}

// Update the HUD display (score, lives)
void update_hud(void) {
    // Score at top-left
    gotoxy(0, 0);
    printf("%05u", score);

    // Level indicator
    gotoxy(6, 0);
    printf("L%u", level);

    // Lives at top-right (show hearts/count clearly)
    gotoxy(16, 0);
    printf("x%u", lives);
}
