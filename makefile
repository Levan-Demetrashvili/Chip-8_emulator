chip8:
	gcc *.c -o ./chip8_emulator.exe -lSDL3
debug:
	gcc -g3 -O0 -Wall -Wextra *.c -o ./chip8_emulator.exe