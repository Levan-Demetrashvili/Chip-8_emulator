#include "helper.h"

const uint8_t FONTSET[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

const int INSTRUCTION_SIZE = 2;

GraphicsContext set_up_SDL(const char* title, int width, int height) {
    GraphicsContext ctx = {
        .window = NULL,
        .renderer = NULL,
        .success = false, 
    };

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("Unable to initialize SDL_Video: %s", SDL_GetError());
        SDL_Quit();
    }
    
    if (!SDL_CreateWindowAndRenderer(title,width,height,SDL_WINDOW_MINIMIZED | SDL_WINDOW_RESIZABLE, &ctx.window,&ctx.renderer)) {
        SDL_Log("Unable to create SDL window and renderer: %s", SDL_GetError());
        SDL_Quit();
    }
    ctx.success = true;
    return ctx;
}

void chip8_init(CHIP8 *chip8) {
	memset(chip8, 0, sizeof(CHIP8));
	chip8->pc = 0x200;
	chip8->legacy_version = false;

	for (int i = 0; i < 80; i++) {
		chip8->memory[FONTSET_START + i] = FONTSET[i];
	}
} 

void load_ROM(CHIP8 *chip8, const char *filename) {
    FILE *src = fopen(filename, "rb");
	if (src == NULL) {
		printf("Cannot read the file\n");
        exit(2);
    }

	uint8_t instruction_buffer[INSTRUCTION_SIZE];

	// Load the ROM
	while (fread(instruction_buffer, sizeof(uint8_t), 2, src) != 0) {
		chip8->memory[chip8->pc] = instruction_buffer[0];
		chip8->memory[chip8->pc + 1] = instruction_buffer[1];
        // Check if program is bigger than memory can load
        if (chip8->pc >= 4095) {
            printf("Program is too large to load (> 3.5kb)");
            fclose(src);
            exit(4);
        }
		chip8->pc += 2;
	}
    fclose(src);

	chip8->pc = 0x200;
}
void chip8_print_screen_ascii(CHIP8 *chip8) {
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            putchar(chip8->screen[y][x] ? '#' : ' ');
        }
        printf("\n");
    }
}


void tick_timers(CHIP8 *chip8) {
    if (chip8->delay_timer > 0) chip8->delay_timer--;
    if (chip8->sound_timer > 0) chip8->sound_timer--;
}

int endsWith(const char *str, const char *suffix) {
	if (!str || !suffix)
		return 0;
	size_t lenstr = strlen(str);
	size_t lensuffix = strlen(suffix);
	if (lensuffix > lenstr)
		return 0;
	return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}
