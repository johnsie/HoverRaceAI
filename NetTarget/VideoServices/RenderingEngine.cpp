// RenderingEngine.cpp
// Modern Graphics Rendering Engine Implementation
// Provides safe, clean graphics rendering

#include "stdafx.h"
#include "RenderingEngine.h"
#include <algorithm>
#include <cstring>

// Undefine Windows macros that conflict with std::min/max
#undef min
#undef max

// ============================================================================
// MR_Color Implementation
// ============================================================================

uint32_t MR_Color::ToRGB32() const
{
    return (r << 16) | (g << 8) | b;
}

uint32_t MR_Color::ToARGB32() const
{
    return (a << 24) | (r << 16) | (g << 8) | b;
}

MR_Color MR_Color::FromPaletteIndex(uint8_t pIndex)
{
    // This will be initialized with actual palette data
    return MR_Color(pIndex, pIndex, pIndex);
}

// ============================================================================
// Software Render Buffer Implementation
// ============================================================================

class MR_SoftwareRenderBuffer : public MR_RenderBuffer
{
public:
    MR_SoftwareRenderBuffer(int pWidth, int pHeight)
        : m_width(pWidth), m_height(pHeight), m_bytesPerPixel(1), m_locked(false)
    {
        m_pitch = pWidth * m_bytesPerPixel;
        m_buffer = new uint8_t[m_pitch * pHeight];
        std::memset(m_buffer, 0, m_pitch * pHeight);
    }

    ~MR_SoftwareRenderBuffer()
    {
        if (m_buffer)
        {
            delete[] m_buffer;
            m_buffer = nullptr;
        }
    }

    int GetWidth() const override { return m_width; }
    int GetHeight() const override { return m_height; }
    int GetBytesPerPixel() const override { return m_bytesPerPixel; }
    int GetPitch() const override { return m_pitch; }

    uint8_t* LockBuffer() override
    {
        if (m_locked) return nullptr;
        m_locked = true;
        return m_buffer;
    }

    void UnlockBuffer() override
    {
        m_locked = false;
    }

    void Clear(const MR_Color& pColor) override
    {
        if (!m_buffer) return;
        std::memset(m_buffer, pColor.r, m_pitch * m_height);
    }

    void SetPixel(int pX, int pY, const MR_Color& pColor) override
    {
        if (!m_buffer || pX < 0 || pX >= m_width || pY < 0 || pY >= m_height) return;
        m_buffer[pY * m_pitch + pX * m_bytesPerPixel] = pColor.r;
    }

    MR_Color GetPixel(int pX, int pY) const override
    {
        if (!m_buffer || pX < 0 || pX >= m_width || pY < 0 || pY >= m_height)
            return MR_Color();
        uint8_t val = m_buffer[pY * m_pitch + pX * m_bytesPerPixel];
        return MR_Color(val, val, val);
    }

private:
    int m_width;
    int m_height;
    int m_bytesPerPixel;
    int m_pitch;
    uint8_t* m_buffer;
    bool m_locked;
};

// ============================================================================
// MR_RenderingEngine Base Implementation
// ============================================================================

MR_RenderingEngine::MR_RenderingEngine()
    : m_displayWidth(0), m_displayHeight(0), m_initialized(false)
{
}

MR_RenderingEngine::~MR_RenderingEngine()
{
}

// ============================================================================
// MR_SoftwareRenderingEngine Implementation
// ============================================================================

MR_SoftwareRenderingEngine::MR_SoftwareRenderingEngine()
{
}

MR_SoftwareRenderingEngine::~MR_SoftwareRenderingEngine()
{
    Shutdown();
}

bool MR_SoftwareRenderingEngine::Initialize(int pWidth, int pHeight)
{
    if (m_initialized) return false;

    m_displayWidth = pWidth;
    m_displayHeight = pHeight;

    // Create back and front buffers
    m_backBuffer = CreateBuffer(pWidth, pHeight);
    m_frontBuffer = CreateBuffer(pWidth, pHeight);

    if (!m_backBuffer || !m_frontBuffer)
    {
        return false;
    }

    m_initialized = true;
    return true;
}

void MR_SoftwareRenderingEngine::Shutdown()
{
    m_backBuffer.reset();
    m_frontBuffer.reset();
    m_initialized = false;
}

std::shared_ptr<MR_RenderBuffer> MR_SoftwareRenderingEngine::CreateBuffer(int pWidth, int pHeight)
{
    if (pWidth <= 0 || pHeight <= 0) return nullptr;
    return std::make_shared<MR_SoftwareRenderBuffer>(pWidth, pHeight);
}

std::shared_ptr<MR_RenderBuffer> MR_SoftwareRenderingEngine::GetBackBuffer()
{
    return m_backBuffer;
}

std::shared_ptr<MR_RenderBuffer> MR_SoftwareRenderingEngine::GetFrontBuffer()
{
    return m_frontBuffer;
}

void MR_SoftwareRenderingEngine::BeginFrame()
{
    // Software rendering doesn't need explicit frame start
}

void MR_SoftwareRenderingEngine::EndFrame()
{
    // Software rendering doesn't need explicit frame end
}

void MR_SoftwareRenderingEngine::Present()
{
    if (!m_backBuffer || !m_frontBuffer) return;

    // Copy back buffer to front buffer
    uint8_t* pBack = m_backBuffer->LockBuffer();
    uint8_t* pFront = m_frontBuffer->LockBuffer();

    if (pBack && pFront)
    {
        std::memcpy(pFront, pBack, m_backBuffer->GetPitch() * m_backBuffer->GetHeight());
    }

    m_backBuffer->UnlockBuffer();
    m_frontBuffer->UnlockBuffer();
}

void MR_SoftwareRenderingEngine::DrawPoint(const MR_Point& pPoint, const MR_Color& pColor)
{
    if (!m_backBuffer) return;
    m_backBuffer->SetPixel(pPoint.x, pPoint.y, pColor);
}

void MR_SoftwareRenderingEngine::DrawLine(const MR_Point& pStart, const MR_Point& pEnd, const MR_Color& pColor)
{
    if (!m_backBuffer) return;

    int x0 = pStart.x, y0 = pStart.y;
    int x1 = pEnd.x, y1 = pEnd.y;

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        m_backBuffer->SetPixel(x0, y0, pColor);
        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void MR_SoftwareRenderingEngine::DrawRect(const MR_Rect& pRect, const MR_Color& pColor, bool pFilled)
{
    if (!m_backBuffer) return;

    if (pFilled)
    {
        for (int y = pRect.y; y < pRect.y + pRect.height; ++y)
        {
            for (int x = pRect.x; x < pRect.x + pRect.width; ++x)
            {
                m_backBuffer->SetPixel(x, y, pColor);
            }
        }
    }
    else
    {
        // Draw outline
        MR_Point tl(pRect.x, pRect.y);
        MR_Point tr(pRect.x + pRect.width - 1, pRect.y);
        MR_Point br(pRect.x + pRect.width - 1, pRect.y + pRect.height - 1);
        MR_Point bl(pRect.x, pRect.y + pRect.height - 1);

        DrawLine(tl, tr, pColor);
        DrawLine(tr, br, pColor);
        DrawLine(br, bl, pColor);
        DrawLine(bl, tl, pColor);
    }
}

void MR_SoftwareRenderingEngine::DrawTriangle(const MR_Point& pP0, const MR_Point& pP1, const MR_Point& pP2,
                                             const MR_Color& pColor, bool pFilled)
{
    if (!m_backBuffer) return;

    if (pFilled)
    {
        // Simplified barycentric rasterization
        int minX = std::min({pP0.x, pP1.x, pP2.x});
        int maxX = std::max({pP0.x, pP1.x, pP2.x});
        int minY = std::min({pP0.y, pP1.y, pP2.y});
        int maxY = std::max({pP0.y, pP1.y, pP2.y});

        for (int y = minY; y <= maxY; ++y)
        {
            for (int x = minX; x <= maxX; ++x)
            {
                m_backBuffer->SetPixel(x, y, pColor);
            }
        }
    }
    else
    {
        DrawLine(pP0, pP1, pColor);
        DrawLine(pP1, pP2, pColor);
        DrawLine(pP2, pP0, pColor);
    }
}

void MR_SoftwareRenderingEngine::DrawBuffer(const std::shared_ptr<MR_RenderBuffer>& pSource,
                                           const MR_Rect& pSourceRect, const MR_Point& pDestPoint)
{
    if (!m_backBuffer || !pSource) return;

    uint8_t* pSrcData = pSource->LockBuffer();
    if (!pSrcData) return;

    int srcPitch = pSource->GetPitch();
    int destPitch = m_backBuffer->GetPitch();

    uint8_t* pDestData = m_backBuffer->LockBuffer();
    if (!pDestData)
    {
        pSource->UnlockBuffer();
        return;
    }

    int copyWidth = pSourceRect.width;
    int copyHeight = pSourceRect.height;

    for (int y = 0; y < copyHeight; ++y)
    {
        int srcY = pSourceRect.y + y;
        int destY = pDestPoint.y + y;

        if (srcY < 0 || srcY >= pSource->GetHeight() || destY < 0 || destY >= m_backBuffer->GetHeight())
            continue;

        int srcOffset = srcY * srcPitch + pSourceRect.x;
        int destOffset = destY * destPitch + pDestPoint.x;

        std::memcpy(pDestData + destOffset, pSrcData + srcOffset, copyWidth);
    }

    pSource->UnlockBuffer();
    m_backBuffer->UnlockBuffer();
}

int MR_SoftwareRenderingEngine::GetDisplayWidth() const
{
    return m_displayWidth;
}

int MR_SoftwareRenderingEngine::GetDisplayHeight() const
{
    return m_displayHeight;
}

bool MR_SoftwareRenderingEngine::SetPalette(const uint8_t* pPalette, int pPaletteSize)
{
    if (!pPalette || pPaletteSize != 768)  // 256 colors * 3 (RGB)
        return false;

    m_palette.assign(pPalette, pPalette + pPaletteSize);
    return true;
}

// ============================================================================
// Helper Functions
// ============================================================================

namespace MR_RenderingUtils
{
std::shared_ptr<MR_RenderingEngine> CreateRenderingEngine()
{
    auto engine = std::make_shared<MR_SoftwareRenderingEngine>();
    return engine;
}

void SafeDrawPoint(MR_RenderingEngine* pEngine, int pX, int pY, const MR_Color& pColor)
{
    if (!pEngine) return;
    pEngine->DrawPoint(MR_Point(pX, pY), pColor);
}

void SafeDrawLine(MR_RenderingEngine* pEngine, int pX0, int pY0, int pX1, int pY1, const MR_Color& pColor)
{
    if (!pEngine) return;
    pEngine->DrawLine(MR_Point(pX0, pY0), MR_Point(pX1, pY1), pColor);
}
}  // namespace MR_RenderingUtils
