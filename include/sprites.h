#ifndef SPRITES_H
#define SPRITES_H

#include <gb/gb.h>

// Placeholder sprite data (8x8 pixels, 2bpp = 16 bytes per tile)
// Replace these with actual sprite graphics later using GBTD or png2asset

// Tile 0: Player facing UP (arrow/ship pointing up)
// Tile 1: Player facing DOWN (arrow/ship pointing down)
// Tile 2: Enemy
// Tile 3: Bullet

const uint8_t sprite_data[] = {
    // Tile 0: Player facing UP - simple arrow shape
    0x00,0x00,
    0x24,0x24,
    0x3C,0x24,
    0x3C,0x24,
    0x3C,0x7E,
    0x3C,0xFF,
    0x3C,0xE7,
    0x00,0x00, 


    

    // Tile 1: Player facing DOWN - arrow pointing down
    0x00,0x00,
    0x3C,0xE7,
    0x3C,0xFF,
    0x3C,0x7E,
    0x3C,0x24,
    0x3C,0x24,
    0x24,0x24,
    0x00,0x00,

    // Tile 2: Enemy - simple invader shape
    0x24, 0x24,  // .X..X...
    0x18, 0x18,  // ..XX....
    0x7E, 0x7E,  // XXXXXX..
    0xDB, 0xDB,  // XX.XX.XX
    0xFF, 0xFF,  // XXXXXXXX
    0xBD, 0xBD,  // X.XXXX.X
    0x24, 0x24,  // .X..X...
    0x42, 0x42,  // X....X..

    // Tile 3: Bullet - small dot/line
    0x00, 0x00,  // ........
    0x18, 0x18,  // ..XX....
    0x18, 0x18,  // ..XX....
    0x3C, 0x3C,  // .XXXX...
    0x3C, 0x3C,  // .XXXX...
    0x18, 0x18,  // ..XX....
    0x18, 0x18,  // ..XX....
    0x00, 0x00   // ........
};

#endif
