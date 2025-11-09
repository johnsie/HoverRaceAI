// VideoBufferSDL2Integration.cpp
// Integration implementation for SDL2Graphics backend with VideoBuffer

#include "stdafx.h"
#include "VideoBufferSDL2Integration.h"
#include <fstream>

// Global SDL2 adapter instance
VideoBufferSDL2Adapter* g_SDL2GraphicsAdapter = NULL;

// Helper function to initialize SDL2 graphics
BOOL InitializeSDL2Graphics(HWND pWindow, int pXRes, int pYRes)
{
   // Log to file for debugging
   std::ofstream log("C:\\originalhr\\HoverRace\\Release\\sdl2_integration_debug.log", std::ios::app);
   log << "=== InitializeSDL2Graphics called ===" << std::endl;
   log << "Window: " << pWindow << ", Res: " << pXRes << "x" << pYRes << std::endl;
   log.flush();

   // Clean up any existing adapter
   if (g_SDL2GraphicsAdapter != NULL)
   {
      log << "Cleaning up existing adapter" << std::endl;
      log.flush();
      delete g_SDL2GraphicsAdapter;
      g_SDL2GraphicsAdapter = NULL;
   }

   try
   {
      // Create new adapter instance
      log << "Creating new VideoBufferSDL2Adapter instance..." << std::endl;
      log.flush();
      g_SDL2GraphicsAdapter = new VideoBufferSDL2Adapter();

      // Initialize with window and resolution
      log << "Calling g_SDL2GraphicsAdapter->Initialize..." << std::endl;
      log.flush();
      if (g_SDL2GraphicsAdapter->Initialize(pWindow, pXRes, pYRes))
      {
         log << "SDL2Graphics initialized successfully" << std::endl;
         log.close();
         OutputDebugStringW(L"SDL2Graphics initialized successfully\n");
         return TRUE;
      }
      else
      {
         log << "ERROR: adapter->Initialize returned false" << std::endl;
         log.close();
         OutputDebugStringW(L"SDL2Graphics initialization failed\n");
         delete g_SDL2GraphicsAdapter;
         g_SDL2GraphicsAdapter = NULL;
         return FALSE;
      }
   }
   catch (const std::exception& e)
   {
      log << "Exception: " << e.what() << std::endl;
      log.close();
      OutputDebugStringW(L"SDL2Graphics initialization exception\n");
      if (g_SDL2GraphicsAdapter != NULL)
      {
         delete g_SDL2GraphicsAdapter;
         g_SDL2GraphicsAdapter = NULL;
      }
      return FALSE;
   }
   catch (...)
   {
      log << "Unknown exception" << std::endl;
      log.close();
      OutputDebugStringW(L"SDL2Graphics initialization unknown exception\n");
      if (g_SDL2GraphicsAdapter != NULL)
      {
         delete g_SDL2GraphicsAdapter;
         g_SDL2GraphicsAdapter = NULL;
      }
      return FALSE;
   }
}

// Helper function to shutdown SDL2 graphics
void ShutdownSDL2Graphics()
{
   if (g_SDL2GraphicsAdapter != NULL)
   {
      g_SDL2GraphicsAdapter->Shutdown();
      delete g_SDL2GraphicsAdapter;
      g_SDL2GraphicsAdapter = NULL;
   }
}

// Check if SDL2 graphics is available
BOOL IsSDL2GraphicsAvailable()
{
   return (g_SDL2GraphicsAdapter != NULL);
}
