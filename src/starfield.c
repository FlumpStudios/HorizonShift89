// starfield.c
// Twinkling starfield background for Horizon Shift 89

#include <gb/gb.h>
#include <rand.h>
#include "game.h"

// Star positions (x, y in tile coordinates)
static uint8_t star_x[NUM_STARS];
static uint8_t star_y[NUM_STARS];
static uint8_t star_visible[NUM_STARS];

// Background tile data for the star (loaded into background VRAM)
// Using light gray (color 1): low byte has pixels, high byte is 0
static const uint8_t star_tile_data[] = {
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x18, 0x00,  // Light gray pixels
    0x18, 0x00,  // Light gray pixels
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00
};

// Initialize the starfield with pseudo-random positions
void init_starfield(void) {
    uint8_t i;

    // Load star tile into background VRAM at tile index 1
    set_bkg_data(1, 1, star_tile_data);

    // Generate star positions avoiding the center line area (y=8-10)
    // and the HUD area (y=0-1)
    for (i = 0; i < NUM_STARS; i++) {
        // Use simple distribution across screen (20x18 visible tiles)
        star_x[i] = (uint8_t)((i * 7 + 3) % 20);

        // Distribute stars in upper and lower screen halves
        if (i < NUM_STARS / 2) {
            // Upper half: rows 2-7 (avoid HUD at 0-1 and center at 8-10)
            star_y[i] = (uint8_t)(2 + (i % 6));
        } else {
            // Lower half: rows 11-17 (avoid center at 8-10)
            star_y[i] = (uint8_t)(11 + ((i - NUM_STARS / 2) % 7));
        }

        // Start all stars visible
        star_visible[i] = 1;

        // Place the star tile
        set_bkg_tile_xy(star_x[i], star_y[i], 1);
    }

    // Enable background display
    SHOW_BKG;
}

// Update starfield - toggle random stars for twinkling effect
void update_starfield(void) {
    uint8_t star_index;

    // Only update every TWINKLE_RATE frames for subtle effect
    if ((frame_count % TWINKLE_RATE) != 0) {
        return;
    }

    // Toggle a random star
    star_index = (uint8_t)(rand() % NUM_STARS);

    if (star_visible[star_index]) {
        // Hide the star (set to empty tile 0)
        set_bkg_tile_xy(star_x[star_index], star_y[star_index], 0);
        star_visible[star_index] = 0;
    } else {
        // Show the star
        set_bkg_tile_xy(star_x[star_index], star_y[star_index], 1);
        star_visible[star_index] = 1;
    }
}
