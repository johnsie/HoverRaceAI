// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <winsock2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include <map>
#include <vector>
#include <queue>
#include <string>
#include <memory>
#include <algorithm>

// Pragma to link Winsock2
#pragma comment(lib, "ws2_32.lib")

// Placeholder types for HoverRace compatibility
// These will be replaced with actual HoverRace types during integration
typedef struct {
    float x, y, z;
} MR_Vector3;

typedef struct {
    float x, y, z, w;
} MR_Quaternion;
