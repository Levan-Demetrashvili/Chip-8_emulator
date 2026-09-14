#include <cs50.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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
	bool shift_quirk;
} CHIP8;

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

void emulate_cycle(CHIP8 *chip8, uint16_t opcode);
void chip8_init(CHIP8 *chip8);
int endsWith(const char *str, const char *suffix);

int main(int argc, char *argv[]) {

	if (argc != 2 || !endsWith(argv[1], ".ch8")) {
		printf("Usage: ./emulator.exe *.ch8\n");
		return 1;
	}

	CHIP8 chip8;
	chip8_init(&chip8);

	FILE *src = fopen(argv[1], "rb");
	if (src == NULL) {
		printf("Cannot read the file\n");
		return 2;
	}

	uint8_t instruction_buffer[2];

	// Load the ROM
	while (fread(instruction_buffer, sizeof(uint8_t), 2, src) != 0) {
		chip8.memory[chip8.pc] = instruction_buffer[0];
		chip8.memory[chip8.pc + 1] = instruction_buffer[1];
		chip8.pc += 2;
	}
	chip8.pc = 0x200;

	while (true) {
		if (chip8.pc > 1000) {
			printf("Out of bound error");
			return 3;
		}
		uint16_t opcode =
			(chip8.memory[chip8.pc] << 8) | chip8.memory[chip8.pc + 1];
		chip8.pc += 2;

		emulate_cycle(&chip8, opcode);
	}
}

void emulate_cycle(CHIP8 *chip8, uint16_t opcode) {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	uint8_t kk = opcode & 0x00FF;
	uint8_t n = opcode & 0x000F;
	uint16_t nnn = opcode & 0x0FFF;

	switch (opcode & 0xF000) {
	case 0x0000:
		switch (opcode & 0xF00F) {
		case 0x00E0:
			printf("0x0__0\n");
			memset(chip8->screen, 0, sizeof(chip8->screen));
			break;
		case 0x00EE:
			printf("0x0__E\n");
			chip8->sp -= 1;
			chip8->pc = chip8->stack[chip8->sp];
			break;

		default:
			break;
		}
    
    case 0x1000:
        printf("0x1\n");
        chip8->pc = nnn;
        break;
    case 0x2000:
        printf("0x2\n");
        chip8->stack[chip8->sp] = chip8->pc; 
        chip8->sp += 1;
        chip8->pc = nnn;
        break;
        
    case 0x3000:
        printf("0x3\n");
        if (chip8->registers[x] == kk) {
            chip8->pc += 2;
        }
        break;
    case 0x4000:
        printf("0x4\n");
        if (chip8->registers[x] != kk) {
            chip8->pc += 2;
        }
        break;
	case 0x5000:
		printf("0x5\n");
		if (chip8->registers[x] == chip8->registers[y]) {
			chip8->pc += 2;
		}
		break;

	case 0x6000:
		printf("0x6: \n");
		uint8_t value = opcode & 0x00FF;
		uint8_t register_addr = (opcode & 0x0F00) >> 8;
		chip8->registers[register_addr] = value;
		break;
    
    case 0x7000:
        printf("0x7: \n");
        chip8->registers[x] += kk;
        break;
	case 0x8000:

		switch (opcode & 0xF00F) {

		case 0x8006:
			printf("0x8__6\n");
			if (chip8->shift_quirk) {
				// Old Systems
				chip8->registers[0xf] = chip8->registers[y] & 0x0001;
				chip8->registers[x] = chip8->registers[y] >> 1;

			} else {
				// Newer Systems
				chip8->registers[0xf] = chip8->registers[x] & 0x0001;
				chip8->registers[x] = chip8->registers[x] >> 1;
			}
			break;

		default:
			// printf("Undefined opcode\n");
			break;
		}
    case 0xF000:
        switch (opcode & 0xF0FF) {
            case 0xf007:
                printf("0xf007\n");
                chip8->registers[x] = 0; 
                break;
            default:
	        	// printf("Undefined opcode\n");
                break;
        }
	default:
		// printf("Undefined opcode\n");
		break;
	}
}

void chip8_init(CHIP8 *chip8) {
	memset(chip8, 0, sizeof(CHIP8));
	chip8->pc = 0x200;
	chip8->shift_quirk = false;

	for (int i = 0; i < 80; i++) {
		chip8->memory[0x50 + i] = FONTSET[i];
	}
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
