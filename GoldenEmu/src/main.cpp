#include <iostream>
#include <Windows.h>

#include "chip8.h"

Chip8Core chipCpu;

void debugGraphics() {
	DWORD dw;
	COORD pos;
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE)
	{
		printf("Invalid handle");
	}
	for (int x = 0; x < 64; x++) {
		for (int y = 0; y < 32; y++) {
			if (chipCpu.IsPixelActive(x, y)) {
				pos.X = x;
				pos.Y = y;
				WriteConsoleOutputCharacterA(hStdOut, "X", 7, pos, &dw);
			}
			else {
				pos.X = x;
				pos.Y = y;
				WriteConsoleOutputCharacterA(hStdOut, " ", 7, pos, &dw);
			}
		}
	}
}

void main() {
	// TODO:
	// Setup graphics
	// Setup input and callbacks


	// Clear memory, registers and screen
	chipCpu.Initialize();
	// Load program into memory
	chipCpu.LoadProgram("test_opcode.ch8");

	while (chipCpu.IsRunning())
	{
		// Emulate 1 CPU cycle
		chipCpu.Cycle();

		// Not every cycle should render
		if (chipCpu.ShouldDraw()) {
			debugGraphics();
			// TODO:
			// Draw graphics
		}

		chipCpu.SetKeys();
	}
}