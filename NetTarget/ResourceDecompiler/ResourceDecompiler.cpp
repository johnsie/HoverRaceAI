#include "ResourceDecompiler.h"
#include <direct.h>
#include <stdio.h>
#include <string.h>

MR_ResourceDecompiler::MR_ResourceDecompiler()
{
}

MR_ResourceDecompiler::~MR_ResourceDecompiler()
{
}

BOOL MR_ResourceDecompiler::Decompile(const char* pInputFile, const char* pOutputDir)
{
    if (!pInputFile || !pOutputDir)
        return FALSE;

    // Create output directory
    if (!CreateOutputDirectory(pOutputDir))
    {
        printf("ERROR: Could not create output directory: %s\n", pOutputDir);
        return FALSE;
    }

    printf("\n=== HoverRace Resource Decompiler ===\n");
    printf("Version 1.0\n\n");
    printf("Input file: %s\n", pInputFile);
    printf("Output directory: %s\n\n", pOutputDir);

    // Read all resources from binary library
    std::vector<MR_ResourceEntry> lEntries;
    if (!ReadResourceLibrary(pInputFile, lEntries))
    {
        printf("ERROR: Could not read resource library: %s\n", pInputFile);
        return FALSE;
    }

    if (lEntries.empty())
    {
        printf("WARNING: No resources found in library\n");
        return FALSE;
    }

    printf("Found %d resources\n\n", (int)lEntries.size());

    // Count resources by type
    printf("=== Extracting Resources ===\n");
    int bitmapCount = 0, actorCount = 0, spriteCount = 0, soundCount = 0;
    int extractedBitmaps = 0;
    int extractedActors = 0;
    
    for (const auto& entry : lEntries)
    {
        switch (entry.mType)
        {
        case RESOURCE_TYPE_BITMAP:
        {
            bitmapCount++;
            // Try to extract bitmap
            FILE* srcFile = fopen(pInputFile, "rb");
            if (srcFile)
            {
                if (ExtractBitmap(srcFile, entry, pOutputDir))
                    extractedBitmaps++;
                fclose(srcFile);
            }
            break;
        }
        case RESOURCE_TYPE_ACTOR: 
        {
            actorCount++;
            // Try to extract actor
            FILE* srcFile = fopen(pInputFile, "rb");
            if (srcFile)
            {
                if (ExtractActor(srcFile, entry, pOutputDir))
                    extractedActors++;
                fclose(srcFile);
            }
            break;
        }
        case RESOURCE_TYPE_SPRITE: 
            spriteCount++; 
            break;
        case RESOURCE_TYPE_SHORT_SOUND:
        case RESOURCE_TYPE_CONT_SOUND: 
            soundCount++; 
            break;
        }
    }
    
    printf("\n=== Resource Summary ===\n");
    printf("Bitmaps: %d (exported: %d)\n", bitmapCount, extractedBitmaps);
    printf("Actors: %d (exported: %d)\n", actorCount, extractedActors);
    printf("Sprites: %d\n", spriteCount);
    printf("Sounds: %d\n\n", soundCount);
    
    printf("Binary format successfully analyzed!\n");
    printf("The file uses MFC CArchive serialization wrapped in RecordFile format.\n");
    printf("Format signature: Magic number 12345 at offset detected successfully.\n");
    printf("\nExport complete! Resources saved to: %s\n", pOutputDir);
    
    return TRUE;
}

BOOL MR_ResourceDecompiler::ReadResourceLibrary(const char* pInputFile, std::vector<MR_ResourceEntry>& pEntries)
{

    
    FILE* lFile = fopen(pInputFile, "rb");
    if (!lFile)
    {
        printf("ERROR: Failed to open file\n");
        return FALSE;
    }
    


    // Check for RecordFile format (contains title string)
    char lHeader[256];
    if (fread(lHeader, 1, sizeof(lHeader), lFile) != sizeof(lHeader))
    {
        fclose(lFile);
        return FALSE;
    }

    // Check if this is a RecordFile format (starts with title)
    BOOL lIsRecordFile = FALSE;
    if (strstr(lHeader, "Fireball") || strstr(lHeader, "object factory") || strstr(lHeader, "GrokkSoft"))
    {
        lIsRecordFile = TRUE;
        printf("Detected MFC RecordFile format (title in header)\n");
        printf("Parsing ResourceLib data stream...\n");
    }

    if (!lIsRecordFile)
    {
        // Try simple magic number format
        rewind(lFile);
        int lMagic;
        if (fread(&lMagic, sizeof(int), 1, lFile) != 1)
        {
            fclose(lFile);
            return FALSE;
        }

        if (lMagic != 12345)
        {
            printf("ERROR: File does not appear to be a valid resource library\n");
            printf("  Expected magic number 12345, got: %d\n", lMagic);
            fclose(lFile);
            return FALSE;
        }
    }
    else
    {
        // For RecordFile format, search for the magic number
        rewind(lFile);
        unsigned char buffer[4];
        int32_t magic;
        long lMagicOffset = -1;
        

        
        // Search for magic number 12345 (0x00003039 in little-endian)
        int searchCount = 0;
        while (fread(buffer, 1, 4, lFile) == 4)
        {
            magic = *(int32_t*)buffer;
            if (magic == 12345)
            {
                lMagicOffset = ftell(lFile) - 4;

                break;
            }
            fseek(lFile, -3, SEEK_CUR);
            
            searchCount++;
        }
        
        if (lMagicOffset == -1)
        {
            printf("ERROR: Could not find CArchive magic number (12345) in RecordFile\n");

            fclose(lFile);
            return FALSE;
        }
        
        printf("Found CArchive magic number at offset 0x%lX\n", lMagicOffset);
        
        // Close and reopen to use CArchive from the magic number position
        fclose(lFile);
        lFile = fopen(pInputFile, "rb");
        if (!lFile)
            return FALSE;
        fseek(lFile, lMagicOffset, SEEK_SET);
    }

    // Now parse the CArchive data using SimpleCArchive
    // Start from the magic number position

    SimpleCArchive lArchive(lFile, true);
    
    // Verify magic number
    int32_t lMagic;
    lArchive >> lMagic;

    
    if (!lArchive.IsGood() || lMagic != 12345)
    {
        printf("ERROR: Invalid magic number: %d (expected 12345)\n", lMagic);

        fclose(lFile);
        return FALSE;
    }

    // Read bitmaps - skip past the actual bitmap data to find the next resource type
    int32_t lNbBitmap;
    lArchive >> lNbBitmap;

    if (!lArchive.IsGood())
    {
        printf("ERROR: Could not read bitmap count\n");
        fclose(lFile);
        return FALSE;
    }

    printf("Found %d bitmaps\n", lNbBitmap);

    // Safety check: if lNbBitmap is unreasonable, it's likely we're at the wrong offset
    if (lNbBitmap < 0 || lNbBitmap > 1000)
    {
        printf("ERROR: Invalid bitmap count: %d (expected 0-1000)\n", lNbBitmap);
        printf("ERROR: Archive status: good=%d, position=%ld\n", lArchive.IsGood() ? 1 : 0, lArchive.GetPosition());
        fclose(lFile);
        return FALSE;
    }

    for (int i = 0; i < lNbBitmap; i++)
    {
        int32_t lBitmapId;
        lArchive >> lBitmapId;
        
        if (!lArchive.IsGood())
            break;

        long lDataStart = lArchive.GetPosition();
        
        // Read bitmap structure fields to skip them
        int32_t w, h, xr, yr, cnt;
        uint8_t color;
        
        lArchive >> w >> h >> xr >> yr >> cnt >> color;
        if (!lArchive.IsGood())
        {
            printf("  ERROR: Cannot read bitmap header for ID %d\n", lBitmapId);
            break;
        }
        
        // For each subitmap, read the header and skip the pixel buffer
        for (int j = 0; j < cnt && lArchive.IsGood(); j++)
        {
            int32_t sr_x, sr_y, sr_shift_x, sr_shift_y;
            int32_t sr_trans;  // BOOL is 4 bytes
            
            lArchive >> sr_x >> sr_y >> sr_shift_x >> sr_shift_y >> sr_trans;
            
            if (!lArchive.IsGood())
            {
                printf("  ERROR: Cannot read subitmap %d header\n", j);
                break;
            }
            
            // Calculate buffer size and skip it
            int bufSize = sr_x * sr_y;
            
            if (bufSize < 0 || bufSize > 50000000)  // 50MB sanity check
            {
                printf("  ERROR: Invalid subitmap buffer size: %d at j=%d\n", bufSize, j);
                break;
            }
            
            // Skip the pixel buffer
            std::vector<uint8_t> tempBuf(bufSize);
            size_t bytesRead = lArchive.Read(tempBuf.data(), bufSize);
            
            if (bytesRead != (size_t)bufSize)
            {
                printf("  ERROR: Could only read %zu of %d bytes for subitmap buffer\n", bytesRead, bufSize);
                break;
            }
        }

        long lDataEnd = lArchive.GetPosition();
        int lDataLength = (int)(lDataEnd - lDataStart);

        MR_ResourceEntry lEntry;
        lEntry.mType = RESOURCE_TYPE_BITMAP;
        lEntry.mId = lBitmapId;
        lEntry.mDataOffset = lDataStart;
        lEntry.mDataLength = lDataLength;
        sprintf_s(lEntry.mName, sizeof(lEntry.mName), "bitmap_%d", lBitmapId);
        
        // Capture the actual binary data
        if (lDataLength > 0)
        {
            lEntry.mData.resize(lDataLength);
            long lCurrentPos = lArchive.GetPosition();
            lArchive.SetPosition(lDataStart);
            size_t lBytesRead = lArchive.Read(lEntry.mData.data(), lDataLength);
            lArchive.SetPosition(lCurrentPos);
        }
        
        pEntries.push_back(lEntry);

        printf("  Bitmap %d: %d bytes (w=%d h=%d xr=%d yr=%d cnt=%d)\n", lBitmapId, lDataLength, w, h, xr, yr, cnt);
    }

    
    printf("Successfully parsed %d bitmaps\n", (int)pEntries.size());
    
    // Parse actors
    int lNbActor;
    lArchive >> lNbActor;
    printf("Found %d actors\n", lNbActor);
    
    for (int i = 0; i < lNbActor; i++)
    {
        int32_t lActorId;
        lArchive >> lActorId;
        
        if (!lArchive.IsGood())
            break;

        long lDataStart = lArchive.GetPosition();
        
        // Parse actor structure (sequences -> frames -> patches)
        int32_t nbSequence;
        lArchive >> nbSequence;
        
        for (int s = 0; s < nbSequence && lArchive.IsGood(); s++)
        {
            int32_t nbFrame;
            lArchive >> nbFrame;
            
            for (int f = 0; f < nbFrame && lArchive.IsGood(); f++)
            {
                int32_t nbComponent;
                lArchive >> nbComponent;
                
                for (int c = 0; c < nbComponent && lArchive.IsGood(); c++)
                {
                    int32_t componentType;
                    lArchive >> componentType;  // 0 = ePatch
                    
                    if (componentType == 0)  // ePatch
                    {
                        int32_t uRes, vRes, bitmapId;
                        lArchive >> uRes >> vRes >> bitmapId;
                        
                        // Read vertices
                        for (int v = 0; v < uRes * vRes && lArchive.IsGood(); v++)
                        {
                            int32_t x, y, z;
                            lArchive >> x >> y >> z;
                        }
                    }
                }
            }
        }
        
        long lDataEnd = lArchive.GetPosition();
        int lDataLength = (int)(lDataEnd - lDataStart);

        MR_ResourceEntry lEntry;
        lEntry.mType = RESOURCE_TYPE_ACTOR;
        lEntry.mId = lActorId;
        lEntry.mDataOffset = lDataStart;
        lEntry.mDataLength = lDataLength;
        sprintf_s(lEntry.mName, sizeof(lEntry.mName), "actor_%d", lActorId);
        
        // Capture the actual binary data
        if (lDataLength > 0)
        {
            lEntry.mData.resize(lDataLength);
            long lCurrentPos = lArchive.GetPosition();
            lArchive.SetPosition(lDataStart);
            size_t lBytesRead = lArchive.Read(lEntry.mData.data(), lDataLength);
            lArchive.SetPosition(lCurrentPos);
        }
        
        pEntries.push_back(lEntry);
        printf("  Actor %d: %d bytes (sequences=%d)\n", lActorId, lDataLength, nbSequence);
    }
    
    printf("Successfully parsed %d actors\n", lNbActor);
    
    // Skip sprite/sound parsing for now
    
    fclose(lFile);
    return (int)pEntries.size() > 0;
}

BOOL MR_ResourceDecompiler::ExtractBitmap(FILE* pSrcFile, const MR_ResourceEntry& pEntry, const char* pOutputDir)
{
    if (pEntry.mData.empty())
    {
        printf("  WARNING: No bitmap data available\n");
        return FALSE;
    }

    // Create a temp file from the binary data
    char lTempPath[512];
    sprintf_s(lTempPath, sizeof(lTempPath), "%s\\bitmap_%d.tmp", pOutputDir, pEntry.mId);
    
    FILE* lTempFile = fopen(lTempPath, "wb");
    if (!lTempFile)
        return FALSE;
    
    fwrite(pEntry.mData.data(), 1, pEntry.mData.size(), lTempFile);
    fclose(lTempFile);

    // Parse binary bitmap data
    BOOL lResult = FALSE;
    lTempFile = fopen(lTempPath, "rb");
    if (lTempFile)
    {
        try
        {
            SimpleCArchive lArchive(lTempFile, true);
            Bitmap lBitmap;
            lBitmap.Serialize(lArchive);

            if (lArchive.IsGood() && !lBitmap.mSubBitmaps.empty())
            {
                // Export to .tga format
                char lOutputPath[512];
                sprintf_s(lOutputPath, sizeof(lOutputPath), "%s\\bitmap_%d.tga", pOutputDir, pEntry.mId);
                
                FILE* lOutFile = fopen(lOutputPath, "wb");
                if (lOutFile)
                {
                    lBitmap.ExportToTGA(lOutFile);
                    fclose(lOutFile);
                    printf("  -> %s (%ux%u pixels)\n", lOutputPath, 
                        lBitmap.mSubBitmaps[0].mXRes, lBitmap.mSubBitmaps[0].mYRes);
                    lResult = TRUE;
                }
            }
            else
            {
                printf("  ERROR: Failed to parse bitmap data (archive %s)\n", 
                    lArchive.IsGood() ? "good" : "bad");
            }
        }
        catch (...)
        {
            printf("  ERROR: Exception during bitmap parsing\n");
        }

        fclose(lTempFile);
    }
    
    // Clean up temp file
    remove(lTempPath);
    
    return lResult;
}

// Helper function to map actor IDs to readable names
static const char* GetActorName(int pId)
{
    // Based on analysis of main.cpp and binary content
    // These mappings are inferred from game object factory IDs
    switch (pId)
    {
    case 10:  return "demo_fighter";     // MR_TestElement(MR_DEMO_FIGHTER)
    case 11:  return "babe";              // Commented as unavailable in source, but present in binary
    case 12:  return "ball";              // MR_BallElement
    case 19:  return "actor_19";          // Unknown actor from binary
    case 20:  return "actor_20";          // Unknown actor from binary
    case 21:  return "actor_21";          // Unknown actor from binary
    case 22:  return "actor_22";          // Unknown actor from binary
    case 23:  return "actor_23";          // Unknown actor from binary
    default:  return "actor";             // Generic fallback
    }
}

BOOL MR_ResourceDecompiler::ExtractActor(FILE* pSrcFile, const MR_ResourceEntry& pEntry, const char* pOutputDir)
{
    if (pEntry.mData.empty())
    {
        printf("  WARNING: No actor data available\n");
        return FALSE;
    }

    // Create a temp file from the binary data
    char lTempPath[512];
    sprintf_s(lTempPath, sizeof(lTempPath), "%s\\actor_%d.tmp", pOutputDir, pEntry.mId);
    
    FILE* lTempFile = fopen(lTempPath, "wb");
    if (!lTempFile)
        return FALSE;
    
    fwrite(pEntry.mData.data(), 1, pEntry.mData.size(), lTempFile);
    fclose(lTempFile);

    // Parse binary actor data
    BOOL lResult = FALSE;
    lTempFile = fopen(lTempPath, "rb");
    if (lTempFile)
    {
        try
        {
            SimpleCArchive lArchive(lTempFile, true);
            Actor lActor;
            lActor.Serialize(lArchive);

            if (lArchive.IsGood() && lActor.mNbSequence > 0)
            {
                // Export to .msh format with actor name
                const char* lActorName = GetActorName(pEntry.mId);
                char lOutputPath[512];
                sprintf_s(lOutputPath, sizeof(lOutputPath), "%s\\actor_%s_%d.msh", pOutputDir, lActorName, pEntry.mId);
                
                FILE* lOutFile = fopen(lOutputPath, "w");
                if (lOutFile)
                {
                    lActor.ExportToMsh(lOutFile);
                    fclose(lOutFile);
                    printf("  -> %s (%u sequences)\n", lOutputPath, lActor.mNbSequence);
                    lResult = TRUE;
                }
            }
            else
            {
                printf("  ERROR: Failed to parse actor data (archive %s, seqs=%u)\n", 
                    lArchive.IsGood() ? "good" : "bad", lActor.mNbSequence);
            }
        }
        catch (...)
        {
            printf("  ERROR: Exception during actor parsing\n");
        }

        fclose(lTempFile);
    }
    
    // Clean up temp file
    remove(lTempPath);
    
    return lResult;
}

BOOL MR_ResourceDecompiler::ExtractSprite(FILE* pSrcFile, const MR_ResourceEntry& pEntry, const char* pOutputDir)
{
    // Similar to bitmap extraction
    fseek(pSrcFile, pEntry.mDataOffset, SEEK_SET);
    
    void* lData = malloc(pEntry.mDataLength);
    if (!lData)
        return FALSE;

    if ((int)fread(lData, 1, pEntry.mDataLength, pSrcFile) != pEntry.mDataLength)
    {
        free(lData);
        return FALSE;
    }

    char lOutputPath[512];
    sprintf_s(lOutputPath, sizeof(lOutputPath), "%s\\sprite_%d.dat", pOutputDir, pEntry.mId);

    FILE* lOutFile = fopen(lOutputPath, "wb");
    if (lOutFile)
    {
        fwrite(lData, 1, pEntry.mDataLength, lOutFile);
        fclose(lOutFile);
        printf("  -> %s\n", lOutputPath);
        free(lData);
        return TRUE;
    }

    free(lData);
    return FALSE;
}

BOOL MR_ResourceDecompiler::ExtractSound(FILE* pSrcFile, const MR_ResourceEntry& pEntry, const char* pOutputDir)
{
    // Read sound data from file
    fseek(pSrcFile, pEntry.mDataOffset, SEEK_SET);
    
    void* lData = malloc(pEntry.mDataLength);
    if (!lData)
        return FALSE;

    if ((int)fread(lData, 1, pEntry.mDataLength, pSrcFile) != pEntry.mDataLength)
    {
        free(lData);
        return FALSE;
    }

    // Write to temp file for CArchive parsing
    char lTempPath[512];
    sprintf_s(lTempPath, sizeof(lTempPath), "%s\\sound_%d.tmp", pOutputDir, pEntry.mId);
    
    FILE* lTempFile = fopen(lTempPath, "wb");
    if (!lTempFile)
    {
        free(lData);
        return FALSE;
    }
    
    fwrite(lData, 1, pEntry.mDataLength, lTempFile);
    fclose(lTempFile);

    // Parse binary sound data
    BOOL lResult = FALSE;
    lTempFile = fopen(lTempPath, "rb");
    if (lTempFile)
    {
        try
        {
            SimpleCArchive lArchive(lTempFile, true);
            
            // Read sound structure: nbCopy, dataLen, binary data
            uint32_t nbCopy, dataLen;
            lArchive >> nbCopy >> dataLen;

            if (lArchive.IsGood() && dataLen > 0 && dataLen < 10000000)  // Sanity check
            {
                std::vector<uint8_t> soundData(dataLen);
                if (lArchive.Read(soundData.data(), dataLen) == dataLen)
                {
                    // Write WAV file
                    char lOutputPath[512];
                    sprintf_s(lOutputPath, sizeof(lOutputPath), "%s\\sound_%d.wav", pOutputDir, pEntry.mId);
                    
                    FILE* lOutFile = fopen(lOutputPath, "wb");
                    if (lOutFile)
                    {
                        lResult = WriteWavFile(lOutFile, soundData.data(), dataLen, nbCopy);
                        fclose(lOutFile);
                        printf("  -> %s (%u bytes, %u copies)\n", lOutputPath, dataLen, nbCopy);
                    }
                }
            }
            else
            {
                printf("  ERROR: Failed to parse sound data (archive %s, len=%u)\n", 
                    lArchive.IsGood() ? "good" : "bad", dataLen);
            }
        }
        catch (...)
        {
            printf("  ERROR: Exception during sound parsing\n");
        }

        fclose(lTempFile);
    }
    
    // Clean up temp file
    remove(lTempPath);
    free(lData);
    
    return lResult;
}

BOOL MR_ResourceDecompiler::WriteMeshFile(const char* pFileName, const void* pActorData, int pDataLength)
{
    FILE* lOutFile = fopen(pFileName, "w");
    if (!lOutFile)
        return FALSE;

    // Write .msh format header
    fprintf(lOutFile, "# HoverRace Actor Mesh File\n");
    fprintf(lOutFile, "# Decompiled from binary resource library\n\n");

    // TODO: Parse pActorData to extract:
    // - Sequences (animation sequences)
    // - Frames (animation frames)
    // - Patches (3D geometry with vertices)
    // - Colors and materials
    
    // For now, write placeholder structure
    fprintf(lOutFile, "SEQUENCE\n");
    fprintf(lOutFile, "  FRAME 0\n");
    fprintf(lOutFile, "    PATCH 0\n");
    fprintf(lOutFile, "      # Placeholder - TODO: extract actual vertex data\n");
    fprintf(lOutFile, "    PATCH_END\n");
    fprintf(lOutFile, "  FRAME_END\n");
    fprintf(lOutFile, "SEQUENCE_END\n");

    fclose(lOutFile);
    return TRUE;
}

BOOL MR_ResourceDecompiler::WriteWavFile(FILE* pOutFile, const void* pSoundData, uint32_t pDataLength, uint32_t pNbCopy)
{
    if (!pOutFile || !pSoundData || pDataLength == 0)
        return FALSE;

    // Assume 44100 Hz, 8-bit, mono (standard HoverRace sound format)
    const uint32_t sampleRate = 44100;
    const uint16_t numChannels = 1;      // Mono
    const uint16_t bitsPerSample = 8;    // 8-bit
    const uint32_t byteRate = sampleRate * numChannels * (bitsPerSample / 8);
    const uint16_t blockAlign = numChannels * (bitsPerSample / 8);

    // RIFF chunk descriptor
    char riffHeader[4] = {'R', 'I', 'F', 'F'};
    uint32_t fileSize = 36 + pDataLength;  // 44 (header) - 8 (RIFF header) + audio data
    
    // fmt sub-chunk
    char fmtHeader[4] = {'f', 'm', 't', ' '};
    uint32_t fmtSize = 16;  // PCM format

    // data sub-chunk
    char dataHeader[4] = {'d', 'a', 't', 'a'};
    uint32_t dataSize = pDataLength;

    // Write RIFF header
    fwrite(riffHeader, 1, 4, pOutFile);
    fwrite(&fileSize, 4, 1, pOutFile);
    
    char waveMarker[4] = {'W', 'A', 'V', 'E'};
    fwrite(waveMarker, 1, 4, pOutFile);

    // Write fmt sub-chunk
    fwrite(fmtHeader, 1, 4, pOutFile);
    fwrite(&fmtSize, 4, 1, pOutFile);
    
    uint16_t audioFormat = 1;  // PCM
    fwrite(&audioFormat, 2, 1, pOutFile);
    fwrite(&numChannels, 2, 1, pOutFile);
    fwrite(&sampleRate, 4, 1, pOutFile);
    fwrite(&byteRate, 4, 1, pOutFile);
    fwrite(&blockAlign, 2, 1, pOutFile);
    fwrite(&bitsPerSample, 2, 1, pOutFile);

    // Write data sub-chunk
    fwrite(dataHeader, 1, 4, pOutFile);
    fwrite(&dataSize, 4, 1, pOutFile);
    fwrite(pSoundData, 1, pDataLength, pOutFile);
    
    return TRUE;
}

BOOL MR_ResourceDecompiler::CreateOutputDirectory(const char* pPath)
{
    return (_mkdir(pPath) == 0) || (GetLastError() == ERROR_ALREADY_EXISTS);
}

void MR_ResourceDecompiler::SanitizeFileName(char* pName)
{
    if (!pName) return;
    
    // Replace invalid filename characters
    for (char* p = pName; *p; p++)
    {
        switch (*p)
        {
        case '<': case '>': case ':': case '"':
        case '/': case '\\': case '|': case '?': case '*':
            *p = '_';
            break;
        }
    }
}
