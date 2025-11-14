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
    
    // Create renderer with VSYNC enabled to synchronize with monitor refresh rate
    // This prevents flickering by ensuring Present() waits for the next vertical blank
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer) { 
        log << "ERROR: SDL_CreateRenderer with VSYNC failed, trying without VSYNC: " << SDL_GetError() << std::endl; 
        log.flush();
        // Fallback: try without VSYNC
        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_SOFTWARE);
        if (!m_renderer) { 
            log << "ERROR: SDL_CreateRenderer also failed without VSYNC: " << SDL_GetError() << std::endl; 
            log.close(); 
            SDL_DestroyWindow(m_window); 
            SDL_QuitSubSystem(SDL_INIT_VIDEO); 
            return false; 
        }
    }
    log << "SDL_CreateRenderer OK (software, VSYNC enabled)" << std::endl; log.flush();
    
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
    
    // Diagnostic: Check buffer for actual rendering data
    static int frame_diag_count = 0;
    bool should_log_buffer = (frame_diag_count < 5) || (frame_count % 500 == 0);
    
    int non_zero_count = 0;
    int max_index = 0;
    int bad_index_count = 0;
    for (int i = 0; i < width * height; i++) {
        if (buffer[i] != 0) non_zero_count++;
        if (buffer[i] > max_index) max_index = buffer[i];
        if (buffer[i] >= 256) bad_index_count++;
    }
    
    FILE* bufferLog = nullptr;
    if (should_log_buffer || bad_index_count > 0) {
        bufferLog = fopen("C:\\originalhr\\HoverRace\\Release\\sdl2_buffer_analysis.log", "a");
        if (bufferLog) {
            fprintf(bufferLog, "Frame %d: Buffer analysis: width=%d, height=%d\n", frame_count, width, height);
            fprintf(bufferLog, "  Non-zero pixels: %d/%d (%.1f%%)\n", non_zero_count, width*height, 100.0*non_zero_count/(width*height));
            fprintf(bufferLog, "  Max palette index found: %d\n", max_index);
            if (bad_index_count > 0) {
                fprintf(bufferLog, "  WARNING: Found %d pixels with INVALID palette indices (>= 256)!\n", bad_index_count);
                // Find and log locations of bad indices
                int logged = 0;
                for (int i = 0; i < width * height && logged < 10; i++) {
                    if (buffer[i] >= 256) {
                        int x = i % width;
                        int y = i / width;
                        fprintf(bufferLog, "    Invalid index %d at pixel (%d, %d)\n", buffer[i], x, y);
                        logged++;
                    }
                }
            }
            frame_diag_count++;
        }
    }
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t index = buffer[y * width + x];  // Linear stride = width
            
            // SAFETY: Check palette index is valid and clamp to valid range
            if (index >= 256) {
                index = 0;  // Default to black for invalid indices
                if (bufferLog && should_log_buffer) {
                    fprintf(bufferLog, "  Clamped invalid index to 0 at (x=%d, y=%d)\n", x, y);
                }
            }
            
            // EXTRA FIX: Detect isolated corruption artifacts (single pixels surrounded by different colors)
            // These are typically from ObjFac1 bitmap rendering buffer overruns
            // Pattern: if a pixel is very different from neighbors AND appears sporadically, it's likely an artifact
            BOOL is_isolated_artifact = FALSE;
            if (x > 0 && x < width-1 && y > 0 && y < height-1) {
                int left = buffer[y * width + (x-1)];
                int right = buffer[y * width + (x+1)];
                int up = buffer[y * width - width + x];
                int down = buffer[y * width + width + x];
                
                // If all 4 neighbors are the same color but this pixel is very different, it's likely an artifact
                if (left == right && up == down && left == up && left != 0 && left < 256) {
                    int diff_left = (index >= left) ? (index - left) : (left - index);
                    int diff_right = (index >= right) ? (index - right) : (right - index);
                    int diff_up = (index >= up) ? (index - up) : (up - index);
                    int diff_down = (index >= down) ? (index - down) : (down - index);
                    
                    // If differences are extreme (> 40 palette entries away), likely corruption
                    if (diff_left > 40 && diff_right > 40 && diff_up > 40 && diff_down > 40) {
                        is_isolated_artifact = TRUE;
                    }
                }
            }
            
            // Replace artifact with neighbor color to smooth out corruption
            if (is_isolated_artifact) {
                if (x > 0) index = buffer[y * width + (x-1)];  // Use left neighbor
            }
            
            int offset = y * pitch + x * 3;
            rgb_buffer[offset + 0] = m_paletteRGB[index*3 + 0];  // R
            rgb_buffer[offset + 1] = m_paletteRGB[index*3 + 1];  // G
            rgb_buffer[offset + 2] = m_paletteRGB[index*3 + 2];  // B
        }
    }
    
    if (bufferLog && should_log_buffer) {
        fprintf(bufferLog, "\n");
        fflush(bufferLog);
        fclose(bufferLog);
    }
    
    // Diagnostic: log first few frames' buffer content
    static bool diagLogged = false;
    if (!diagLogged && frame_count == 1) {
        diagLogged = true;
        FILE* diagLog = fopen("C:\\originalhr\\HoverRace\\Release\\sdl2_present_diag.log", "w");
        if (diagLog) {
            fprintf(diagLog, "Diagnostic buffer check (first Present call, Frame %d):\n", frame_count);
            fprintf(diagLog, "width=%d, height=%d, pitch=%d\n", width, height, pitch);
            fprintf(diagLog, "Buffer total size: %d bytes\n", width * height);
            fprintf(diagLog, "RGB buffer total size: %d bytes\n", pitch * height);
            fprintf(diagLog, "\nSample from center (x=%d, y=%d to y=%d):\n", width/2, height/2-2, height/2+2);
            for (int y = height/2-2; y <= height/2+2 && y < height; y++) {
                fprintf(diagLog, "Row %d (x=0-40): ", y);
                for (int x = 0; x < 40 && x < width; x++) {
                    uint8_t idx = buffer[y * width + x];
                    fprintf(diagLog, "%3d ", idx);
                }
                fprintf(diagLog, "\n");
            }
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
