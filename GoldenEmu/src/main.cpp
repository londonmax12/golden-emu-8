#define SDL_MAIN_HANDLED

#include <iostream>
#include <Windows.h>

#include "application.h"

Application app;

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
			if (app.GetEmulatedCPU()->IsPixelActive(x, y)) {
				pos.X = x;
				pos.Y = y;
				WriteConsoleOutputCharacter(hStdOut, L"█", 1, pos, &dw);
			}
			else {
				pos.X = x;
				pos.Y = y;
				WriteConsoleOutputCharacterA(hStdOut, "-", 1, pos, &dw);
			}
		}
	}
}

void main() {
	app.Initialize();

	app.GetEmulatedCPU()->LoadProgram("test_opcode.ch8");

	while (!app.WantClose())
	{
		app.Update();
	}

}