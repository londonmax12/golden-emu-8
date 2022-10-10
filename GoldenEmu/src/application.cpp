#include "application.h"
#include "util.h"
#include "ui.h"

#include <iostream>
#include <string>
#include <Windows.h>
#include <ShlObj.h>
#include <filesystem>
#include <fstream>

bool Application::Initialize()
{
    // Get app data folder
    {
        PWSTR roamingFilePath;
        HRESULT result = SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &roamingFilePath);
        if (result == S_OK)
        {
            char filepath[256];
            wcstombs(filepath, roamingFilePath, 128);
            std::string filepathStr = filepath;
            std::replace(filepathStr.begin(), filepathStr.end(), L'\\', L'/');
            filepathStr += "/GoldenEmu8";
            m_Settings.Filepath = filepathStr + "/Settings.yaml";

            if (!std::filesystem::exists(filepathStr))
                std::filesystem::create_directories(filepathStr);
        }
    }
    if (std::filesystem::exists(m_Settings.Filepath))
    {
        SettingsSerializer::Deserialize(m_Settings, m_Settings.Filepath);
    }

    // Initalize SDL
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    // Init audio device
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    // Load audio
    m_BeepSfx = Mix_LoadWAV("assets/audio/beep.wav");

    // Set volume from settings
    Mix_VolumeChunk(m_BeepSfx, m_Settings.Volume);

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


    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    Style();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(m_Window, m_Renderer);
    ImGui_ImplSDLRenderer_Init(m_Renderer);
    
    io.FontDefault = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 16.0f);

    return true;
}

void Application::Update()
{
    // Poll events
    while (SDL_PollEvent(&m_Event)) 
    {
        ImGui_ImplSDL2_ProcessEvent(&m_Event);
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
    if(!m_SettingsOpen)
        m_ChipCpu.Cycle();

    Input::Update();

    bool forcedRedraw = false;
    if (m_ForceRedraw)
    {
        m_ChipCpu.ForceRedraw(true);
    }

    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    
    // Get window size
    int w, h;
    SDL_GetWindowSize(m_Window, &w, &h);

    // ImGui

    int barSize = 0;
    if (ImGui::BeginMainMenuBar())
    {
        barSize = ImGui::GetWindowSize().y;
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open ROM"))
            {
                std::string input = Util::GetUserInputFile();
                if (!input.empty()) {
                    m_ChipCpu.Reset();
                    LoadChipProgram(input);
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Quit", "ALT + F4"))
                m_ShouldClose = true;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Settings"))
            {
                showSettingsPopup = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    h = h - barSize;

    if (showSettingsPopup)
    {
        ImGui::OpenPopup("Settings");
        showSettingsPopup = false;
    }

    m_SettingsOpen = false;
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2{ 700, 325 });
    if (ImGui::BeginPopupModal("Settings", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration))
    {
        m_SettingsOpen = true;
        if (ImGui::Button("General"))
        {
            m_SettingsSwitch = 0;
        }
        ImGui::SameLine();
        if (ImGui::Button("Advanced"))
        {
            m_SettingsSwitch = 1;
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 75);
        if (ImGui::Button("Close", ImVec2(70.0f, 0.0f)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::Separator();

        switch (m_SettingsSwitch)
        {
        case 0:
            if (ImGui::UI::SliderIntAnimated("Beep Volume", &m_Settings.Volume, 0, 128, "%d"))
                Mix_VolumeChunk(m_BeepSfx, m_Settings.Volume);
            break;
        case 1:
            ImGui::UI::SliderIntAnimated("Clock Speed (Hz)", &m_Settings.RefreshRateHz, 1, 2000, "%dHz");
            break;
        default:
            break;
        }

        ImGui::EndPopup();
    }

    ImGui::Render();

    SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);

    // Clear screen
    SDL_RenderClear(m_Renderer);

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
                SDL_Rect rect{ x * pixelSize, y * pixelSize + barSize, pixelSize, pixelSize };
                // Set rect draw colour
                SDL_SetRenderDrawColor(m_Renderer, 255, 255, 255, 255);
                // Render rect
                SDL_RenderFillRect(m_Renderer, &rect);
            }
        }
    }

    if (m_ChipCpu.ShouldPlaySound()) {
        m_ChipCpu.CPlaySound();
        Mix_PlayChannel(-1, m_BeepSfx, 0);
    }

    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    // Udate screen
    SDL_RenderPresent(m_Renderer);

    if (m_ForceRedraw) {
        m_ChipCpu.ForceRedraw(false);
        m_ForceRedraw = false;
    }
    m_ChipCpu.SetKeys(Input::GetKeys());
}

bool Application::LoadChipProgram(std::string path)
{
    m_ChipCpu.LoadProgram(path);
    return true;
}

void Application::Style()
{
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };
    colors[ImGuiCol_ChildBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };

    colors[ImGuiCol_SliderGrab] = ImVec4{ 0.8f, 0.8f, 0.0f, 1.0f };
    colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.9f, 0.9f, 0.0f, 1.0f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f }; 
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
}

Application::~Application()
{
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    // Clean up SDL
    SDL_DestroyRenderer(m_Renderer);
    SDL_DestroyWindow(m_Window);

    Mix_FreeChunk(m_BeepSfx);
    Mix_CloseAudio();

    SDL_Quit();

    if (!std::filesystem::exists(m_Settings.Filepath))
    {
        std::ofstream file{ m_Settings.Filepath, std::ofstream::out };
        file.close();
    }

    SettingsSerializer::Serialize(m_Settings, m_Settings.Filepath);
}