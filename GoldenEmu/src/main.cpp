#define SDL_MAIN_HANDLED

#include <iostream>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <string>

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

	using clock = std::chrono::steady_clock;

	auto nextFrame = clock::now();

	while (!app.WantClose())
	{
		nextFrame += std::chrono::milliseconds(1000 / app.m_Settings.RefreshRateHz);
		app.Update();
		std::this_thread::sleep_until(nextFrame);
	}

}