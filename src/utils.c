// Utility functions for Horizon Shift 89
// HUD display and center line drawing

#include <gb/gb.h>
#include <gbdk/console.h>
#include <stdio.h>
#include "game.h"

// Draw the center line using background tiles
void draw_center_line(void) {
    // The center line is at tile row 9 (72 pixels / 8 pixels per tile)
    // We'll use a simple line pattern

    // For now, we'll draw it with printf positioned at the center
    // In a full implementation, you'd use custom tiles

    gotoxy(0, 9);
    printf("--------------------");
}

// Update the HUD display (score, lives)
void update_hud(void) {
    // Score at top-left
    gotoxy(0, 0);
    printf("SC:%05u", score);

    // Lives at top-right
    gotoxy(14, 0);
    printf("L:%u", lives);

    // Level indicator
    gotoxy(10, 0);
    printf("Lv%u", level);
}
