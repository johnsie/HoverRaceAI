// ModernVideoBuffer.h
// Safe wrapper around modern rendering engine for existing code compatibility

#ifndef MR_MODERN_VIDEO_BUFFER_H
#define MR_MODERN_VIDEO_BUFFER_H

#include "RenderingEngine.h"
#include "VideoBuffer.h"
#include <memory>

#ifdef MR_VIDEO_SERVICES
#define MR_DllDeclare __declspec(dllexport)
#else
#define MR_DllDeclare __declspec(dllimport)
#endif

// ============================================================================
// Modern VideoBuffer Adapter
// Bridges existing MR_VideoBuffer API to modern rendering engine
// ============================================================================

class MR_DllDeclare MR_ModernVideoBuffer
{
public:
    MR_ModernVideoBuffer();
    ~MR_ModernVideoBuffer();

    // Initialize with modern rendering engine
    bool Initialize(int pWidth, int pHeight);
    void Shutdown();
    bool IsInitialized() const;

    // Buffer access (maintains backward compatibility)
    int GetWidth() const;
    int GetHeight() const;
    int GetPitch() const;

    uint8_t* LockBuffer();
    void UnlockBuffer();

    // Rendering operations
    void Clear(const MR_Color& pColor = MR_Color(0, 0, 0));
    void Present();

    // Get the underlying rendering engine
    MR_RenderingEngine* GetRenderingEngine() { return m_renderingEngine.get(); }
    const MR_RenderingEngine* GetRenderingEngine() const { return m_renderingEngine.get(); }

    // Palette management
    bool SetPalette(const uint8_t* pPalette, int pPaletteSize);

private:
    std::shared_ptr<MR_RenderingEngine> m_renderingEngine;
    std::shared_ptr<MR_RenderBuffer> m_currentBuffer;
    int m_width;
    int m_height;
    bool m_locked;
};

// ============================================================================
// Safe Rendering Context
// RAII wrapper for rendering operations
// ============================================================================

class MR_DllDeclare MR_RenderContext
{
public:
    explicit MR_RenderContext(MR_ModernVideoBuffer* pBuffer);
    ~MR_RenderContext();

    // Begin/End frame operations
    bool BeginFrame();
    void EndFrame();
    bool IsFrameActive() const { return m_frameActive; }

    // Get buffer for drawing operations
    std::shared_ptr<MR_RenderBuffer> GetBuffer() const;

    // Prevent copying
    MR_RenderContext(const MR_RenderContext&) = delete;
    MR_RenderContext& operator=(const MR_RenderContext&) = delete;

private:
    MR_ModernVideoBuffer* m_buffer;
    bool m_frameActive;
};

// ============================================================================
// Viewport Rendering Helper
// Modern safe viewport for 3D to 2D projection
// ============================================================================

class MR_DllDeclare MR_ModernViewport
{
public:
    MR_ModernViewport(std::shared_ptr<MR_RenderBuffer> pBuffer);
    ~MR_ModernViewport();

    // Setup viewport dimensions and projection
    void Setup(int pX, int pY, int pWidth, int pHeight);
    void SetupProjection(double pFOV, double pNear, double pFar);

    // Drawing operations
    void DrawPoint(int pX, int pY, const MR_Color& pColor);
    void DrawLine(int pX0, int pY0, int pX1, int pY1, const MR_Color& pColor);
    void DrawTriangle(int pX0, int pY0, int pX1, int pY1, int pX2, int pY2, const MR_Color& pColor);

    // Get viewport properties
    int GetX() const { return m_x; }
    int GetY() const { return m_y; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    std::shared_ptr<MR_RenderBuffer> m_buffer;
    int m_x, m_y;
    int m_width, m_height;
    double m_fov, m_near, m_far;

    bool IsPixelInViewport(int pX, int pY) const;
};

#undef MR_DllDeclare

#endif  // MR_MODERN_VIDEO_BUFFER_H
