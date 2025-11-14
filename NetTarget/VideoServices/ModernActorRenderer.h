// ModernActorRenderer.h
// Safe, modern actor rendering with proper resource management

#ifndef MR_MODERN_ACTOR_RENDERER_H
#define MR_MODERN_ACTOR_RENDERER_H

#include "ModernRenderer.h"
#include "../Util/MR_Types.h"
#include <memory>
#include <vector>

#ifdef MR_VIDEO_SERVICES
#define MR_DllDeclare __declspec(dllexport)
#else
#define MR_DllDeclare __declspec(dllimport)
#endif

// ============================================================================
// Actor Resource Management
// Safe wrapper for actor data
// ============================================================================

class MR_DllDeclare MR_ActorResource
{
public:
    MR_ActorResource(int pActorId, int pModelType = 0);
    ~MR_ActorResource();

    int GetActorId() const { return m_actorId; }
    int GetModelType() const { return m_modelType; }
    bool IsValid() const { return m_valid; }
    bool IsLoaded() const { return m_loaded; }

    // Resource lifecycle
    bool Load();
    void Unload();

private:
    int m_actorId;
    int m_modelType;
    bool m_valid;
    bool m_loaded;
};

// ============================================================================
// Modern Actor Renderer
// Manages actor rendering with safe resource management
// ============================================================================

class MR_DllDeclare MR_ModernActorRenderer : public MR_BaseRenderer
{
public:
    MR_ModernActorRenderer();
    ~MR_ModernActorRenderer();

    bool Initialize(MR_RenderingEngine* pEngine) override;
    void Shutdown() override;
    bool IsInitialized() const override { return m_initialized; }

    void BeginRender() override;
    void EndRender() override;

    // Actor management
    bool LoadActor(int pActorId, int pModelType = 0);
    bool UnloadActor(int pActorId);
    bool HasActor(int pActorId) const;

    // Actor rendering
    void RenderActor(int pActorId,
                    int pScreenX, int pScreenY,
                    int pSequence = 0, int pFrame = 0,
                    float pScale = 1.0f,
                    const MR_Color& pTintColor = MR_Color(255, 255, 255));

    // Position and orientation
    void SetActorPosition(int pActorId, double pX, double pY, double pZ);
    void SetActorOrientation(int pActorId, double pYaw, double pPitch, double pRoll);

private:
    std::vector<std::shared_ptr<MR_ActorResource>> m_actors;
    bool m_initialized;

    MR_ActorResource* FindActor(int pActorId);
    const MR_ActorResource* FindActor(int pActorId) const;
};

// ============================================================================
// Modern Sprite Renderer (Specialized for HoverRace sprites)
// ============================================================================

class MR_DllDeclare MR_ModernSpriteRenderer : public MR_SpriteRenderer
{
public:
    MR_ModernSpriteRenderer();
    ~MR_ModernSpriteRenderer();

    // Sprite animation
    void DrawAnimatedSprite(const std::shared_ptr<MR_RenderBuffer>& pSpriteSheet,
                           int pFrameWidth, int pFrameHeight,
                           int pFrameIndex,
                           int pX, int pY,
                           const MR_Color& pTintColor = MR_Color(255, 255, 255));

    // Sprite effects
    void DrawSpriteMirror(const std::shared_ptr<MR_RenderBuffer>& pSprite,
                         int pX, int pY,
                         bool pHorizontal = false, bool pVertical = false);

    void DrawSpriteWithTransparency(const std::shared_ptr<MR_RenderBuffer>& pSprite,
                                   int pX, int pY,
                                   uint8_t pTransparencyKey = 0);

private:
    std::shared_ptr<MR_SpriteRenderer> m_baseRenderer;
};

#undef MR_DllDeclare

#endif  // MR_MODERN_ACTOR_RENDERER_H
