#ifndef SPRITES_H
#define SPRITES_H

#include <gb/gb.h>

// Placeholder sprite data (8x8 pixels, 2bpp = 16 bytes per tile)
// Replace these with actual sprite graphics later using GBTD or png2asset

// Tile 0: Player facing UP (arrow/ship pointing up)
// Tile 1: Player facing DOWN (arrow/ship pointing down)
// Tile 2: Enemy (vertical mover)
// Tile 3: Bullet (player)
// Tile 4: Shooter enemy (horizontal mover)
// Tile 5: Enemy bullet
// Tile 6: Zigzag enemy (wavy mover)
// Tile 7: Asteroid (rocky shape)
// Tile 8: Star (background tile for starfield)

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
    0x00, 0x00,  // ........

    // Tile 4: Shooter enemy - horizontal ship with wings
    0x18, 0x18,  // ...XX...
    0x18, 0x18,  // ...XX...
    0xDB, 0xDB,  // XX.XX.XX
    0xFF, 0xFF,  // XXXXXXXX
    0xFF, 0xFF,  // XXXXXXXX
    0xDB, 0xDB,  // XX.XX.XX
    0x18, 0x18,  // ...XX...
    0x18, 0x18,  // ...XX...

    // Tile 5: Enemy bullet - diamond shape
    0x00, 0x00,  // ........
    0x18, 0x18,  // ...XX...
    0x3C, 0x3C,  // ..XXXX..
    0x7E, 0x7E,  // .XXXXXX.
    0x7E, 0x7E,  // .XXXXXX.
    0x3C, 0x3C,  // ..XXXX..
    0x18, 0x18,  // ...XX...
    0x00, 0x00,  // ........

    // Tile 6: Zigzag enemy - serpent/wave shape
    0x60, 0x60,  // .XX.....
    0x90, 0x90,  // X..X....
    0x3C, 0x3C,  // ..XXXX..
    0x5A, 0x5A,  // .X.XX.X.
    0x5A, 0x5A,  // .X.XX.X.
    0x3C, 0x3C,  // ..XXXX..
    0x09, 0x09,  // ....X..X
    0x06, 0x06,  // .....XX.

    // Tile 7: Asteroid - rocky/chunky shape
    0x1C, 0x1C,  // ...XXX..
    0x3E, 0x22,  // ..XXXXX.
    0x7F, 0x63,  // .XXXXXXX
    0xFF, 0xC3,  // XXXXXXXX
    0xFE, 0xC6,  // XXXXXXX.
    0x7C, 0x44,  // .XXXXX..
    0x38, 0x28,  // ..XXX...
    0x10, 0x10,  // ...X....

    // Tile 8: Star - small twinkling star for background
    0x00, 0x00,  // ........
    0x00, 0x00,  // ........
    0x00, 0x00,  // ........
    0x18, 0x18,  // ...XX...
    0x18, 0x18,  // ...XX...
    0x00, 0x00,  // ........
    0x00, 0x00,  // ........
    0x00, 0x00,  // ........

    // Tile 9: Diver enemy - arrow/dart shape pointing down
    0x18, 0x18,  // ...XX...
    0x3C, 0x3C,  // ..XXXX..
    0x7E, 0x7E,  // .XXXXXX.
    0xFF, 0xFF,  // XXXXXXXX
    0x7E, 0x5A,  // .XXXXXX.
    0x3C, 0x24,  // ..XXXX..
    0x18, 0x18,  // ...XX...
    0x18, 0x18   // ...XX...
};

#endif
