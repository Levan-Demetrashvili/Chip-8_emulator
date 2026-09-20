#include "helper.h"


void emulate_cycle(CHIP8 *chip8, uint16_t opcode);

int main(int argc, char *argv[]) {

	if (argc != 2 || !endsWith(argv[1], ".ch8")) {
		printf("Usage: ./emulator.exe *.ch8\n");
		return 1;
	}
   

    GraphicsContext gfx = set_up_SDL("CHIP8 Emulator", 640, 320);
    if (!gfx.success) {
        return 5;
    }

	CHIP8 chip8;
	chip8_init(&chip8);
    load_ROM(&chip8, argv[1]);
	
    SDL_Event event;
    bool running = true;
	while (running) {
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT) running = false;
        }

		if (chip8.pc > 0xFFF) {
			printf("Out of bound error");
			return 3;
		}

		uint16_t opcode =
			(chip8.memory[chip8.pc] << 8) | chip8.memory[chip8.pc + 1];
		chip8.pc += 2;
        
		emulate_cycle(&chip8, opcode);
        

   
	}
    SDL_DestroyRenderer(gfx.renderer);
    SDL_DestroyWindow(gfx.window);
    SDL_Quit();

    return 0;
}

void emulate_cycle(CHIP8 *chip8, uint16_t opcode) {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	uint8_t kk = opcode & 0x00FF;
	uint8_t n = opcode & 0x000F;
	uint16_t nnn = opcode & 0x0FFF;

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode) {
                case 0x00E0:
                    printf("0x0__0\n");
                    memset(chip8->screen, 0, sizeof(chip8->screen));
                    chip8->draw_flag = true;
                    break;
                case 0x00EE:
                    printf("0x0__E\n");
                        chip8->sp -= 1;
                        chip8->pc = chip8->stack[chip8->sp];
                    break;

                default:
                    printf("UNHANDLED OPCODE: 0x%04X at PC: 0x%03X\n", opcode, chip8->pc - 2);
                    break;
            }
            break;
        
        case 0x1000:
            printf("0x1 %i %i\n", chip8->pc, nnn);
            chip8->pc = nnn;
            break;
        case 0x2000:
            printf("0x2 %i %i\n", chip8->pc, nnn);
            chip8->stack[chip8->sp] = chip8->pc; 
            chip8->sp += 1;
            chip8->pc = nnn;
            break;
            
        case 0x3000:
            printf("0x3\n");
            if (chip8->registers[x] == kk) chip8->pc += 2;
            break;
        case 0x4000:
            printf("0x4\n");
            if (chip8->registers[x] != kk) chip8->pc += 2;
            break;
        case 0x5000:
            printf("0x5\n");
            if (chip8->registers[x] == chip8->registers[y]) chip8->pc += 2;
            break;

        case 0x6000:
            printf("0x6: \n");
            chip8->registers[x] = kk;
            break;
        
        case 0x7000:
            printf("0x7: \n");
            chip8->registers[x] += kk;
            break;
        case 0x8000:

            switch (opcode & 0xF00F) {
                case 0x8000:
                    printf("0x8__0\n");
                    chip8->registers[x] = chip8->registers[y];
                    break;

                case 0x8001:
                    printf("0x8__1\n");
                    if (chip8->legacy_version) chip8->registers[0xF] = 0;
                    chip8->registers[x] |= chip8->registers[y];
                    break;

                case 0x8002:
                    printf("0x8__2\n");
                    if (chip8->legacy_version) chip8->registers[0xF] = 0;
                    chip8->registers[x] &= chip8->registers[y];
                    break;

                case 0x8003:
                    printf("0x8__3\n");
                    if (chip8->legacy_version) chip8->registers[0xF] = 0;
                    chip8->registers[x] ^= chip8->registers[y];
                    break;

                case 0x8004:
                    printf("0x8__4\n");
                    int sum = chip8->registers[x] + chip8->registers[y];
                    chip8->registers[x] = sum;
                    chip8->registers[0xF] =  sum > 255 ? 1 : 0;
                
                    break;

                case 0x8005:
                    printf("0x8__5\n");
                    chip8->registers[0xF] = chip8->registers[x] >= chip8->registers[y] ? 1 : 0;
                    chip8->registers[x] -= chip8->registers[y];
                    break;

                case 0x8006:
                    printf("0x8__6\n");
                    if (chip8->legacy_version) {
                        // Old Systems
                        chip8->registers[0xF] = chip8->registers[y] & 0x0001;
                        chip8->registers[x] = chip8->registers[y] >> 1;

                    } else {
                        // Newer Systems
                        chip8->registers[0xF] = chip8->registers[x] & 0x0001;
                        chip8->registers[x] >>= 1;
                    }
                    break;

                case 0x8007:
                    printf("0x8__7\n");
                    chip8->registers[0xF] = chip8->registers[y] >= chip8->registers[x] ? 1 : 0;
                    chip8->registers[x] = chip8->registers[y] - chip8->registers[x];
                    break;

                case 0x800E:
                    printf("0x8__E\n");
                    if (chip8->legacy_version) {
                        // Old Systems
                        chip8->registers[0xF] = (chip8->registers[y] & 0x80) >> 7;
                        chip8->registers[x] = chip8->registers[y] << 1;

                    } else {
                        // Newer Systems
                        chip8->registers[0xF] = (chip8->registers[x] & 0x80) >> 7;
                        chip8->registers[x] <<=  1;
                    }
                    break;

                default:
                    printf("UNHANDLED OPCODE: 0x%04X at PC: 0x%03X\n", opcode, chip8->pc - 2);
                    break;
            }
            break;
        case 0x9000:
            printf("0x9\n");
            if (chip8->registers[x] != chip8->registers[y]) chip8->pc += 2;
            break;
        case 0xA000:
            printf("0xA\n");
            chip8->I = nnn;
            break;

        case 0xB000:
            printf("0xB\n");
            chip8->pc = nnn + chip8->registers[0];
            break;
        case 0xC000:
            printf("0xC\n");
            chip8->registers[x] = (rand() % 256) & kk;
            break;
        
        case 0xD000: {
            printf("0xD\n");
            chip8->registers[0xF] = 0; 
            uint8_t reg_y = chip8->registers[y] % 32, reg_x = chip8->registers[x] % 64;
            chip8->draw_flag = true;

            // Draw byte
            for (int i = 0; i < n; i++) {
                uint8_t sprite_byte = chip8->memory[chip8->I + i];
                uint8_t screen_y = reg_y + i;
                if (screen_y >= 32 ) break;
                
                // Draw pixel of byte
                for (int j = 0; j < 8; j++) {
                    uint8_t pixel = (sprite_byte >> (7 - j)) & 0x1;
                    uint8_t screen_x = reg_x + j;
                    if (screen_x >= 64 ) break;

                    if (pixel == 1 && chip8->screen[screen_y][screen_x] == 1) {
                        chip8->registers[0xF] = 1;
                    }

                    chip8->screen[screen_y][screen_x] ^=  pixel;
                }
            }
            break;
        }

        case 0xE000: {
            uint8_t reg_x = chip8->registers[x];
            switch (opcode & 0xF0FF) {
                case 0xE09E:
                    printf("0x0xE_9E\n");  
                    if (reg_x <= 15 && chip8->keypad[reg_x]) chip8->pc += 2;
                    break;

                case 0xE0A1:
                    printf("0x0xE_A1\n");
                    chip8->keypad[chip8->registers[x]] = 1;
                    if (reg_x <= 15 && !chip8->keypad[reg_x]) chip8->pc += 2;
                    break;

                default:
                    break;
            }
            break;
        }

        case 0xF000:
            switch (opcode & 0xF0FF) {
                case 0xF007:
                    printf("0xF_07\n");
                    chip8->registers[x] = chip8->delay_timer;
                    break;

                  case 0xF00A:
                    printf("0xF_0A\n");
                    bool key_pressed = false;
                    for (int i = 0; i < 16; i++) {
                        if (chip8->keypad[i]) {
                            chip8->registers[x] = i;
                            key_pressed = true;
                            break;
                        }
                    }
                    if (!key_pressed) chip8->pc -= 2;
                    break;

                case 0xF015:
                    printf("0xF_15\n");
                    chip8->delay_timer = chip8->registers[x];
                    break;
                
                case 0xF018:
                    printf("0xF_18\n");
                    chip8->sound_timer = chip8->registers[x];
                    break;
                
                case 0xF01E:
                    printf("0xF_1E\n");
                    chip8->I += chip8->registers[x];
                    break;

                case 0xF029:
                    printf("0xF_29\n");
                        chip8->I = FONTSET_START + (chip8->registers[x] & 0x0F) * 5;
                    break;
                
                case 0xF033:
                    printf("0xF_33\n");
                    chip8->memory[chip8->I] = chip8->registers[x] / 100;
                    chip8->memory[chip8->I + 1] = (chip8->registers[x] / 10) % 10;
                    chip8->memory[chip8->I + 2] = chip8->registers[x] % 10;
                    break;
                
                case 0xF055:
                    printf("0xF_55\n");
                    for (int i = 0; i <= x; i++) {
                        chip8->memory[chip8->I + i] = chip8->registers[i];
                    }
                    if (chip8->legacy_version) chip8->I = chip8->I + x + 1;
                    break;
                
                case 0xF065:
                    printf("0xF_65\n");
                    for (int i = 0; i <= x; i++) {
                        chip8->registers[i] = chip8->memory[chip8->I + i];
                    }
                    if (chip8->legacy_version) chip8->I = chip8->I + x + 1;
                    break;
                
                default:
                    printf("UNHANDLED OPCODE: 0x%04X at PC: 0x%03X\n", opcode, chip8->pc - 2);
                    break;
            }
            break;
        
        default:
            printf("UNHANDLED OPCODE: 0x%04X at PC: 0x%03X\n", opcode, chip8->pc - 2);
            break;
        }
}



