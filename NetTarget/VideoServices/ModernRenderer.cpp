// ModernRenderer.cpp
// Implementation of modern renderer classes

#include "stdafx.h"
#include "ModernRenderer.h"
#include <algorithm>
#include <cmath>

// Undefine Windows macros that conflict with std::min/max
#undef min
#undef max

// ============================================================================
// MR_BaseRenderer Implementation
// ============================================================================

void MR_BaseRenderer::SafeDrawPoint(int pX, int pY, const MR_Color& pColor)
{
    if (!m_engine)
        return;
    m_engine->DrawPoint(MR_Point(pX, pY), pColor);
}

void MR_BaseRenderer::SafeDrawLine(int pX0, int pY0, int pX1, int pY1, const MR_Color& pColor)
{
    if (!m_engine)
        return;
    m_engine->DrawLine(MR_Point(pX0, pY0), MR_Point(pX1, pY1), pColor);
}

void MR_BaseRenderer::SafeDrawRect(const MR_Rect& pRect, const MR_Color& pColor, bool pFilled)
{
    if (!m_engine)
        return;
    m_engine->DrawRect(pRect, pColor, pFilled);
}

// ============================================================================
// MR_SpriteRenderer Implementation
// ============================================================================

MR_SpriteRenderer::MR_SpriteRenderer()
    : m_initialized(false), m_batchActive(false)
{
}

MR_SpriteRenderer::~MR_SpriteRenderer()
{
    Shutdown();
}

bool MR_SpriteRenderer::Initialize(MR_RenderingEngine* pEngine)
{
    if (!pEngine || m_initialized)
        return false;

    m_engine = pEngine;
    m_initialized = true;
    return true;
}

void MR_SpriteRenderer::Shutdown()
{
    if (m_batchActive)
        EndBatch();

    m_batch.clear();
    m_engine = nullptr;
    m_initialized = false;
}

void MR_SpriteRenderer::BeginRender()
{
    if (m_engine)
        m_engine->BeginFrame();
}

void MR_SpriteRenderer::EndRender()
{
    if (m_engine)
        m_engine->EndFrame();
}

void MR_SpriteRenderer::DrawSprite(const std::shared_ptr<MR_RenderBuffer>& pSprite,
                                  int pX, int pY,
                                  float pScaleX, float pScaleY,
                                  float pRotation,
                                  uint8_t pAlpha)
{
    if (!m_engine || !pSprite)
        return;

    // For now, simple non-scaled, non-rotated rendering
    MR_Rect srcRect(0, 0, pSprite->GetWidth(), pSprite->GetHeight());
    m_engine->DrawBuffer(pSprite, srcRect, MR_Point(pX, pY));
}

void MR_SpriteRenderer::BeginBatch()
{
    if (m_batchActive)
        return;

    m_batch.clear();
    m_batchActive = true;
}

void MR_SpriteRenderer::EndBatch()
{
    if (!m_batchActive || !m_engine)
        return;

    for (const auto& item : m_batch)
    {
        if (item.sprite)
        {
            MR_Rect srcRect(0, 0, item.sprite->GetWidth(), item.sprite->GetHeight());
            m_engine->DrawBuffer(item.sprite, srcRect, MR_Point(item.x, item.y));
        }
    }

    m_batch.clear();
    m_batchActive = false;
}

void MR_SpriteRenderer::AddSpriteToBatch(const std::shared_ptr<MR_RenderBuffer>& pSprite,
                                        int pX, int pY)
{
    if (!m_batchActive || !pSprite)
        return;

    SpriteBatchItem item;
    item.sprite = pSprite;
    item.x = pX;
    item.y = pY;
    m_batch.push_back(item);
}

// ============================================================================
// MR_GeometryRenderer Implementation
// ============================================================================

MR_GeometryRenderer::MR_GeometryRenderer()
    : m_initialized(false)
{
    m_projectionMatrix = MR_Matrix4x4::Identity();
    m_viewMatrix = MR_Matrix4x4::Identity();
    m_modelMatrix = MR_Matrix4x4::Identity();
}

MR_GeometryRenderer::~MR_GeometryRenderer()
{
    Shutdown();
}

bool MR_GeometryRenderer::Initialize(MR_RenderingEngine* pEngine)
{
    if (!pEngine || m_initialized)
        return false;

    m_engine = pEngine;
    m_initialized = true;
    return true;
}

void MR_GeometryRenderer::Shutdown()
{
    m_engine = nullptr;
    m_initialized = false;
}

void MR_GeometryRenderer::BeginRender()
{
    if (m_engine)
        m_engine->BeginFrame();
}

void MR_GeometryRenderer::EndRender()
{
    if (m_engine)
        m_engine->EndFrame();
}

void MR_GeometryRenderer::SetProjectionMatrix(const MR_Matrix4x4& pMatrix)
{
    m_projectionMatrix = pMatrix;
}

void MR_GeometryRenderer::SetViewMatrix(const MR_Matrix4x4& pMatrix)
{
    m_viewMatrix = pMatrix;
}

void MR_GeometryRenderer::SetModelMatrix(const MR_Matrix4x4& pMatrix)
{
    m_modelMatrix = pMatrix;
}

void MR_GeometryRenderer::DrawLine3D(const MR_Vector3D& pStart, const MR_Vector3D& pEnd,
                                    const MR_Color& pColor)
{
    if (!m_engine)
        return;

    MR_Point p0, p1;
    if (Project3DTo2D(pStart, p0) && Project3DTo2D(pEnd, p1))
    {
        m_engine->DrawLine(p0, p1, pColor);
    }
}

void MR_GeometryRenderer::DrawTriangle3D(const MR_Vector3D& pP0, const MR_Vector3D& pP1, const MR_Vector3D& pP2,
                                        const MR_Color& pColor, bool pFilled)
{
    if (!m_engine)
        return;

    MR_Point p0, p1, p2;
    if (Project3DTo2D(pP0, p0) && Project3DTo2D(pP1, p1) && Project3DTo2D(pP2, p2))
    {
        m_engine->DrawTriangle(p0, p1, p2, pColor, pFilled);
    }
}

bool MR_GeometryRenderer::Project3DTo2D(const MR_Vector3D& pPoint3D, MR_Point& pPoint2D)
{
    // Transform through all matrices
    MR_Vector3D worldPos = m_modelMatrix.Transform(pPoint3D);
    MR_Vector3D viewPos = m_viewMatrix.Transform(worldPos);
    
    // Simple perspective projection
    if (viewPos.z <= 0)
        return false;  // Behind camera

    double ndcX = viewPos.x / viewPos.z;
    double ndcY = viewPos.y / viewPos.z;

    // Convert to screen coordinates (assuming -1 to 1 NDC)
    int screenX = static_cast<int>((ndcX + 1.0) * 0.5 * m_engine->GetDisplayWidth());
    int screenY = static_cast<int>((1.0 - ndcY) * 0.5 * m_engine->GetDisplayHeight());

    pPoint2D = MR_Point(screenX, screenY);
    return true;
}

// ============================================================================
// MR_TextRenderer Implementation
// ============================================================================

MR_TextRenderer::MR_TextRenderer()
    : m_fontSize(8), m_initialized(false)
{
}

MR_TextRenderer::~MR_TextRenderer()
{
    Shutdown();
}

bool MR_TextRenderer::Initialize(MR_RenderingEngine* pEngine)
{
    if (!pEngine || m_initialized)
        return false;

    m_engine = pEngine;
    m_initialized = true;
    return true;
}

void MR_TextRenderer::Shutdown()
{
    m_engine = nullptr;
    m_initialized = false;
}

void MR_TextRenderer::BeginRender()
{
    if (m_engine)
        m_engine->BeginFrame();
}

void MR_TextRenderer::EndRender()
{
    if (m_engine)
        m_engine->EndFrame();
}

void MR_TextRenderer::DrawText(const char* pText, int pX, int pY, const MR_Color& pColor)
{
    if (!pText || !m_engine)
        return;

    int x = pX;
    for (const char* ptr = pText; *ptr != '\0'; ++ptr)
    {
        DrawCharacter(*ptr, x, pY, pColor);
        x += m_fontSize;  // Simple character spacing
    }
}

void MR_TextRenderer::DrawCharacter(char pChar, int pX, int pY, const MR_Color& pColor)
{
    if (!m_engine)
        return;

    // Placeholder: just draw a rectangle for each character
    MR_Rect charRect(pX, pY, m_fontSize, m_fontSize);
    m_engine->DrawRect(charRect, pColor, false);
}

void MR_TextRenderer::SetFontSize(int pSize)
{
    m_fontSize = std::max(1, pSize);
}
