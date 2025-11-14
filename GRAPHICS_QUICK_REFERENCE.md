# Graphics Rendering - Quick Reference

## Initialization

```cpp
// Create rendering engine
MR_ModernVideoBuffer videoBuffer;
videoBuffer.Initialize(640, 480);
videoBuffer.SetPalette(paletteData, 768);

MR_RenderingEngine* engine = videoBuffer.GetRenderingEngine();
```

## Drawing Primitives

```cpp
// Point
engine->DrawPoint(MR_Point(x, y), MR_Color(r, g, b));

// Line
engine->DrawLine(MR_Point(x0, y0), MR_Point(x1, y1), MR_Color(r, g, b));

// Rectangle (outline)
engine->DrawRect(MR_Rect(x, y, w, h), MR_Color(r, g, b), false);

// Rectangle (filled)
engine->DrawRect(MR_Rect(x, y, w, h), MR_Color(r, g, b), true);

// Triangle
engine->DrawTriangle(MR_Point(x0, y0), MR_Point(x1, y1), MR_Point(x2, y2), 
                    MR_Color(r, g, b), true);
```

## Frame Management

```cpp
// Frame rendering
engine->BeginFrame();
// ... draw operations ...
engine->EndFrame();
videoBuffer.Present();

// With RAII context (recommended)
{
    MR_RenderContext context(&videoBuffer);
    context.BeginFrame();
    // ... draw operations ...
    context.EndFrame();
}
```

## Sprite Rendering

```cpp
MR_SpriteRenderer spriteRenderer;
spriteRenderer.Initialize(engine);

// Single sprite
spriteRenderer.DrawSprite(spriteBuffer, x, y, 1.0f, 1.0f, 0.0f);

// Batch rendering (faster)
spriteRenderer.BeginBatch();
for (int i = 0; i < count; ++i) {
    spriteRenderer.AddSpriteToBatch(spriteBuffer, x + i*w, y);
}
spriteRenderer.EndBatch();
```

## Actor Rendering

```cpp
MR_ModernActorRenderer actorRenderer;
actorRenderer.Initialize(engine);
actorRenderer.LoadActor(actorId, modelType);
actorRenderer.RenderActor(actorId, screenX, screenY, sequence, frame);
```

## 3D Geometry

```cpp
MR_GeometryRenderer geomRenderer;
geomRenderer.Initialize(engine);

// Set matrices
geomRenderer.SetProjectionMatrix(projection);
geomRenderer.SetViewMatrix(view);
geomRenderer.SetModelMatrix(model);

// Draw 3D geometry
geomRenderer.DrawLine3D(MR_Vector3D(0, 0, 0), MR_Vector3D(10, 0, 0), color);
geomRenderer.DrawTriangle3D(p0, p1, p2, color, true);
```

## Math Operations

```cpp
// Vectors
MR_Vector3D v1(10, 20, 30);
MR_Vector3D v2(5, 5, 5);
double distance = v1.Distance(v2);
MR_Vector3D sum = v1 + v2;
MR_Vector3D scaled = v1 * 2.0;
MR_Vector3D normalized = v1.Normalized();
double dot = v1.Dot(v2);
MR_Vector3D cross = v1.Cross(v2);

// Matrices
MR_Matrix4x4 translation = MR_Matrix4x4::Translation(x, y, z);
MR_Matrix4x4 rotation = MR_Matrix4x4::RotationZ(angle);
MR_Matrix4x4 scale = MR_Matrix4x4::Scale(sx, sy, sz);
MR_Matrix4x4 combined = translation * rotation * scale;
MR_Vector3D transformed = combined.Transform(point);
```

## Memory Safety

```cpp
// Safe buffer allocation
{
    MR_MemorySafety::MR_BufferGuard buffer(size);
    if (buffer.IsValid()) {
        uint8_t* data = buffer.Get();
        // Use data - auto cleanup
    }
}

// Bounds checking
if (MR_MemorySafety::IsBoundsValid(x, y, width, height)) {
    // Safe to draw at (x, y)
}

// Safe drawing
MR_RenderingUtils::SafeDrawPoint(engine, x, y, color);
MR_RenderingUtils::SafeDrawLine(engine, x0, y0, x1, y1, color);
```

## Color Management

```cpp
// RGB colors
MR_Color white(255, 255, 255);
MR_Color red(255, 0, 0);
MR_Color transparent(0, 0, 0, 0);

// Conversions
uint32_t rgb32 = color.ToRGB32();
uint32_t argb32 = color.ToARGB32();

// Palette
uint8_t palette[768];  // 256 colors * 3 (RGB)
engine->SetPalette(palette, 768);
```

## Viewport Operations

```cpp
MR_ModernViewport viewport(buffer);
viewport.Setup(x, y, width, height);
viewport.SetupProjection(fov, near, far);

viewport.DrawPoint(x, y, color);
viewport.DrawLine(x0, y0, x1, y1, color);
viewport.DrawTriangle(x0, y0, x1, y1, x2, y2, color);
```

## Text Rendering

```cpp
MR_TextRenderer textRenderer;
textRenderer.Initialize(engine);
textRenderer.SetFontSize(16);
textRenderer.DrawText("Hello World", x, y, color);
```

## Common Patterns

### Complete Render Loop
```cpp
class Game {
    MR_ModernVideoBuffer m_buffer;
    
public:
    bool Initialize() {
        return m_buffer.Initialize(640, 480);
    }
    
    void Update() { /* game logic */ }
    
    void Render() {
        MR_RenderContext ctx(&m_buffer);
        ctx.BeginFrame();
        
        auto engine = m_buffer.GetRenderingEngine();
        engine->DrawRect(MR_Rect(0, 0, 640, 480), MR_Color(0, 0, 0), true);
        // ... draw game content ...
        
        ctx.EndFrame();
    }
    
    void Shutdown() {
        m_buffer.Shutdown();
    }
};
```

### Safe Actor Rendering
```cpp
void RenderActors(MR_ModernActorRenderer* renderer, int count) {
    if (!renderer) return;
    
    for (int i = 0; i < count; ++i) {
        if (renderer->HasActor(i)) {
            renderer->RenderActor(i, 100 + i*50, 100);
        }
    }
}
```

### Batch Sprite Rendering
```cpp
void RenderSprites(MR_SpriteRenderer* renderer, 
                  const std::vector<SpriteData>& sprites) {
    if (!renderer) return;
    
    renderer->BeginBatch();
    for (const auto& sprite : sprites) {
        renderer->AddSpriteToBatch(sprite.buffer, sprite.x, sprite.y);
    }
    renderer->EndBatch();
}
```

## Error Handling

```cpp
// Check initialization
if (!engine->IsInitialized()) {
    return false;
}

// Check null pointers
if (!engine || !buffer) {
    return;
}

// Validate dimensions
if (width <= 0 || height <= 0) {
    return false;
}

// Check bounds
if (!MR_MemorySafety::IsBoundsValid(x, y, maxW, maxH)) {
    return;
}
```

## Performance Tips

1. **Use batch rendering**: Combine multiple draws
2. **Implement viewport culling**: Skip off-screen content
3. **Cache matrices**: Don't recalculate frequently
4. **Minimize lock/unlock**: Batch buffer access
5. **Use appropriate primitive types**: Quads instead of triangles when possible

## Debugging

```cpp
// Enable safe mode (if available)
#ifdef _DEBUG
    engine->EnableSafeMode();
#endif

// Log rendering operations
engine->SetDebugLogging(true);

// Verify buffer contents
auto buffer = engine->GetBackBuffer();
auto data = buffer->LockBuffer();
// Examine pixel data...
buffer->UnlockBuffer();
```

## Resource Cleanup

```cpp
// Automatic cleanup with RAII
{
    MR_ModernVideoBuffer buffer;
    buffer.Initialize(640, 480);
    // ... use buffer ...
}  // Automatically cleaned up

// Manual cleanup if needed
buffer.Shutdown();
```

## Reference

### Color
- `MR_Color(r, g, b)`: RGB color (0-255)
- `MR_Color(r, g, b, a)`: RGBA color with alpha

### Geometry
- `MR_Point(x, y)`: 2D point
- `MR_Rect(x, y, w, h)`: 2D rectangle
- `MR_Vector2D(x, y)`: 2D vector with math
- `MR_Vector3D(x, y, z)`: 3D vector with math

### Matrix
- `MR_Matrix4x4::Identity()`: Identity matrix
- `MR_Matrix4x4::Translation(x, y, z)`: Translation
- `MR_Matrix4x4::RotationX/Y/Z(angle)`: Rotation
- `MR_Matrix4x4::Scale(x, y, z)`: Scale

### Renderers
- `MR_SpriteRenderer`: 2D sprite rendering
- `MR_GeometryRenderer`: 3D geometry rendering
- `MR_TextRenderer`: Text rendering
- `MR_ModernActorRenderer`: Actor management

### Utilities
- `MR_MemorySafety::MR_BufferGuard`: RAII buffer
- `MR_RenderingUtils::SafeDrawPoint`: Safe point drawing
- `MR_RenderingUtils::SafeDrawLine`: Safe line drawing

---

For complete documentation, see:
- `MODERN_GRAPHICS_ARCHITECTURE.md` - System design
- `GRAPHICS_IMPLEMENTATION_GUIDE.md` - Detailed examples
- Header files - API documentation
