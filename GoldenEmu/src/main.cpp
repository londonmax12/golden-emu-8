#include <iostream>

#include "chip8.h"

Chip8Core chipCpu;

void main() {
	// TODO:
	// Setup graphics
	// Setup input and callbacks


	// Clear memory, registers and screen
	chipCpu.Initialize();
	// Load program into memory
	chipCpu.LoadProgram("tetris.c8");

	while (chipCpu.IsRunning())
	{
		// Emulate 1 CPU cycle
		chipCpu.Cycle();

		// Not every cycle should render
		if (chipCpu.ShouldDraw()) {
			// TODO:
			// Draw graphics
		}

		chipCpu.SetKeys();
	}
}