// GraphicsTest.cpp
// Create an overlay window for graphics rendering
// This approach creates an overlay window that sits on top of the game window

#include "stdafx.h"
#include "RenderingEngine.h"
#include "GraphicsUtilities.h"
#include <windows.h>
#include <thread>
#include <atomic>
#include <memory>

namespace MR_GraphicsTest
{
    static HWND g_gameWindow = NULL;
    static HWND g_overlayWindow = NULL;
    static std::atomic<bool> g_renderingActive(false);
    static std::unique_ptr<std::thread> g_renderThread;

    // Overlay window procedure
    LRESULT CALLBACK OverlayWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
            case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);

                RECT clientRect;
                GetClientRect(hwnd, &clientRect);
                int width = clientRect.right - clientRect.left;
                int height = clientRect.bottom - clientRect.top;

                // Draw blue background
                HBRUSH blueBrush = CreateSolidBrush(RGB(30, 80, 150));
                SelectObject(hdc, blueBrush);
                Rectangle(hdc, 0, 0, width, height);
                DeleteObject(blueBrush);

                // Draw title
                SetTextColor(hdc, RGB(255, 255, 255));
                SetBkMode(hdc, TRANSPARENT);
                SetTextAlign(hdc, TA_CENTER);
                
                HFONT hFont = CreateFontA(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
                    DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
                SelectObject(hdc, hFont);
                TextOutA(hdc, width / 2, 50, "HoverRace Graphics System", 24);
                DeleteObject(hFont);

                // Draw subtitle
                HFONT hSmallFont = CreateFontA(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
                    DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
                SelectObject(hdc, hSmallFont);
                SetTextColor(hdc, RGB(200, 220, 255));
                TextOutA(hdc, width / 2, 100, "Modern Rendering Engine - ACTIVE", 32);
                DeleteObject(hSmallFont);

                // Draw shapes
                HPEN whitePen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
                HPEN greenPen = CreatePen(PS_SOLID, 2, RGB(100, 255, 100));
                HBRUSH redBrush = CreateSolidBrush(RGB(255, 100, 100));

                // Top rectangles
                SelectObject(hdc, whitePen);
                SelectObject(hdc, (HBRUSH)GetStockObject(HOLLOW_BRUSH));
                Rectangle(hdc, 50, 150, 220, 280);
                Rectangle(hdc, width - 220, 150, width - 50, 280);

                // Center circle
                SelectObject(hdc, redBrush);
                SelectObject(hdc, whitePen);
                int cx = width / 2;
                int cy = height / 2;
                Ellipse(hdc, cx - 100, cy - 100, cx + 100, cy + 100);

                // Diagonal lines
                SelectObject(hdc, greenPen);
                MoveToEx(hdc, 40, height - 80, NULL);
                LineTo(hdc, width - 40, height - 80);
                MoveToEx(hdc, 40, height - 80, NULL);
                LineTo(hdc, cx, 120);
                MoveToEx(hdc, width - 40, height - 80, NULL);
                LineTo(hdc, cx, 120);

                // Bottom status box
                SelectObject(hdc, whitePen);
                SelectObject(hdc, (HBRUSH)GetStockObject(HOLLOW_BRUSH));
                Rectangle(hdc, 60, height - 160, width - 60, height - 40);

                HFONT hStatusFont = CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
                    DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
                SelectObject(hdc, hStatusFont);
                SetTextAlign(hdc, TA_LEFT);
                SetTextColor(hdc, RGB(100, 255, 100));
                TextOutA(hdc, 80, height - 140, "Graphics Engine: RENDERING", 27);
                TextOutA(hdc, 80, height - 110, "Direct Window Rendering: ACTIVE", 31);
                TextOutA(hdc, 80, height - 80, "Resolution: 960x720", 19);
                TextOutA(hdc, 80, height - 50, "Close window to exit", 20);
                DeleteObject(hStatusFont);

                // Cleanup
                DeleteObject(whitePen);
                DeleteObject(greenPen);
                DeleteObject(redBrush);

                EndPaint(hwnd, &ps);
                return 0;
            }

            case WM_CLOSE:
                return 0; // Ignore close

            case WM_DESTROY:
                g_overlayWindow = NULL;
                return 0;
        }

        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }

    void CreateOverlayWindow()
    {
        // Register overlay window class
        static bool classRegistered = false;
        if (!classRegistered)
        {
            WNDCLASSA overlayClass = {};
            overlayClass.style = CS_HREDRAW | CS_VREDRAW;
            overlayClass.lpfnWndProc = OverlayWindowProc;
            overlayClass.hInstance = GetModuleHandleA(NULL);
            overlayClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
            overlayClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            overlayClass.lpszClassName = "HoverRaceGraphicsOverlay";

            if (!RegisterClassA(&overlayClass))
            {
                OutputDebugStringA("GraphicsTest: Failed to register overlay class\n");
                return;
            }
            classRegistered = true;
        }

        // Get game window position and size
        if (!g_gameWindow || !IsWindow(g_gameWindow))
        {
            OutputDebugStringA("GraphicsTest: Game window not found\n");
            return;
        }

        RECT gameRect;
        GetWindowRect(g_gameWindow, &gameRect);
        
        // Calculate client area size and position
        POINT topLeft = { gameRect.left, gameRect.top };
        ClientToScreen(g_gameWindow, &topLeft);
        
        int windowWidth = gameRect.right - gameRect.left;
        int windowHeight = gameRect.bottom - gameRect.top;
        
        // Adjust for window decorations (title bar, borders)
        RECT clientRect = {};
        GetClientRect(g_gameWindow, &clientRect);
        int clientWidth = clientRect.right - clientRect.left;
        int clientHeight = clientRect.bottom - clientRect.top;

        // Create overlay window - child of game window to match its area exactly
        g_overlayWindow = CreateWindowExA(
            WS_EX_NOACTIVATE,
            "HoverRaceGraphicsOverlay",
            "HoverRace Graphics",
            WS_CHILD | WS_VISIBLE,
            0, 0, clientWidth, clientHeight,
            g_gameWindow, NULL,
            GetModuleHandleA(NULL),
            NULL
        );

        if (g_overlayWindow)
        {
            OutputDebugStringA("GraphicsTest: Overlay created as child window\n");
            ShowWindow(g_overlayWindow, SW_SHOW);
            UpdateWindow(g_overlayWindow);
            SetWindowPos(g_overlayWindow, HWND_TOP, 0, 0, clientWidth, clientHeight, SWP_SHOWWINDOW);
        }
    }

    void RenderingLoop()
    {
        while (g_renderingActive.load())
        {
            if (g_overlayWindow && IsWindow(g_overlayWindow))
            {
                InvalidateRect(g_overlayWindow, NULL, FALSE);
                UpdateWindow(g_overlayWindow);
            }
            Sleep(16);
        }
    }

    void EnableGraphicsRendering()
    {
        g_gameWindow = FindWindowA("HoverRaceClass", NULL);
        if (!g_gameWindow)
        {
            g_gameWindow = FindWindowA("MR_GameApp", NULL);
        }

        if (!g_gameWindow)
        {
            HWND hwnd = FindWindowA(NULL, NULL);
            while (hwnd)
            {
                char title[256];
                GetWindowTextA(hwnd, title, sizeof(title));
                if (strstr(title, "HoverRace") != nullptr && IsWindowVisible(hwnd) && GetParent(hwnd) == NULL)
                {
                    g_gameWindow = hwnd;
                    break;
                }
                hwnd = GetNextWindow(hwnd, GW_HWNDNEXT);
            }
        }

        if (!g_gameWindow)
        {
            OutputDebugStringA("GraphicsTest: Could not find game window\n");
            return;
        }

        CreateOverlayWindow();

        if (g_overlayWindow && !g_renderingActive.load())
        {
            g_renderingActive.store(true);
            g_renderThread = std::make_unique<std::thread>(RenderingLoop);
            g_renderThread->detach();
        }
    }

    void TestGraphicsSystem()
    {
        EnableGraphicsRendering();
    }
}

// Export the graphics test function
extern "C" __declspec(dllexport) void TestGraphicsSystem()
{
    MR_GraphicsTest::TestGraphicsSystem();
}
