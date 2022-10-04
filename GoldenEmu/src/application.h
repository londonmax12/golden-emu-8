#pragma once

#include "chip8.h"
#include "input.h"

#include "SDL.h"
#include "SDL_mixer.h"

#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_sdlrenderer.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif


class Application {
public:
	bool Initialize();

	// New frame
	void Update();

	// Incase of window close or unrecoverable error
	bool WantClose() { return m_ShouldClose; }

	bool LoadChipProgram(std::string path);

	Chip8Core* GetEmulatedCPU() { return &m_ChipCpu; }

	void Style();

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

	// Beep sound
	Mix_Chunk* m_BeepSfx;

	Chip8Core m_ChipCpu;
};