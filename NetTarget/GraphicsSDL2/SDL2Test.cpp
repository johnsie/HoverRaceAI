#include <stdio.h>
#include <windows.h>

// Forward declarations from VideoBufferSDL2Integration.cpp
extern "C" {
    int InitializeSDL2Graphics(HWND pWindow, int pXRes, int pYRes);
    void ShutdownSDL2Graphics();
    int IsSDL2GraphicsAvailable();
}

int main()
{
    printf("HoverRace SDL2 Graphics Test\n");
    printf("==============================\n\n");

    printf("Creating test window...\n");
    HWND hWnd = CreateWindowExA(
        0,
        "STATIC",
        "HoverRace SDL2 Test",
        WS_OVERLAPPEDWINDOW,
        100, 100, 800, 600,
        NULL, NULL, GetModuleHandleA(NULL), NULL
    );

    if (!hWnd) {
        printf("ERROR: Failed to create window!\n");
        return 1;
    }

    printf("Window created: 0x%p\n", hWnd);
    printf("Attempting SDL2Graphics initialization (800x600)...\n");

    int result = InitializeSDL2Graphics(hWnd, 800, 600);

    if (result) {
        printf("SUCCESS: SDL2Graphics initialized!\n");
        printf("SDL2 Graphics is available: %d\n", IsSDL2GraphicsAvailable());
        
        printf("\nShutting down SDL2Graphics...\n");
        ShutdownSDL2Graphics();
        printf("Done!\n");
    }
    else {
        printf("ERROR: SDL2Graphics initialization failed!\n");
    }

    if (hWnd) {
        DestroyWindow(hWnd);
    }

    return result ? 0 : 1;
}
