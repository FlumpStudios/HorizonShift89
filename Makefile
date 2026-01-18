# Horizon Shift 89 - Game Boy Makefile
# Requires GBDK-2020: https://github.com/gbdk-2020/gbdk-2020/releases

# --- Configuration ---
# Set GBDK path (update this to your GBDK installation path)
ifndef GBDK
	GBDK = C:/Projects/GB/gbdk
endif

ROM_NAME = HorizonShift89

# --- Tools ---
LCC = $(GBDK)/bin/lcc
PNG2ASSET = $(GBDK)/bin/png2asset

# --- Directories ---
SRC_DIR = src
RES_DIR = res
INC_DIR = include
BUILD_DIR = build

# --- Compiler Flags ---
LCCFLAGS = -Wa-l -Wl-m -Wl-j -Wm-yoA -Wm-yt0x1B -Wm-yn"$(ROM_NAME)"
LCCFLAGS += -I$(INC_DIR) -I$(RES_DIR)

# --- Source Files ---
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/player.c $(SRC_DIR)/enemy.c $(SRC_DIR)/bullet.c $(SRC_DIR)/collision.c $(SRC_DIR)/utils.c $(wildcard $(RES_DIR)/*.c)
OBJS = $(SRCS:.c=.o)

# --- Targets ---
all: $(BUILD_DIR)/$(ROM_NAME).gb

$(BUILD_DIR)/$(ROM_NAME).gb: $(SRCS)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(LCC) $(LCCFLAGS) -o $@ $^

clean:
	@if exist $(BUILD_DIR)\*.gb del /Q $(BUILD_DIR)\*.gb
	@if exist $(BUILD_DIR)\*.map del /Q $(BUILD_DIR)\*.map
	@if exist $(BUILD_DIR)\*.sym del /Q $(BUILD_DIR)\*.sym
	@if exist $(SRC_DIR)\*.o del /Q $(SRC_DIR)\*.o
	@if exist $(RES_DIR)\*.o del /Q $(RES_DIR)\*.o

# Convert PNG sprites to C source (run manually as needed)
# Usage: make sprites
sprites:
	$(PNG2ASSET) $(RES_DIR)/sprites/player.png -o $(RES_DIR)/sprite_player.c -spr8x8 -c $(RES_DIR)/sprite_player.c
	$(PNG2ASSET) $(RES_DIR)/sprites/enemies.png -o $(RES_DIR)/sprite_enemies.c -spr8x8 -c $(RES_DIR)/sprite_enemies.c
	$(PNG2ASSET) $(RES_DIR)/sprites/bullets.png -o $(RES_DIR)/sprite_bullets.c -spr8x8 -c $(RES_DIR)/sprite_bullets.c

run: $(BUILD_DIR)/$(ROM_NAME).gb
	@echo Run $(BUILD_DIR)/$(ROM_NAME).gb in your favorite emulator (BGB, Emulicious, etc.)

.PHONY: all clean sprites run
