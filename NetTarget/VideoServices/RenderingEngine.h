// RenderingEngine.h
// Modern Graphics Rendering Engine for HoverRace
// Provides a safe, clean abstraction for all graphics operations
//
// Copyright (c) 1995-1998 - Richard Langlois and Grokksoft Inc.
// Modern refactoring 2025

#ifndef MR_RENDERING_ENGINE_H
#define MR_RENDERING_ENGINE_H

#include "../Util/MR_Types.h"
#include <memory>
#include <vector>
#include <cstdint>

#ifdef MR_VIDEO_SERVICES
#define MR_DllDeclare __declspec(dllexport)
#else
#define MR_DllDeclare __declspec(dllimport)
#endif

// ============================================================================
// Color Management
// ============================================================================

class MR_DllDeclare MR_Color
{
public:
    MR_Color() : r(0), g(0), b(0), a(255) {}
    MR_Color(uint8_t pR, uint8_t pG, uint8_t pB, uint8_t pA = 255)
        : r(pR), g(pG), b(pB), a(pA) {}

    uint8_t r, g, b, a;  // RGBA

    // Conversion methods
    uint32_t ToRGB32() const;
    uint32_t ToARGB32() const;
    static MR_Color FromPaletteIndex(uint8_t pIndex);
    
    bool operator==(const MR_Color& pOther) const {
        return r == pOther.r && g == pOther.g && b == pOther.b && a == pOther.a;
    }
};

// ============================================================================
// 2D Geometry Types
// ============================================================================

struct MR_DllDeclare MR_Rect
{
    int x, y, width, height;
    
    MR_Rect() : x(0), y(0), width(0), height(0) {}
    MR_Rect(int pX, int pY, int pW, int pH) : x(pX), y(pY), width(pW), height(pH) {}
    
    int Right() const { return x + width; }
    int Bottom() const { return y + height; }
    bool Contains(int pX, int pY) const {
        return pX >= x && pX < Right() && pY >= y && pY < Bottom();
    }
};

struct MR_DllDeclare MR_Point
{
    int x, y;
    MR_Point() : x(0), y(0) {}
    MR_Point(int pX, int pY) : x(pX), y(pY) {}
};

// ============================================================================
// Texture/Buffer Interface
// ============================================================================

class MR_DllDeclare MR_RenderBuffer
{
public:
    virtual ~MR_RenderBuffer() = default;
    
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual int GetBytesPerPixel() const = 0;
    virtual int GetPitch() const = 0;
    
    virtual uint8_t* LockBuffer() = 0;
    virtual void UnlockBuffer() = 0;
    
    virtual void Clear(const MR_Color& pColor) = 0;
    virtual void SetPixel(int pX, int pY, const MR_Color& pColor) = 0;
    virtual MR_Color GetPixel(int pX, int pY) const = 0;
};

// ============================================================================
// Modern Rendering Pipeline
// ============================================================================

class MR_DllDeclare MR_RenderingEngine
{
public:
    MR_RenderingEngine();
    virtual ~MR_RenderingEngine();

    // Initialization
    virtual bool Initialize(int pWidth, int pHeight) = 0;
    virtual void Shutdown() = 0;
    virtual bool IsInitialized() const = 0;

    // Buffer management
    virtual std::shared_ptr<MR_RenderBuffer> CreateBuffer(int pWidth, int pHeight) = 0;
    virtual std::shared_ptr<MR_RenderBuffer> GetBackBuffer() = 0;
    virtual std::shared_ptr<MR_RenderBuffer> GetFrontBuffer() = 0;

    // Rendering operations
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void Present() = 0;

    // Primitive rendering
    virtual void DrawPoint(const MR_Point& pPoint, const MR_Color& pColor) = 0;
    virtual void DrawLine(const MR_Point& pStart, const MR_Point& pEnd, const MR_Color& pColor) = 0;
    virtual void DrawRect(const MR_Rect& pRect, const MR_Color& pColor, bool pFilled = false) = 0;
    virtual void DrawTriangle(const MR_Point& pP0, const MR_Point& pP1, const MR_Point& pP2,
                             const MR_Color& pColor, bool pFilled = false) = 0;

    // Texture/bitmap operations
    virtual void DrawBuffer(const std::shared_ptr<MR_RenderBuffer>& pSource,
                           const MR_Rect& pSourceRect, const MR_Point& pDestPoint) = 0;

    // Display properties
    virtual int GetDisplayWidth() const = 0;
    virtual int GetDisplayHeight() const = 0;
    virtual float GetAspectRatio() const {
        return static_cast<float>(GetDisplayWidth()) / GetDisplayHeight();
    }

    // Palette management
    virtual bool SetPalette(const uint8_t* pPalette, int pPaletteSize) = 0;
    virtual const uint8_t* GetPalette() const = 0;

protected:
    int m_displayWidth;
    int m_displayHeight;
    std::vector<uint8_t> m_palette;
    bool m_initialized;
};

// ============================================================================
// Software Rendering Backend (always available)
// ============================================================================

class MR_DllDeclare MR_SoftwareRenderingEngine : public MR_RenderingEngine
{
public:
    MR_SoftwareRenderingEngine();
    ~MR_SoftwareRenderingEngine();

    bool Initialize(int pWidth, int pHeight) override;
    void Shutdown() override;
    bool IsInitialized() const override { return m_initialized; }

    std::shared_ptr<MR_RenderBuffer> CreateBuffer(int pWidth, int pHeight) override;
    std::shared_ptr<MR_RenderBuffer> GetBackBuffer() override;
    std::shared_ptr<MR_RenderBuffer> GetFrontBuffer() override;

    void BeginFrame() override;
    void EndFrame() override;
    void Present() override;

    void DrawPoint(const MR_Point& pPoint, const MR_Color& pColor) override;
    void DrawLine(const MR_Point& pStart, const MR_Point& pEnd, const MR_Color& pColor) override;
    void DrawRect(const MR_Rect& pRect, const MR_Color& pColor, bool pFilled = false) override;
    void DrawTriangle(const MR_Point& pP0, const MR_Point& pP1, const MR_Point& pP2,
                     const MR_Color& pColor, bool pFilled = false) override;
    void DrawBuffer(const std::shared_ptr<MR_RenderBuffer>& pSource,
                   const MR_Rect& pSourceRect, const MR_Point& pDestPoint) override;

    int GetDisplayWidth() const override;
    int GetDisplayHeight() const override;

    bool SetPalette(const uint8_t* pPalette, int pPaletteSize) override;
    const uint8_t* GetPalette() const override { return m_palette.data(); }

private:
    std::shared_ptr<MR_RenderBuffer> m_backBuffer;
    std::shared_ptr<MR_RenderBuffer> m_frontBuffer;
};

// ============================================================================
// Helper Functions
// ============================================================================

namespace MR_RenderingUtils
{
    MR_DllDeclare std::shared_ptr<MR_RenderingEngine> CreateRenderingEngine();
    MR_DllDeclare void SafeDrawPoint(MR_RenderingEngine* pEngine, int pX, int pY, const MR_Color& pColor);
    MR_DllDeclare void SafeDrawLine(MR_RenderingEngine* pEngine, int pX0, int pY0, int pX1, int pY1, const MR_Color& pColor);
}

#undef MR_DllDeclare

#endif  // MR_RENDERING_ENGINE_H
