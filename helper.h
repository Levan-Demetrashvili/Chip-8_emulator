#ifndef HELPER_H
#define HELPER_H


#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL3/SDL.h>


#define MEMORY_SIZE_BYTES 4096
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCALE 20
#define PC_START 0x200
#define FONTSET_START 0x50
#define FONTSET_LENGTH 80
typedef struct {
    uint8_t memory[MEMORY_SIZE_BYTES];
    uint8_t registers[16];
    uint16_t I;
    uint16_t pc;
    uint16_t stack[16];
    uint16_t sp;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t screen[SCREEN_HEIGHT][SCREEN_WIDTH];
    bool keypad[16];
    bool legacy_version;
    bool draw_flag;
} CHIP8;

typedef struct {
   SDL_Window *window;
   SDL_Renderer *renderer;
   SDL_Texture *texture;
   bool success;
} GraphicsContext;

extern const uint8_t FONTSET[];
extern const uint8_t KEYPAD_MAPPING[];

GraphicsContext set_up_SDL(const char* title, int width, int height);
void chip8_init(CHIP8 *chip8);
void load_ROM(CHIP8 *chip8, const char *filename);
void chip8_print_screen_ascii(CHIP8 *chip8);
void update_timers(CHIP8 *chip8);
void draw_graphics(CHIP8 *chip8, GraphicsContext *gfx);
void handle_input(CHIP8 *chip8, SDL_Event event);
int endsWith(const char *str, const char *suffix);

#endif 