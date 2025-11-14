// ModernRenderer.h
// Modern rendering base classes with safety and clean architecture

#ifndef MR_MODERN_RENDERER_H
#define MR_MODERN_RENDERER_H

#include "RenderingEngine.h"
#include "GraphicsUtilities.h"
#include "../Util/MR_Types.h"
#include <memory>
#include <vector>

#ifdef MR_VIDEO_SERVICES
#define MR_DllDeclare __declspec(dllexport)
#else
#define MR_DllDeclare __declspec(dllimport)
#endif

// ============================================================================
// Base Renderer Class
// Safe, modern interface for all rendering operations
// ============================================================================

class MR_DllDeclare MR_BaseRenderer
{
public:
    virtual ~MR_BaseRenderer() = default;

    // Initialize/shutdown renderer
    virtual bool Initialize(MR_RenderingEngine* pEngine) = 0;
    virtual void Shutdown() = 0;
    virtual bool IsInitialized() const = 0;

    // Rendering state
    virtual void BeginRender() = 0;
    virtual void EndRender() = 0;

    // Get properties
    MR_RenderingEngine* GetEngine() { return m_engine; }
    const MR_RenderingEngine* GetEngine() const { return m_engine; }

protected:
    MR_BaseRenderer() : m_engine(nullptr) {}

    // Helper methods for safe rendering
    void SafeDrawPoint(int pX, int pY, const MR_Color& pColor);
    void SafeDrawLine(int pX0, int pY0, int pX1, int pY1, const MR_Color& pColor);
    void SafeDrawRect(const MR_Rect& pRect, const MR_Color& pColor, bool pFilled = false);

    MR_RenderingEngine* m_engine;
};

// ============================================================================
// 2D Sprite Renderer
// Handles sprites and bitmaps with proper bounds checking
// ============================================================================

class MR_DllDeclare MR_SpriteRenderer : public MR_BaseRenderer
{
public:
    MR_SpriteRenderer();
    ~MR_SpriteRenderer();

    bool Initialize(MR_RenderingEngine* pEngine) override;
    void Shutdown() override;
    bool IsInitialized() const override { return m_initialized; }

    void BeginRender() override;
    void EndRender() override;

    // Sprite rendering with safe bounds checking
    void DrawSprite(const std::shared_ptr<MR_RenderBuffer>& pSprite,
                   int pX, int pY,
                   float pScaleX = 1.0f, float pScaleY = 1.0f,
                   float pRotation = 0.0f,
                   uint8_t pAlpha = 255);

    // Batch rendering for efficiency
    void BeginBatch();
    void EndBatch();
    void AddSpriteToBatch(const std::shared_ptr<MR_RenderBuffer>& pSprite,
                         int pX, int pY);

private:
    struct SpriteBatchItem
    {
        std::shared_ptr<MR_RenderBuffer> sprite;
        int x, y;
    };

    bool m_initialized;
    std::vector<SpriteBatchItem> m_batch;
    bool m_batchActive;
};

// ============================================================================
// 3D Geometry Renderer
// Handles 3D to 2D projection with rasterization
// ============================================================================

class MR_DllDeclare MR_GeometryRenderer : public MR_BaseRenderer
{
public:
    MR_GeometryRenderer();
    ~MR_GeometryRenderer();

    bool Initialize(MR_RenderingEngine* pEngine) override;
    void Shutdown() override;
    bool IsInitialized() const override { return m_initialized; }

    void BeginRender() override;
    void EndRender() override;

    // Projection setup
    void SetProjectionMatrix(const MR_Matrix4x4& pMatrix);
    void SetViewMatrix(const MR_Matrix4x4& pMatrix);
    void SetModelMatrix(const MR_Matrix4x4& pMatrix);

    // 3D rendering
    void DrawLine3D(const MR_Vector3D& pStart, const MR_Vector3D& pEnd,
                   const MR_Color& pColor);
    void DrawTriangle3D(const MR_Vector3D& pP0, const MR_Vector3D& pP1, const MR_Vector3D& pP2,
                       const MR_Color& pColor, bool pFilled = true);

    // Get transformation matrices
    const MR_Matrix4x4& GetProjectionMatrix() const { return m_projectionMatrix; }
    const MR_Matrix4x4& GetViewMatrix() const { return m_viewMatrix; }
    const MR_Matrix4x4& GetModelMatrix() const { return m_modelMatrix; }

private:
    MR_Matrix4x4 m_projectionMatrix;
    MR_Matrix4x4 m_viewMatrix;
    MR_Matrix4x4 m_modelMatrix;
    bool m_initialized;

    // Helper: Project 3D point to 2D
    bool Project3DTo2D(const MR_Vector3D& pPoint3D, MR_Point& pPoint2D);
};

// ============================================================================
// Text Renderer
// Safe text rendering with font management
// ============================================================================

class MR_DllDeclare MR_TextRenderer : public MR_BaseRenderer
{
public:
    MR_TextRenderer();
    ~MR_TextRenderer();

    bool Initialize(MR_RenderingEngine* pEngine) override;
    void Shutdown() override;
    bool IsInitialized() const override { return m_initialized; }

    void BeginRender() override;
    void EndRender() override;

    // Text rendering
    void DrawText(const char* pText, int pX, int pY, const MR_Color& pColor);
    void DrawCharacter(char pChar, int pX, int pY, const MR_Color& pColor);

    // Font properties
    void SetFontSize(int pSize);
    int GetFontSize() const { return m_fontSize; }

private:
    int m_fontSize;
    bool m_initialized;
};

#undef MR_DllDeclare

#endif  // MR_MODERN_RENDERER_H
