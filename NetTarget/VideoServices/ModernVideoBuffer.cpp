// ModernVideoBuffer.cpp
// Implementation of modern video buffer wrapper

#include "stdafx.h"
#include "ModernVideoBuffer.h"
#include "GraphicsUtilities.h"

// ============================================================================
// MR_ModernVideoBuffer Implementation
// ============================================================================

MR_ModernVideoBuffer::MR_ModernVideoBuffer()
    : m_width(0), m_height(0), m_locked(false)
{
    m_renderingEngine = MR_RenderingUtils::CreateRenderingEngine();
}

MR_ModernVideoBuffer::~MR_ModernVideoBuffer()
{
    Shutdown();
}

bool MR_ModernVideoBuffer::Initialize(int pWidth, int pHeight)
{
    if (!m_renderingEngine || m_renderingEngine->IsInitialized())
        return false;

    if (!m_renderingEngine->Initialize(pWidth, pHeight))
        return false;

    m_width = pWidth;
    m_height = pHeight;
    m_currentBuffer = m_renderingEngine->GetBackBuffer();

    return m_currentBuffer != nullptr;
}

void MR_ModernVideoBuffer::Shutdown()
{
    if (m_locked)
        UnlockBuffer();

    if (m_renderingEngine && m_renderingEngine->IsInitialized())
    {
        m_renderingEngine->Shutdown();
    }

    m_currentBuffer.reset();
    m_width = 0;
    m_height = 0;
}

bool MR_ModernVideoBuffer::IsInitialized() const
{
    return m_renderingEngine && m_renderingEngine->IsInitialized();
}

int MR_ModernVideoBuffer::GetWidth() const
{
    return m_width;
}

int MR_ModernVideoBuffer::GetHeight() const
{
    return m_height;
}

int MR_ModernVideoBuffer::GetPitch() const
{
    if (!m_currentBuffer)
        return 0;
    return m_currentBuffer->GetPitch();
}

uint8_t* MR_ModernVideoBuffer::LockBuffer()
{
    if (m_locked || !m_currentBuffer)
        return nullptr;

    uint8_t* pBuffer = m_currentBuffer->LockBuffer();
    if (pBuffer)
        m_locked = true;

    return pBuffer;
}

void MR_ModernVideoBuffer::UnlockBuffer()
{
    if (!m_locked || !m_currentBuffer)
        return;

    m_currentBuffer->UnlockBuffer();
    m_locked = false;
}

void MR_ModernVideoBuffer::Clear(const MR_Color& pColor)
{
    if (!m_currentBuffer)
        return;

    m_currentBuffer->Clear(pColor);
}

void MR_ModernVideoBuffer::Present()
{
    if (!m_renderingEngine)
        return;

    m_renderingEngine->Present();
}

bool MR_ModernVideoBuffer::SetPalette(const uint8_t* pPalette, int pPaletteSize)
{
    if (!m_renderingEngine)
        return false;

    return m_renderingEngine->SetPalette(pPalette, pPaletteSize);
}

// ============================================================================
// MR_RenderContext Implementation
// ============================================================================

MR_RenderContext::MR_RenderContext(MR_ModernVideoBuffer* pBuffer)
    : m_buffer(pBuffer), m_frameActive(false)
{
}

MR_RenderContext::~MR_RenderContext()
{
    if (m_frameActive)
        EndFrame();
}

bool MR_RenderContext::BeginFrame()
{
    if (!m_buffer || m_frameActive)
        return false;

    if (MR_RenderingEngine* engine = m_buffer->GetRenderingEngine())
    {
        engine->BeginFrame();
        m_frameActive = true;
        return true;
    }

    return false;
}

void MR_RenderContext::EndFrame()
{
    if (!m_buffer || !m_frameActive)
        return;

    if (MR_RenderingEngine* engine = m_buffer->GetRenderingEngine())
    {
        engine->EndFrame();
        engine->Present();
    }

    m_frameActive = false;
}

std::shared_ptr<MR_RenderBuffer> MR_RenderContext::GetBuffer() const
{
    if (!m_buffer)
        return nullptr;

    if (MR_RenderingEngine* engine = m_buffer->GetRenderingEngine())
    {
        return engine->GetBackBuffer();
    }

    return nullptr;
}

// ============================================================================
// MR_ModernViewport Implementation
// ============================================================================

MR_ModernViewport::MR_ModernViewport(std::shared_ptr<MR_RenderBuffer> pBuffer)
    : m_buffer(pBuffer), m_x(0), m_y(0), m_width(0), m_height(0),
      m_fov(60.0), m_near(0.1), m_far(1000.0)
{
}

MR_ModernViewport::~MR_ModernViewport()
{
}

void MR_ModernViewport::Setup(int pX, int pY, int pWidth, int pHeight)
{
    m_x = pX;
    m_y = pY;
    m_width = pWidth;
    m_height = pHeight;
}

void MR_ModernViewport::SetupProjection(double pFOV, double pNear, double pFar)
{
    m_fov = pFOV;
    m_near = pNear;
    m_far = pFar;
}

void MR_ModernViewport::DrawPoint(int pX, int pY, const MR_Color& pColor)
{
    if (!m_buffer || !IsPixelInViewport(pX, pY))
        return;

    m_buffer->SetPixel(pX, pY, pColor);
}

void MR_ModernViewport::DrawLine(int pX0, int pY0, int pX1, int pY1, const MR_Color& pColor)
{
    if (!m_buffer)
        return;

    // Bresenham line algorithm with viewport bounds checking
    int dx = abs(pX1 - pX0);
    int dy = abs(pY1 - pY0);
    int sx = pX0 < pX1 ? 1 : -1;
    int sy = pY0 < pY1 ? 1 : -1;
    int err = dx - dy;

    int x = pX0, y = pY0;
    while (true)
    {
        if (IsPixelInViewport(x, y))
            m_buffer->SetPixel(x, y, pColor);

        if (x == pX1 && y == pY1)
            break;

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y += sy;
        }
    }
}

void MR_ModernViewport::DrawTriangle(int pX0, int pY0, int pX1, int pY1, int pX2, int pY2, const MR_Color& pColor)
{
    if (!m_buffer)
        return;

    // Draw triangle outline
    DrawLine(pX0, pY0, pX1, pY1, pColor);
    DrawLine(pX1, pY1, pX2, pY2, pColor);
    DrawLine(pX2, pY2, pX0, pY0, pColor);
}

bool MR_ModernViewport::IsPixelInViewport(int pX, int pY) const
{
    return pX >= m_x && pX < (m_x + m_width) &&
           pY >= m_y && pY < (m_y + m_height);
}
