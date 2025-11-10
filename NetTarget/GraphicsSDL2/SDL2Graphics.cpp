#include "SDL2Graphics.h"

#ifdef _HAVE_SDL2
#include <cstring>
#include <iostream>
#include <fstream>
#include <Windows.h>

SDL2GraphicsBackend::SDL2GraphicsBackend()
    : m_window(nullptr)
    , m_renderer(nullptr)
    , m_texture(nullptr)
    , m_paletteRGB(nullptr)
    , m_sdlPalette(nullptr)
    , m_width(0)
    , m_height(0)
    , m_initialized(false)
{
}

SDL2GraphicsBackend::~SDL2GraphicsBackend()
{
    Shutdown();
}

bool SDL2GraphicsBackend::Initialize(void* windowHandle, int width, int height)
{
    std::ofstream log("C:\\originalhr\\HoverRace\\Release\\sdl2_debug.log", std::ios::app);
    log << "=== SDL2Graphics Software Rendering Initialize called ===" << std::endl;
    log << "Width: " << width << ", Height: " << height << std::endl;
    log.flush();

    if (m_initialized) { log << "Already initialized" << std::endl; log.close(); return false; }
    m_width = width; m_height = height;
    HWND hwnd = static_cast<HWND>(windowHandle);
    if (!hwnd) { log << "ERROR: Invalid window handle" << std::endl; log.close(); return false; }
    log << "Window handle valid" << std::endl; log.flush();
    
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
    { log << "ERROR: SDL_InitSubSystem failed: " << SDL_GetError() << std::endl; log.close(); return false; }
    log << "SDL_InitSubSystem OK" << std::endl; log.flush();
    
    m_window = SDL_CreateWindowFrom(hwnd);
    if (!m_window) { log << "ERROR: SDL_CreateWindowFrom failed: " << SDL_GetError() << std::endl; log.close(); SDL_QuitSubSystem(SDL_INIT_VIDEO); return false; }
    log << "SDL_CreateWindowFrom OK" << std::endl; log.flush();
    
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_SOFTWARE);
    if (!m_renderer) { log << "ERROR: SDL_CreateRenderer failed: " << SDL_GetError() << std::endl; log.close(); SDL_DestroyWindow(m_window); SDL_QuitSubSystem(SDL_INIT_VIDEO); return false; }
    log << "SDL_CreateRenderer OK (software)" << std::endl; log.flush();
    
    SDL_RenderSetLogicalSize(m_renderer, width, height);
    m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);
    if (!m_texture) { 
        log << "ERROR: SDL_CreateTexture failed: " << SDL_GetError() << std::endl; 
        log.close(); 
        SDL_DestroyRenderer(m_renderer); 
        SDL_DestroyWindow(m_window); 
        SDL_QuitSubSystem(SDL_INIT_VIDEO); 
        return false; 
    }
    log << "SDL_CreateTexture OK" << std::endl; log.flush();
    
    if (!m_paletteRGB) { m_paletteRGB = new uint8_t[768]; for (int i = 0; i < 256; i++) { m_paletteRGB[i*3+0] = m_paletteRGB[i*3+1] = m_paletteRGB[i*3+2] = i; } }
    log << "Palette allocated" << std::endl; log.flush();
    
    if (!CreateSDLPalette()) { log << "ERROR: CreateSDLPalette failed" << std::endl; log.close(); Shutdown(); return false; }
    m_initialized = true;
    log << "Initialization successful" << std::endl; log.close();
    return true;
}

void SDL2GraphicsBackend::Shutdown()
{
    if (m_texture) { SDL_DestroyTexture(m_texture); m_texture = nullptr; }
    if (m_renderer) { SDL_DestroyRenderer(m_renderer); m_renderer = nullptr; }
    if (m_window) { SDL_DestroyWindow(m_window); m_window = nullptr; }
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    if (m_paletteRGB) { delete[] m_paletteRGB; m_paletteRGB = nullptr; }
    if (m_sdlPalette) { delete[] m_sdlPalette; m_sdlPalette = nullptr; }
    m_initialized = false;
}

bool SDL2GraphicsBackend::AllocateBuffer(int width, int height, uint8_t*& outBuffer)
{ outBuffer = new uint8_t[width*height]; memset(outBuffer, 0, width*height); return true; }

void SDL2GraphicsBackend::FreeBuffer(uint8_t* buffer)
{ if (buffer) delete[] buffer; }

bool SDL2GraphicsBackend::SetPalette(const uint8_t* palette, int paletteSize)
{ if (!palette || paletteSize < 768) return false; memcpy(m_paletteRGB, palette, 768); return CreateSDLPalette(); }

bool SDL2GraphicsBackend::Present(const uint8_t* buffer, int width, int height)
{
    if (!m_renderer || !m_texture) return false;
    
    // Log frame count periodically
    static int frame_count = 0;
    frame_count++;
    if (frame_count % 100 == 0) {
        std::ofstream log("C:\\originalhr\\HoverRace\\Release\\sdl2_debug.log", std::ios::app);
        log << "Frame: " << frame_count << std::endl;
        log.close();
    }
    
    // Convert 8-bit indexed palette data to RGB888 for rendering
    uint8_t* rgb_buffer = new uint8_t[width * height * 3];
    for (int i = 0; i < width * height; i++) {
        uint8_t index = buffer[i];
        rgb_buffer[i*3 + 0] = m_paletteRGB[index*3 + 0];  // R
        rgb_buffer[i*3 + 1] = m_paletteRGB[index*3 + 1];  // G
        rgb_buffer[i*3 + 2] = m_paletteRGB[index*3 + 2];  // B
    }
    
    SDL_UpdateTexture(m_texture, nullptr, rgb_buffer, width * 3);
    delete[] rgb_buffer;
    
    SDL_RenderClear(m_renderer);
    SDL_Rect r = {0, 0, width, height};
    SDL_RenderCopy(m_renderer, m_texture, nullptr, &r);
    SDL_RenderPresent(m_renderer);
    return true;
}

bool SDL2GraphicsBackend::Clear(uint8_t color)
{
    if (!m_renderer) return false;
    SDL_Color c = {0, 0, 0, 255};
    if (m_paletteRGB && color < 256) { c.r = m_paletteRGB[color*3]; c.g = m_paletteRGB[color*3+1]; c.b = m_paletteRGB[color*3+2]; }
    SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, 255);
    SDL_RenderClear(m_renderer);
    SDL_RenderPresent(m_renderer);
    return true;
}

bool SDL2GraphicsBackend::CreateSDLPalette()
{
    if (!m_paletteRGB) return false;
    if (m_sdlPalette) delete[] m_sdlPalette;
    m_sdlPalette = new SDL_Color[256];
    if (!m_sdlPalette) return false;
    for (int i = 0; i < 256; i++)
    { m_sdlPalette[i].r = m_paletteRGB[i*3]; m_sdlPalette[i].g = m_paletteRGB[i*3+1]; m_sdlPalette[i].b = m_paletteRGB[i*3+2]; m_sdlPalette[i].a = 255; }
    if (SDL_SetPaletteColors(SDL_AllocPalette(256), m_sdlPalette, 0, 256) < 0) return false;
    return true;
}

#else
SDL2GraphicsBackend::SDL2GraphicsBackend() : m_window(nullptr), m_renderer(nullptr), m_texture(nullptr), m_paletteRGB(nullptr), m_sdlPalette(nullptr), m_width(0), m_height(0), m_initialized(false) {}
SDL2GraphicsBackend::~SDL2GraphicsBackend() {}
bool SDL2GraphicsBackend::Initialize(void* windowHandle, int width, int height) { return false; }
void SDL2GraphicsBackend::Shutdown() {}
bool SDL2GraphicsBackend::AllocateBuffer(int width, int height, uint8_t*& outBuffer) { return false; }
void SDL2GraphicsBackend::FreeBuffer(uint8_t* buffer) {}
bool SDL2GraphicsBackend::SetPalette(const uint8_t* palette, int paletteSize) { return false; }
bool SDL2GraphicsBackend::Present(const uint8_t* buffer, int width, int height) { return false; }
bool SDL2GraphicsBackend::Clear(uint8_t color) { return false; }
bool SDL2GraphicsBackend::CreateSDLPalette() { return false; }
#endif
