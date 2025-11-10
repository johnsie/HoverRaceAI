#include "VideoBufferSDL2Adapter.h"
#include <cstring>
#include <iostream>
#include <fstream>

VideoBufferSDL2Adapter::VideoBufferSDL2Adapter()
    : m_buffer(nullptr)
    , m_width(0)
    , m_height(0)
    , m_locked(false)
{
}

VideoBufferSDL2Adapter::~VideoBufferSDL2Adapter()
{
    Shutdown();
}

bool VideoBufferSDL2Adapter::Initialize(void* windowHandle, int width, int height)
{
    // Log to file for debugging
    std::ofstream log("C:\\originalhr\\HoverRace\\Release\\sdl2_adapter_debug.log", std::ios::app);
    log << "=== VideoBufferSDL2Adapter::Initialize called ===" << std::endl;
    log << "Width: " << width << ", Height: " << height << ", windowHandle: " << windowHandle << std::endl;
    log.flush();

    m_width = width;
    m_height = height;

    // Initialize SDL2 backend
    log << "Calling m_backend.Initialize..." << std::endl;
    log.flush();
    if (!m_backend.Initialize(windowHandle, width, height))
    {
        log << "ERROR: m_backend.Initialize returned false" << std::endl;
        log.close();
        std::cerr << "Failed to initialize SDL2 graphics backend" << std::endl;
        return false;
    }

    log << "m_backend.Initialize succeeded" << std::endl;
    log.flush();

    // Allocate buffer
    log << "Calling m_backend.AllocateBuffer..." << std::endl;
    log.flush();
    if (!m_backend.AllocateBuffer(width, height, m_buffer))
    {
        log << "ERROR: AllocateBuffer failed" << std::endl;
        log.close();
        std::cerr << "Failed to allocate graphics buffer" << std::endl;
        return false;
    }

    log << "VideoBufferSDL2Adapter initialized successfully" << std::endl;
    log.close();
    std::cout << "VideoBufferSDL2Adapter initialized: " << width << "x" << height << std::endl;
    return true;
}

void VideoBufferSDL2Adapter::Shutdown()
{
    if (m_buffer)
    {
        m_backend.FreeBuffer(m_buffer);
        m_buffer = nullptr;
    }
    m_backend.Shutdown();
}

bool VideoBufferSDL2Adapter::Lock(uint8_t*& outBuffer)
{
    // If buffer is already locked, it might be stuck from a previous crash
    // Log the attempt but proceed
    if (m_locked)
    {
        // Buffer is still locked from previous attempt
        // This indicates a crash happened without calling Unlock()
        // For robustness, we'll force-reset the lock
        std::ofstream log("C:\\originalhr\\HoverRace\\Release\\sdl2_adapter_debug.log", std::ios::app);
        log << "WARNING: Lock() called while m_locked=true! Forcibly resetting lock (possible crash recovery)" << std::endl;
        log.flush();
        m_locked = false;  // Force reset
    }
    
    if (!m_buffer)
        return false;

    m_locked = true;
    outBuffer = m_buffer;
    return true;
}

bool VideoBufferSDL2Adapter::Unlock()
{
    if (!m_locked || !m_buffer)
        return false;

    m_locked = false;

    // Present the buffer
    std::ofstream log("C:\\originalhr\\HoverRace\\Release\\sdl2_adapter_present.log", std::ios::app);
    log << "Calling Present()" << std::endl;
    log.flush();
    
    bool result = m_backend.Present(m_buffer, m_width, m_height);
    
    log << "Present() returned: " << (result ? "TRUE" : "FALSE") << std::endl;
    log.flush();
    
    return result;
}

bool VideoBufferSDL2Adapter::SetPalette(const uint8_t* paletteRGB)
{
    if (!paletteRGB)
        return false;

    // Log first few palette colors for debugging
    std::ofstream log("C:\\originalhr\\HoverRace\\Release\\sdl2_palette_debug.log", std::ios::app);
    log << "SetPalette called - first 5 colors:" << std::endl;
    for (int i = 0; i < 5; i++) {
        int r = paletteRGB[i*3 + 0];
        int g = paletteRGB[i*3 + 1];
        int b = paletteRGB[i*3 + 2];
        log << "  Color[" << i << "] = RGB(" << r << "," << g << "," << b << ")" << std::endl;
    }
    log.flush();

    return m_backend.SetPalette(paletteRGB, 768);  // 256 colors * 3 bytes
}
