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
    // If buffer is already locked, return immediately without clearing
    // This handles legitimate cases where Lock() is called multiple times
    // before Unlock() (e.g., during viewport setup or exception handling)
    if (m_locked)
    {
        // Already locked - just return the existing buffer pointer
        // Do NOT clear the buffer or modify m_locked state
        if (!m_buffer)
            return false;
        outBuffer = m_buffer;
        return true;
    }
    
    if (!m_buffer)
        return false;

    // DEFENSIVE CHECK: Verify buffer size is valid before memset
    // If width or height is invalid, this could cause massive memory write
    if (m_width <= 0 || m_height <= 0)
    {
        std::ofstream log("C:\\originalhr\\HoverRace\\Release\\sdl2_adapter_lock_error.log", std::ios::app);
        log << "ERROR in Lock(): Invalid dimensions - m_width=" << m_width << ", m_height=" << m_height << std::endl;
        log.flush();
        return false;
    }

    // Calculate total buffer size - use long long to detect overflow
    long long totalSize = (long long)m_width * (long long)m_height;
    if (totalSize > 1000000)  // Sanity check - reasonable max for retro game
    {
        std::ofstream log("C:\\originalhr\\HoverRace\\Release\\sdl2_adapter_lock_error.log", std::ios::app);
        log << "ERROR in Lock(): Buffer size too large - " << totalSize << " bytes (width=" << m_width << ", height=" << m_height << ")" << std::endl;
        log.flush();
        return false;
    }

    // Clear the buffer to black (index 0) at the start of each frame
    // This ensures no garbage from previous frames persists
    // while keeping the same buffer pointer for rendering consistency
    try {
        memset(m_buffer, 0, (size_t)totalSize);
    }
    catch(...) {
        std::ofstream log("C:\\originalhr\\HoverRace\\Release\\sdl2_adapter_lock_error.log", std::ios::app);
        log << "EXCEPTION during memset in Lock(): size=" << totalSize << " bytes" << std::endl;
        log.flush();
        return false;
    }

    m_locked = true;
    outBuffer = m_buffer;
    return true;
}

bool VideoBufferSDL2Adapter::Unlock(uint8_t* pBuffer)
{
    if (!m_locked)
        return false;

    m_locked = false;

    // Use provided buffer if given (from VideoBuffer.mBuffer), otherwise use internal buffer
    uint8_t* bufferToPresent = (pBuffer != nullptr) ? pBuffer : m_buffer;
    
    if (!bufferToPresent)
        return false;

    // Present the buffer
    std::ofstream log("C:\\originalhr\\HoverRace\\Release\\sdl2_adapter_present.log", std::ios::app);
    log << "Calling Present() with buffer=" << (void*)bufferToPresent << ", size=" << (m_width * m_height) << std::endl;
    log.flush();
    
    bool result = m_backend.Present(bufferToPresent, m_width, m_height);
    
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
