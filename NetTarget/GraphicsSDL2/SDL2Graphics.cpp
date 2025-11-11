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
    // Use RGB24 (3 bytes per pixel, no padding) instead of RGB888 to ensure correct pitch handling
    m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);
    if (!m_texture) { 
        log << "ERROR: SDL_CreateTexture failed: " << SDL_GetError() << std::endl; 
        log.close(); 
        SDL_DestroyRenderer(m_renderer); 
        SDL_DestroyWindow(m_window); 
        SDL_QuitSubSystem(SDL_INIT_VIDEO); 
        return false; 
    }
    log << "SDL_CreateTexture OK (RGB24 format)" << std::endl; log.flush();
    
    // Allocate palette buffer but don't initialize with any data - wait for SetPalette()
    // to provide the real game palette loaded from track files
    if (!m_paletteRGB) { 
        m_paletteRGB = new uint8_t[768];
        memset(m_paletteRGB, 0, 768);  // Initialize to zero - will be filled by SetPalette
        log << "Palette buffer allocated (0 bytes)" << std::endl; log.flush();
    }
    
    // Don't call CreateSDLPalette yet - we don't have valid palette data
    // CreateSDLPalette will be called from SetPalette once real palette is available
    
    m_initialized = true;
    log << "Initialization successful (waiting for SetPalette to provide real palette)" << std::endl; log.close();
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
{ 
    if (!palette || paletteSize < 768) {
        FILE* log = fopen("C:\\originalhr\\HoverRace\\Release\\sdl2_palette_error.log", "a");
        if (log) {
            fprintf(log, "ERROR: SetPalette called with invalid params - palette=%p, paletteSize=%d\n", palette, paletteSize);
            fclose(log);
        }
        return false; 
    }
    
    memcpy(m_paletteRGB, palette, 768);
    
    FILE* log = fopen("C:\\originalhr\\HoverRace\\Release\\sdl2_palette_debug.log", "w");
    if (log) {
        fprintf(log, "SetPalette called successfully - logging ALL 256 colors (RGB):\n");
        fprintf(log, "========================================================\n\n");
        for (int i = 0; i < 256; i++) {
            int r = m_paletteRGB[i*3 + 0];
            int g = m_paletteRGB[i*3 + 1];
            int b = m_paletteRGB[i*3 + 2];
            
            // Mark grayscale colors (where R=G=B)
            int is_gray = (r == g && g == b) ? 1 : 0;
            
            if (i % 8 == 0) fprintf(log, "Index %3d-%3d: ", i, i+7);
            fprintf(log, "(%3d,%3d,%3d)%s ", r, g, b, is_gray ? "*" : " ");
            if (i % 8 == 7) fprintf(log, "\n");
        }
        fprintf(log, "\n* = Grayscale color (R=G=B)\n");
        
        // Count grayscale vs colored
        int gray_count = 0, color_count = 0;
        for (int i = 0; i < 256; i++) {
            int r = m_paletteRGB[i*3 + 0];
            int g = m_paletteRGB[i*3 + 1];
            int b = m_paletteRGB[i*3 + 2];
            if (r == g && g == b) gray_count++;
            else color_count++;
        }
        fprintf(log, "\nSummary: %d grayscale colors, %d colored colors\n", gray_count, color_count);
        fflush(log);
        fclose(log);
    }
    
    return CreateSDLPalette(); 
}

bool SDL2GraphicsBackend::Present(const uint8_t* buffer, int width, int height)
{
    if (!m_renderer || !m_texture || !m_paletteRGB || !buffer) return false;
    
    // Verify dimensions match texture
    if (width != m_width || height != m_height) {
        FILE* errorLog = fopen("C:\\originalhr\\HoverRace\\Release\\sdl2_present_error.log", "a");
        if (errorLog) {
            fprintf(errorLog, "ERROR: Present dimension mismatch! Expected %dx%d, got %dx%d\n", 
                    m_width, m_height, width, height);
            fflush(errorLog);
            fclose(errorLog);
        }
        return false;
    }
    
    // Log frame count periodically
    static int frame_count = 0;
    frame_count++;
    if (frame_count % 100 == 0) {
        std::ofstream log("C:\\originalhr\\HoverRace\\Release\\sdl2_debug.log", std::ios::app);
        log << "Frame: " << frame_count << std::endl;
        log.close();
    }
    
    // Debug: Log the Present call parameters
    if (frame_count == 1 || frame_count % 500 == 0) {
        FILE* debugLog = fopen("C:\\originalhr\\HoverRace\\Release\\Debug_SDL2_Present.log", "a");
        if (debugLog) {
            fprintf(debugLog, "Present called: width=%d, height=%d, m_width=%d, m_height=%d, pitch=%d, buffer=%p\n",
                    width, height, m_width, m_height, width * 3, buffer);
            fflush(debugLog);
            fclose(debugLog);
        }
    }
    
    // Convert 8-bit indexed palette data to 24-bit RGB for rendering
    // CRITICAL: Use exact pitch (width * 3) with NO padding - SDL_PIXELFORMAT_RGB24 expects contiguous data
    int pitch = width * 3;  // RGB24 with exactly 3 bytes per pixel, no padding
    uint8_t* rgb_buffer = new uint8_t[pitch * height];
    
    // IMPORTANT: buffer is assumed to have stride == width (linear)
    // This must match mLineLen from VideoBuffer!
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t index = buffer[y * width + x];  // Linear stride = width
            int offset = y * pitch + x * 3;
            rgb_buffer[offset + 0] = m_paletteRGB[index*3 + 0];  // R
            rgb_buffer[offset + 1] = m_paletteRGB[index*3 + 1];  // G
            rgb_buffer[offset + 2] = m_paletteRGB[index*3 + 2];  // B
        }
    }
    
    // Diagnostic: log last row indices on first successful call
    static bool diagLogged = false;
    if (!diagLogged) {
        diagLogged = true;
        FILE* diagLog = fopen("C:\\originalhr\\HoverRace\\Release\\sdl2_present_diag.log", "w");
        if (diagLog) {
            fprintf(diagLog, "Diagnostic buffer check (first Present call):\n");
            fprintf(diagLog, "width=%d, height=%d, pitch=%d\n", width, height, pitch);
            fprintf(diagLog, "Buffer total size: %d bytes\n", width * height);
            fprintf(diagLog, "RGB buffer total size: %d bytes\n", pitch * height);
            fprintf(diagLog, "\nSample from near bottom (y=%d):\n", height - 2);
            int y = height - 2;
            fprintf(diagLog, "Row %d first 40 indices: ", y);
            for (int x = 0; x < 40 && x < width; x++) {
                uint8_t idx = buffer[y * width + x];
                fprintf(diagLog, "%3d ", idx);
            }
            fprintf(diagLog, "\n");
            fflush(diagLog);
            fclose(diagLog);
        }
    }
    
    SDL_UpdateTexture(m_texture, nullptr, rgb_buffer, pitch);
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
