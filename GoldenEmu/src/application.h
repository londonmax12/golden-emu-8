#pragma once

#include "chip8.h"

#include "SDL.h"

class Application {
public:
	bool Initialize();

	// New frame
	void Update();

	// Incase of window close or unrecoverable error
	bool WantClose() { return m_ShouldClose; }

	Chip8Core* GetEmulatedCPU() { return &m_ChipCpu; }

	~Application();
private:
	bool m_ShouldClose = false;
	bool m_ForceRedraw = false;

	// Main window
	SDL_Window* m_Window;

	// SDL renderer
	SDL_Renderer* m_Renderer;
	
	// SDL event
	SDL_Event m_Event;

	Chip8Core m_ChipCpu;
};