# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

HorizonShift89 is a Game Boy (DMG) homebrew vertical shooter written in C using GBDK-2020. Players defend a center line from enemies attacking from above and below.

## Build Commands

**Prerequisites**: GBDK-2020 installed at `C:/Projects/GB/gbdk` (configurable in Makefile/build.bat)

```bash
# Windows (recommended)
build.bat              # Build ROM
build-and-run.bat      # Build and run in emulator
clean.bat              # Remove build artifacts

# Make (Unix/WSL)
make                   # Build ROM to build/HorizonShift89.gb
make clean             # Remove build artifacts
make sprites           # Convert PNG to C source (if using png2asset)
```

**Output**: `build/HorizonShift89.gb`

**Running**: Open the ROM in any Game Boy emulator (BGB, Emulicious, etc.)

## Architecture

### Module Responsibilities

| File | Purpose |
|------|---------|
| `main.c` | Game loop, state machine (title/playing/paused/gameover), initialization |
| `player.c` | Player movement, jumping, dashing (double-tap), aiming, shooting |
| `enemy.c` | Enemy spawning and AI for 4 enemy types |
| `bullet.c` | Player and enemy projectile physics |
| `collision.c` | Collision detection, scoring |
| `utils.c` | HUD rendering, center line management/destruction |
| `game.h` | All global state, constants, type definitions, function prototypes |
| `sprites.h` | Sprite tile data (8x8 pixel definitions in 2bpp format) |

### Data Flow

```
main loop (60fps) → update_player() → update_enemies() → update_bullets()
                  → check_collisions() → render_*() functions
```

### Key Concepts

**Fixed-Point Arithmetic**: Entity positions use 8.8 fixed-point (multiply by 256 to store, shift right 8 to render) for smooth sub-pixel movement.

**Sprite Allocation**: Hardware sprites 0-39 are assigned as:
- 0: Player
- 1-4: Player bullets
- 5-12: Enemies (MAX_ENEMIES=8)
- 13-16: Enemy bullets

**Center Line**: Stored as `center_line_state[20]` array representing 20 tiles. Asteroids destroy sections; player falls through holes.

**Enemy Types**: NORMAL (vertical), SHOOTER (horizontal + fires), ZIGZAG (diagonal oscillation), ASTEROID (destroys line)

**Game States**: STATE_TITLE → STATE_PLAYING ↔ STATE_PAUSED → STATE_GAMEOVER

### Level Progression

- Level increases every 500 points (max level 9)
- Enemy spawn rate increases with level
- Center line resets on level up

## Key Constants (game.h)

- Screen: 160x144 pixels
- Center line Y: 72 (middle)
- Player speed: 2px/frame, Bullet speed: 4px/frame
- Jump velocity: 6, Gravity: 1, Max jump height: 32px
- Dash: 8px/frame for 8 frames, 20 frame cooldown
