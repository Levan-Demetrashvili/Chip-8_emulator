#ifndef HELPER_H
#define HELPER_H


#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL3/SDL.h>


#define FONTSET_START 0x50

typedef struct {
    uint8_t memory[4096];
    uint8_t registers[16];
    uint16_t I;
    uint16_t pc;
    uint16_t stack[16];
    uint16_t sp;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t screen[32][64];
    bool keypad[16];
    bool legacy_version;
    bool draw_flag;
} CHIP8;

typedef struct {
   SDL_Window *window;
   SDL_Renderer *renderer;
   bool success;
} GraphicsContext;

extern const uint8_t FONTSET[80];

GraphicsContext set_up_SDL(const char* title, int width, int height);
void chip8_init(CHIP8 *chip8);
void load_ROM(CHIP8 *chip8, const char *filename);
void chip8_print_screen_ascii(CHIP8 *chip8);
void tick_timers(CHIP8 *chip8);
int endsWith(const char *str, const char *suffix);

#endif 