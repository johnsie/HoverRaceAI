// ModernActorRenderer.cpp
// Implementation of modern actor rendering

#include "stdafx.h"
#include "ModernActorRenderer.h"
#include <algorithm>

// ============================================================================
// MR_ActorResource Implementation
// ============================================================================

MR_ActorResource::MR_ActorResource(int pActorId, int pModelType)
    : m_actorId(pActorId), m_modelType(pModelType), m_valid(true), m_loaded(false)
{
}

MR_ActorResource::~MR_ActorResource()
{
    if (m_loaded)
        Unload();
}

bool MR_ActorResource::Load()
{
    if (!m_valid || m_loaded)
        return false;

    // Placeholder for actual loading logic
    // In a real implementation, this would load actor data from resource library
    m_loaded = true;
    return true;
}

void MR_ActorResource::Unload()
{
    if (!m_loaded)
        return;

    // Placeholder for actual unloading logic
    m_loaded = false;
}

// ============================================================================
// MR_ModernActorRenderer Implementation
// ============================================================================

MR_ModernActorRenderer::MR_ModernActorRenderer()
    : m_initialized(false)
{
}

MR_ModernActorRenderer::~MR_ModernActorRenderer()
{
    Shutdown();
}

bool MR_ModernActorRenderer::Initialize(MR_RenderingEngine* pEngine)
{
    if (!pEngine || m_initialized)
        return false;

    m_engine = pEngine;
    m_actors.clear();
    m_initialized = true;
    return true;
}

void MR_ModernActorRenderer::Shutdown()
{
    m_actors.clear();
    m_engine = nullptr;
    m_initialized = false;
}

void MR_ModernActorRenderer::BeginRender()
{
    if (m_engine)
        m_engine->BeginFrame();
}

void MR_ModernActorRenderer::EndRender()
{
    if (m_engine)
        m_engine->EndFrame();
}

bool MR_ModernActorRenderer::LoadActor(int pActorId, int pModelType)
{
    if (!m_initialized)
        return false;

    // Check if actor already loaded
    if (FindActor(pActorId))
        return true;  // Already loaded

    auto actor = std::make_shared<MR_ActorResource>(pActorId, pModelType);
    if (!actor->Load())
        return false;

    m_actors.push_back(actor);
    return true;
}

bool MR_ModernActorRenderer::UnloadActor(int pActorId)
{
    auto it = std::find_if(m_actors.begin(), m_actors.end(),
        [pActorId](const std::shared_ptr<MR_ActorResource>& pActor) {
            return pActor && pActor->GetActorId() == pActorId;
        });

    if (it != m_actors.end())
    {
        m_actors.erase(it);
        return true;
    }

    return false;
}

bool MR_ModernActorRenderer::HasActor(int pActorId) const
{
    return FindActor(pActorId) != nullptr;
}

void MR_ModernActorRenderer::RenderActor(int pActorId,
                                        int pScreenX, int pScreenY,
                                        int pSequence, int pFrame,
                                        float pScale,
                                        const MR_Color& pTintColor)
{
    if (!m_engine)
        return;

    MR_ActorResource* actor = FindActor(pActorId);
    if (!actor || !actor->IsLoaded())
        return;

    // Placeholder for actual rendering logic
    // Draw a simple placeholder rectangle
    MR_Rect actorRect(pScreenX, pScreenY, 32, 32);
    m_engine->DrawRect(actorRect, pTintColor, true);
}

void MR_ModernActorRenderer::SetActorPosition(int pActorId, double pX, double pY, double pZ)
{
    MR_ActorResource* actor = FindActor(pActorId);
    if (!actor)
        return;

    // Placeholder for position management
}

void MR_ModernActorRenderer::SetActorOrientation(int pActorId, double pYaw, double pPitch, double pRoll)
{
    MR_ActorResource* actor = FindActor(pActorId);
    if (!actor)
        return;

    // Placeholder for orientation management
}

MR_ActorResource* MR_ModernActorRenderer::FindActor(int pActorId)
{
    auto it = std::find_if(m_actors.begin(), m_actors.end(),
        [pActorId](const std::shared_ptr<MR_ActorResource>& pActor) {
            return pActor && pActor->GetActorId() == pActorId;
        });

    return it != m_actors.end() ? it->get() : nullptr;
}

const MR_ActorResource* MR_ModernActorRenderer::FindActor(int pActorId) const
{
    auto it = std::find_if(m_actors.begin(), m_actors.end(),
        [pActorId](const std::shared_ptr<MR_ActorResource>& pActor) {
            return pActor && pActor->GetActorId() == pActorId;
        });

    return it != m_actors.end() ? it->get() : nullptr;
}

// ============================================================================
// MR_ModernSpriteRenderer Implementation
// ============================================================================

MR_ModernSpriteRenderer::MR_ModernSpriteRenderer()
{
}

MR_ModernSpriteRenderer::~MR_ModernSpriteRenderer()
{
}

void MR_ModernSpriteRenderer::DrawAnimatedSprite(const std::shared_ptr<MR_RenderBuffer>& pSpriteSheet,
                                                int pFrameWidth, int pFrameHeight,
                                                int pFrameIndex,
                                                int pX, int pY,
                                                const MR_Color& pTintColor)
{
    if (!m_engine || !pSpriteSheet)
        return;

    // Calculate frame position in sprite sheet
    int framesPerRow = pSpriteSheet->GetWidth() / pFrameWidth;
    int frameRow = (framesPerRow > 0) ? (pFrameIndex / framesPerRow) : 0;
    int frameCol = (framesPerRow > 0) ? (pFrameIndex % framesPerRow) : 0;

    int srcX = frameCol * pFrameWidth;
    int srcY = frameRow * pFrameHeight;

    MR_Rect frameRect(srcX, srcY, pFrameWidth, pFrameHeight);
    m_engine->DrawBuffer(pSpriteSheet, frameRect, MR_Point(pX, pY));
}

void MR_ModernSpriteRenderer::DrawSpriteMirror(const std::shared_ptr<MR_RenderBuffer>& pSprite,
                                              int pX, int pY,
                                              bool pHorizontal, bool pVertical)
{
    if (!m_engine || !pSprite)
        return;

    // Placeholder for mirror rendering
    // In a real implementation, this would flip the sprite horizontally/vertically
    MR_Rect srcRect(0, 0, pSprite->GetWidth(), pSprite->GetHeight());
    m_engine->DrawBuffer(pSprite, srcRect, MR_Point(pX, pY));
}

void MR_ModernSpriteRenderer::DrawSpriteWithTransparency(const std::shared_ptr<MR_RenderBuffer>& pSprite,
                                                        int pX, int pY,
                                                        uint8_t pTransparencyKey)
{
    if (!m_engine || !pSprite)
        return;

    // Placeholder for transparency handling
    // In a real implementation, this would skip pixels matching the transparency key
    MR_Rect srcRect(0, 0, pSprite->GetWidth(), pSprite->GetHeight());
    m_engine->DrawBuffer(pSprite, srcRect, MR_Point(pX, pY));
}
