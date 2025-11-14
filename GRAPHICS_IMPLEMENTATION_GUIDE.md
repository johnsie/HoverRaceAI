# Modern Graphics Rendering - Implementation Guide

## Quick Start

### 1. Initialize Rendering

```cpp
#include "RenderingEngine.h"
#include "ModernVideoBuffer.h"

// Create and initialize
MR_ModernVideoBuffer videoBuffer;
if (!videoBuffer.Initialize(640, 480)) {
    // Handle error
    return;
}

// Set palette for 8-bit color
uint8_t palette[768]; // 256 colors * 3 (RGB)
// Load palette data...
videoBuffer.SetPalette(palette, 768);
```

### 2. Render a Frame

```cpp
MR_RenderingEngine* engine = videoBuffer.GetRenderingEngine();

// Begin frame
engine->BeginFrame();

// Clear screen
engine->DrawRect(MR_Rect(0, 0, 640, 480), MR_Color(0, 0, 0), true);

// Draw primitives
engine->DrawPoint(MR_Point(100, 100), MR_Color(255, 255, 255));
engine->DrawLine(MR_Point(0, 0), MR_Point(100, 100), MR_Color(255, 0, 0));
engine->DrawRect(MR_Rect(50, 50, 100, 100), MR_Color(0, 255, 0), false);

// End frame and present
engine->EndFrame();
videoBuffer.Present();
```

### 3. Safe Context Rendering

```cpp
{
    MR_RenderContext context(&videoBuffer);
    context.BeginFrame();
    
    auto buffer = context.GetBuffer();
    if (buffer) {
        // Safe rendering here
    }
    
    context.EndFrame();  // Automatic in destructor
}
```

## Using Specialized Renderers

### Sprite Rendering

```cpp
MR_SpriteRenderer spriteRenderer;
spriteRenderer.Initialize(engine);

// Create sprite buffer
auto spriteBuffer = engine->CreateBuffer(64, 64);
// Populate sprite data...

// Draw single sprite
spriteRenderer.DrawSprite(spriteBuffer, 100, 100, 1.0f, 1.0f, 0.0f);

// Batch rendering (more efficient)
spriteRenderer.BeginBatch();
for (int i = 0; i < 10; ++i) {
    spriteRenderer.AddSpriteToBatch(spriteBuffer, 100 + i*70, 100);
}
spriteRenderer.EndBatch();
```

### Geometry Rendering

```cpp
MR_GeometryRenderer geomRenderer;
geomRenderer.Initialize(engine);

// Setup matrices
MR_Matrix4x4 projection = MR_Matrix4x4::Identity();
MR_Matrix4x4 view = MR_Matrix4x4::Translation(0, 0, -10);
MR_Matrix4x4 model = MR_Matrix4x4::RotationZ(3.14159 / 4);

geomRenderer.SetProjectionMatrix(projection);
geomRenderer.SetViewMatrix(view);
geomRenderer.SetModelMatrix(model);

// Draw 3D geometry
MR_Vector3D p0(0, 0, 0);
MR_Vector3D p1(10, 0, 0);
MR_Vector3D p2(0, 10, 0);

geomRenderer.DrawLine3D(p0, p1, MR_Color(255, 255, 255));
geomRenderer.DrawTriangle3D(p0, p1, p2, MR_Color(100, 100, 255), true);
```

### Actor Rendering

```cpp
MR_ModernActorRenderer actorRenderer;
actorRenderer.Initialize(engine);

// Load actor
if (actorRenderer.LoadActor(ACTOR_HOVERCRAFT, MODEL_ELECTRO)) {
    // Set position/orientation
    actorRenderer.SetActorPosition(ACTOR_HOVERCRAFT, 100, 100, 0);
    actorRenderer.SetActorOrientation(ACTOR_HOVERCRAFT, 1.57, 0, 0);
    
    // Render actor
    actorRenderer.RenderActor(ACTOR_HOVERCRAFT, 320, 240, 0, 5);
}
```

## Memory Management Patterns

### Safe Buffer Allocation

```cpp
// Automatic RAII cleanup
{
    MR_MemorySafety::MR_BufferGuard buffer(1024 * 1024);
    
    if (buffer.IsValid()) {
        uint8_t* data = buffer.Get();
        // Use buffer
        std::memcpy(data, sourceData, buffer.Size());
    }
    
}  // Automatically cleaned up
```

### Safe Pointer Usage

```cpp
MR_RenderingEngine* engine = videoBuffer.GetRenderingEngine();

// Always check before use
if (!engine) {
    return;  // Handle error
}

// Or use helper functions
MR_RenderingUtils::SafeDrawPoint(engine, x, y, color);
MR_RenderingUtils::SafeDrawLine(engine, x0, y0, x1, y1, color);
```

### Bounds Checking

```cpp
int x = 100, y = 200, width = 640, height = 480;

if (MR_MemorySafety::IsBoundsValid(x, y, width, height)) {
    // Safe to draw at (x, y)
}

if (MR_MemorySafety::IsRectBoundsValid(x, y, 100, 100, width, height)) {
    // Safe to draw rect
}
```

## Vector and Matrix Math

### Vector Operations

```cpp
MR_Vector3D position(10, 20, 30);
MR_Vector3D direction(1, 0, 0);

// Vector math
MR_Vector3D result = position + direction;
MR_Vector3D scaled = direction * 5.0;
double distance = position.Distance(MR_Vector3D(0, 0, 0));
double length = direction.Length();

// Normalized direction
MR_Vector3D normalized = direction.Normalized();

// Dot and cross products
double dot = direction.Dot(normalized);
MR_Vector3D cross = direction.Cross(normalized);
```

### Matrix Transformations

```cpp
// Create transformation
MR_Matrix4x4 translation = MR_Matrix4x4::Translation(5, 10, 15);
MR_Matrix4x4 rotation = MR_Matrix4x4::RotationZ(3.14159 / 4);
MR_Matrix4x4 scale = MR_Matrix4x4::Scale(2, 2, 2);

// Combine transformations
MR_Matrix4x4 combined = translation * rotation * scale;

// Transform points
MR_Vector3D point(1, 2, 3);
MR_Vector3D transformed = combined.Transform(point);
```

## Common Patterns

### Drawing Scene

```cpp
class GameScene {
    MR_ModernVideoBuffer m_videoBuffer;
    MR_ModernActorRenderer m_actorRenderer;
    MR_SpriteRenderer m_spriteRenderer;
    MR_TextRenderer m_textRenderer;
    
public:
    bool Initialize(int width, int height) {
        if (!m_videoBuffer.Initialize(width, height)) return false;
        
        MR_RenderingEngine* engine = m_videoBuffer.GetRenderingEngine();
        m_actorRenderer.Initialize(engine);
        m_spriteRenderer.Initialize(engine);
        m_textRenderer.Initialize(engine);
        
        return true;
    }
    
    void Render() {
        MR_RenderContext context(&m_videoBuffer);
        context.BeginFrame();
        
        auto engine = m_videoBuffer.GetRenderingEngine();
        
        // Clear
        engine->DrawRect(MR_Rect(0, 0, 640, 480), MR_Color(0, 0, 0), true);
        
        // Draw background
        m_spriteRenderer.DrawSprite(m_backgroundSprite, 0, 0);
        
        // Draw actors
        m_actorRenderer.RenderActor(ACTOR_PLAYER, 320, 240);
        
        // Draw UI text
        m_textRenderer.DrawText("Score: 1000", 10, 10, MR_Color(255, 255, 255));
        
        context.EndFrame();
    }
    
    void Shutdown() {
        m_actorRenderer.Shutdown();
        m_spriteRenderer.Shutdown();
        m_textRenderer.Shutdown();
        m_videoBuffer.Shutdown();
    }
};
```

### Viewport-Based Rendering

```cpp
// Setup 3D viewport
auto buffer = engine->GetBackBuffer();
MR_ModernViewport viewport(buffer);
viewport.Setup(0, 0, 640, 480);
viewport.SetupProjection(60.0, 0.1, 1000.0);

// Draw within viewport (bounds checked)
viewport.DrawPoint(100, 100, MR_Color(255, 255, 255));
viewport.DrawLine(0, 0, 640, 480, MR_Color(128, 128, 128));
viewport.DrawTriangle(100, 100, 200, 100, 150, 200, MR_Color(255, 0, 0));
```

### Error Handling

```cpp
bool DrawGameFrame(MR_ModernVideoBuffer& videoBuffer, MR_RenderingEngine* engine) {
    // Validate inputs
    if (!engine || !engine->IsInitialized()) {
        return false;
    }
    
    // Render with safety
    {
        MR_RenderContext context(&videoBuffer);
        if (!context.BeginFrame()) {
            return false;
        }
        
        auto buffer = context.GetBuffer();
        if (!buffer) {
            return false;
        }
        
        // Safe rendering
        engine->DrawPoint(MR_Point(100, 100), MR_Color(255, 255, 255));
        
        context.EndFrame();
    }
    
    return true;
}
```

## Optimization Tips

1. **Use Batch Rendering**: Group similar rendering operations
2. **Viewport Culling**: Only render visible content
3. **Level of Detail**: Use different quality levels based on distance
4. **Cache Frequently Used Matrices**: Avoid recalculation
5. **Minimize Buffer Locking**: Lock once, perform multiple operations

## Troubleshooting

### No Graphics Appear
- Check if engine is initialized: `engine->IsInitialized()`
- Verify buffer is created: `engine->GetBackBuffer() != nullptr`
- Ensure Present() is called: `videoBuffer.Present()`

### Memory Corruption
- Use bounds checking: `MR_MemorySafety::IsBoundsValid()`
- Check for null pointers before dereferencing
- Use `MR_BufferGuard` for automatic cleanup

### Performance Issues
- Use batch rendering instead of individual draws
- Implement viewport culling
- Profile with vector math operations
- Consider implementing frustum culling

## Next Steps

1. Integrate `RenderingEngine` with existing systems
2. Migrate `3DViewport` to use modern rendering
3. Update `HoverRender` to use `ModernActorRenderer`
4. Implement advanced features (shaders, lighting)
5. Optimize performance with GPU acceleration

