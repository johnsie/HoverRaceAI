// VideoBufferSDL2Integration.h
// Integration layer for SDL2Graphics backend with VideoBuffer
//
// This header provides the SDL2Graphics backend integration for VideoBuffer
// allowing seamless replacement of DirectDraw with modern SDL2+OpenGL rendering

#ifndef VIDEO_BUFFER_SDL2_INTEGRATION_H
#define VIDEO_BUFFER_SDL2_INTEGRATION_H

#include "../GraphicsSDL2/VideoBufferSDL2Adapter.h"

// Forward declaration
class MR_VideoBuffer;

// Global SDL2 adapter instance - will be initialized during SetVideoMode()
extern VideoBufferSDL2Adapter* g_SDL2GraphicsAdapter;

// Helper functions for VideoBuffer integration
BOOL InitializeSDL2Graphics(HWND pWindow, int pXRes, int pYRes);
void ShutdownSDL2Graphics();
BOOL IsSDL2GraphicsAvailable();

#endif // VIDEO_BUFFER_SDL2_INTEGRATION_H
