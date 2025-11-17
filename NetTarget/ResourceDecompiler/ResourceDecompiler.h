#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>
#include <windows.h>
#include "CArchiveParser.h"

// Define common types used in HoverRace
typedef int BOOL;
#define TRUE 1
#define FALSE 0

// Disable MSVC security warnings for fopen, sprintf, etc.
#pragma warning(disable:4996)

// Resource type constants (mirrors ResourceCompiler definitions)
#define RESOURCE_TYPE_BITMAP    1
#define RESOURCE_TYPE_ACTOR     2
#define RESOURCE_TYPE_SPRITE    3
#define RESOURCE_TYPE_SHORT_SOUND 4
#define RESOURCE_TYPE_CONT_SOUND  5

// Resource entry structure for reading from binary file
struct MR_ResourceEntry
{
    int mType;                      // RESOURCE_TYPE_*
    int mId;                        // Resource ID
    int mDataOffset;                // Offset in file where data starts
    int mDataLength;                // Length of data
    char mName[256];                // Resource name/path
    std::vector<uint8_t> mData;     // Actual binary data payload
};

// Decompiler class - inverse of ResourceLibBuilder
class MR_ResourceDecompiler
{
public:
    MR_ResourceDecompiler();
    ~MR_ResourceDecompiler();

    // Main decompilation function
    BOOL Decompile(const char* pInputFile, const char* pOutputDir);

private:
    // Read and parse binary resource library
    BOOL ReadResourceLibrary(const char* pInputFile, std::vector<MR_ResourceEntry>& pEntries);

    // Extract individual resource types
    BOOL ExtractBitmap(FILE* pSrcFile, const MR_ResourceEntry& pEntry, const char* pOutputDir);
    BOOL ExtractActor(FILE* pSrcFile, const MR_ResourceEntry& pEntry, const char* pOutputDir);
    BOOL ExtractSprite(FILE* pSrcFile, const MR_ResourceEntry& pEntry, const char* pOutputDir);
    BOOL ExtractSound(FILE* pSrcFile, const MR_ResourceEntry& pEntry, const char* pOutputDir);

    // Mesh conversion - binary actor format to .msh text format
    BOOL WriteMeshFile(const char* pFileName, const void* pActorData, int pDataLength);

    // Audio conversion - binary sound format to WAV
    BOOL WriteWavFile(FILE* pOutFile, const void* pSoundData, uint32_t pDataLength, uint32_t pNbCopy);

    // Helper functions
    BOOL CreateOutputDirectory(const char* pPath);
    void SanitizeFileName(char* pName);
};
