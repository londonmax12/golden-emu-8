#include "application.h"
#include <iostream>

bool Application::Initialize()
{
    // Initalize SDL
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    // Init audio device
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    // Load audio
    m_BeepSfx = Mix_LoadWAV("assets/audio/beep.wav");

    // Create window with parameters
    m_Window = SDL_CreateWindow(
        "Golden Emu 8",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        1200,                               // width, in pixels
        600,                               // height, in pixels
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE                // flags - see below
    );

    // Check that the window was successfully created
    if (m_Window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        m_ShouldClose = true;
        return false;
    }

    // Create SDL renderer
    m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED);

    // Check is renderer was created successfully
    if (m_Renderer == NULL) {
        printf("Could not create renderer: %s\n", SDL_GetError());
        m_ShouldClose = true;
        return false;
    }

    m_ChipCpu.Initialize();

    return true;
}

void Application::Update()
{
    // Poll events
    while (SDL_PollEvent(&m_Event)) 
    { 
        switch (m_Event.type)
        {
        // Window close event
        case SDL_QUIT:
            m_ShouldClose = true;
            break;
        // Other window event
        case SDL_WINDOWEVENT:
            // On window resize
            if (m_Event.window.event == SDL_WINDOWEVENT_RESIZED) {
                // Force redraw
                m_ForceRedraw = true;
            }
            break;

        default:
            break;
        } 
    } 
    // Emulate 1 CPU cycle
    m_ChipCpu.Cycle();

    Input::Update();

    bool forcedRedraw = false;
    if (m_ForceRedraw)
    {
        m_ChipCpu.ForceRedraw(true);
    }

    // Not every cycle should render
    if (m_ChipCpu.ShouldDraw()) {
        SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);

        // Clear screen
        SDL_RenderClear(m_Renderer);

        // Get window size
        int w, h;
        SDL_GetWindowSize(m_Window, &w, &h);

        // Calculate the max pixel size to fit all content
        int pixelSize = 0;
        pixelSize = std::round(w / 64);
        if (pixelSize * 32 > h)
            pixelSize = std::round(h / 32);
        // Render all active pixels
        for (int x = 0; x < 64; x++) {
            for (int y = 0; y < 32; y++) {
                if (m_ChipCpu.IsPixelActive(x, y)) {
                    // Initialize new rect
                    SDL_Rect rect{ x * pixelSize, y * pixelSize, pixelSize, pixelSize };
                    // Set rect draw colour
                    SDL_SetRenderDrawColor(m_Renderer, 255, 255, 255, 255);
                    // Render rect
                    SDL_RenderFillRect(m_Renderer, &rect);
                }
            }
        }

        if (m_ChipCpu.ShouldPlaySound()) {
            m_ChipCpu.PlaySound();
            Mix_PlayChannel(-1, m_BeepSfx, 0);
        }

        // Udate screen
        SDL_RenderPresent(m_Renderer);
    }

    if (m_ForceRedraw) {
        m_ChipCpu.ForceRedraw(false);
        m_ForceRedraw = false;
    }
    m_ChipCpu.SetKeys(Input::GetKeys());
}

bool Application::LoadChipProgram(const char* path)
{
    char buffer[256];
    sprintf(buffer, "Golden Emu 8 (%s)", path);
    SDL_SetWindowTitle(m_Window, buffer);
    m_ChipCpu.LoadProgram(path);
    return true;
}

Application::~Application()
{
    // Clean up SDL
    SDL_DestroyRenderer(m_Renderer);
    SDL_DestroyWindow(m_Window);

    Mix_FreeChunk(m_BeepSfx);
    Mix_CloseAudio();

    SDL_Quit();
}
