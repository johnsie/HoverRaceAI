#ifndef VIDEOBUFFER_SDL2_ADAPTER_H
#define VIDEOBUFFER_SDL2_ADAPTER_H

// Adapter layer to integrate SDL2Graphics with existing VideoBuffer code
// This maintains backward compatibility while using modern SDL2+OpenGL rendering

#include "SDL2Graphics.h"
#include <cstdint>

class VideoBufferSDL2Adapter
{
public:
    VideoBufferSDL2Adapter();
    ~VideoBufferSDL2Adapter();

    // Initialize graphics
    bool Initialize(void* windowHandle, int width, int height);
    void Shutdown();

    // Buffer operations (match VideoBuffer interface)
    bool Lock(uint8_t*& outBuffer);
    bool Unlock(uint8_t* pBuffer = nullptr);  // Optional buffer parameter for display
    
    // Palette operations
    bool SetPalette(const uint8_t* paletteRGB);
    
    // Utility
    bool IsInitialized() const { return m_backend.IsInitialized(); }
    int GetWidth() const { return m_backend.GetWidth(); }
    int GetHeight() const { return m_backend.GetHeight(); }
    uint8_t* GetBuffer() const { return m_buffer; }

private:
    SDL2GraphicsBackend m_backend;
    uint8_t* m_buffer;
    int m_width;
    int m_height;
    bool m_locked;
};

#endif // VIDEOBUFFER_SDL2_ADAPTER_H
