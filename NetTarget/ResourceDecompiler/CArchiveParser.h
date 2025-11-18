#pragma once

#include <cstdio>
#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>

// Simple CArchive-like reader for binary data
// Mimics MFC CArchive for reading serialized data
class SimpleCArchive
{
public:
    SimpleCArchive(FILE* pFile, bool pIsReading = true);
    ~SimpleCArchive();

    // Read/Write operators
    SimpleCArchive& operator>>(int32_t& val);
    SimpleCArchive& operator>>(uint32_t& val);
    SimpleCArchive& operator>>(int16_t& val);
    SimpleCArchive& operator>>(uint16_t& val);
    SimpleCArchive& operator>>(uint8_t& val);
    SimpleCArchive& operator>>(float& val);
    SimpleCArchive& operator>>(double& val);
    SimpleCArchive& operator>>(char& val);
    SimpleCArchive& operator>>(std::string& str);

    // Raw read for binary data
    size_t Read(void* pBuffer, size_t pSize);

    // Special read for null-terminated strings
    bool ReadString(char* pBuffer, int pMaxLen);

    // Position tracking
    long GetPosition() const;
    void SetPosition(long pos);
    void Seek(long offset, int origin);

    // Status
    bool IsGood() const { return mIsGood; }

private:
    FILE* mFile;
    bool mIsReading;
    bool mIsGood;
};

// Helper structs for deserialization
struct MR_3DCoordinate
{
    int32_t mX, mY, mZ;  // Millimeters precision

    MR_3DCoordinate() : mX(0), mY(0), mZ(0) {}

    friend SimpleCArchive& operator>>(SimpleCArchive& ar, MR_3DCoordinate& coord)
    {
        ar >> coord.mX >> coord.mY >> coord.mZ;
        return ar;
    }
};

// Bitmap subcomponent
struct SubBitmap
{
    int32_t mXRes, mYRes;
    int32_t mXResShiftFactor, mYResShiftFactor;
    int32_t mHaveTransparent;  // BOOL - 4 bytes in Windows
    std::vector<uint8_t> mBuffer;

    void Serialize(SimpleCArchive& ar)
    {
        ar >> mXRes >> mYRes >> mXResShiftFactor >> mYResShiftFactor >> mHaveTransparent;
        
        mBuffer.resize(mXRes * mYRes);
        ar.Read(mBuffer.data(), mXRes * mYRes);
    }
};

// Bitmap resource
struct Bitmap
{
    int32_t mWidth, mHeight;
    int32_t mXRes, mYRes;
    int32_t mSubBitmapCount;
    uint8_t mPlainColor;
    std::vector<SubBitmap> mSubBitmaps;

    void Serialize(SimpleCArchive& ar)
    {
        ar >> mWidth >> mHeight >> mXRes >> mYRes;
        ar >> mSubBitmapCount >> mPlainColor;
        
        mSubBitmaps.resize(mSubBitmapCount);
        for (size_t i = 0; i < mSubBitmaps.size(); i++)
        {
            mSubBitmaps[i].Serialize(ar);
        }
    }

    // Game's basic palette (68 colors as RGB triples, normalized to 0.0-1.0)
    // From HoverRace VideoServices/ColorTab.cpp
    static const double GAME_PALETTE[68][3];
    
    // Get the game palette data directly (inline version)
    static const double* GetGamePalette()
    {
        static const double palette[68][3] = {
            { 1.000000, 1.000000, 1.000000 },  // White
            { 0.000000, 0.000000, 0.000000 },  // Black
            { 0.937500, 0.937500, 0.937500 },  // Light gray
            { 0.875000, 0.875000, 0.875000 },
            { 0.812500, 0.812500, 0.812500 },
            { 0.750000, 0.750000, 0.750000 },
            { 0.687500, 0.687500, 0.687500 },
            { 0.625000, 0.625000, 0.625000 },
            { 0.562500, 0.562500, 0.562500 },
            { 0.500000, 0.500000, 0.500000 },  // Mid gray
            { 0.437500, 0.437500, 0.437500 },
            { 0.375000, 0.375000, 0.375000 },
            { 0.312500, 0.312500, 0.312500 },
            { 0.250000, 0.250000, 0.250000 },
            { 0.187500, 0.187500, 0.187500 },
            { 0.125000, 0.125000, 0.125000 },
            { 0.062500, 0.062500, 0.062500 },  // Dark gray
            { 0.972549, 0.647059, 0.384314 },  // Tan
            { 0.875294, 0.582353, 0.345882 },
            { 0.680784, 0.452941, 0.269020 },
            { 0.979412, 0.735294, 0.538235 },
            { 0.349020, 0.803922, 0.270588 },  // Green
            { 0.261765, 0.602941, 0.202941 },
            { 0.174510, 0.401961, 0.135294 },
            { 0.674510, 0.901961, 0.635294 },
            { 0.992157, 0.039216, 0.039216 },  // Red
            { 0.744118, 0.029412, 0.029412 },
            { 0.496078, 0.019608, 0.019608 },
            { 0.996078, 0.519608, 0.519608 },
            { 0.988235, 0.843137, 0.003922 },  // Yellow
            { 0.741176, 0.632353, 0.002941 },
            { 0.494118, 0.421569, 0.001961 },
            { 0.247059, 0.210784, 0.000980 },
            { 0.890196, 0.023529, 0.243137 },  // Magenta/Pink
            { 0.801176, 0.021176, 0.218824 },
            { 0.712157, 0.018824, 0.194510 },
            { 0.623137, 0.016471, 0.170196 },
            { 0.917647, 0.267647, 0.432353 },
            { 0.945098, 0.511765, 0.621569 },
            { 0.972549, 0.755882, 0.810784 },
            { 0.000000, 0.172549, 0.756863 },  // Blue
            { 0.000000, 0.155294, 0.681176 },
            { 0.000000, 0.138039, 0.605490 },
            { 0.000000, 0.120784, 0.529804 },
            { 0.250000, 0.379412, 0.817647 },
            { 0.500000, 0.586275, 0.878431 },
            { 0.750000, 0.793137, 0.939216 },
            { 0.556863, 0.274510, 0.650980 },  // Purple
            { 0.501176, 0.247059, 0.585882 },
            { 0.445490, 0.219608, 0.520784 },
            { 0.389804, 0.192157, 0.455686 },
            { 0.667647, 0.455882, 0.738235 },
            { 0.778431, 0.637255, 0.825490 },
            { 0.889216, 0.818627, 0.912745 },
            { 0.968627, 0.968627, 0.000000 },  // Yellow variants
            { 0.887908, 0.887908, 0.000000 },
            { 0.807190, 0.807190, 0.000000 },
            { 0.726471, 0.726471, 0.000000 },
            { 0.976471, 0.976471, 0.250000 },
            { 0.984314, 0.984314, 0.500000 },
            { 0.992157, 0.992157, 0.750000 },
            { 1.000000, 0.533333, 0.066667 },  // Orange
            { 0.916667, 0.488889, 0.061111 },
            { 0.833333, 0.444444, 0.055556 },
            { 0.750000, 0.400000, 0.050000 },
            { 1.000000, 0.650000, 0.300000 },
            { 1.000000, 0.766667, 0.533333 },
            { 1.000000, 0.883333, 0.766667 }
        };
        return (const double*)palette;
    }

    // Generate game palette with gamma correction
    static void GenerateGamePalette(uint8_t* pOutPalette, double pGamma = 1.0, double pIntensity = 0.8, double pIntensityBase = 0.0)
    {
        // Apply gamma correction to basic palette
        const double* gamePalette = GetGamePalette();
        int colorIdx = 0;
        
        // First 68 colors from game palette with gamma correction
        for (colorIdx = 0; colorIdx < 68; colorIdx++)
        {
            double r = pIntensityBase + pIntensity * pow(gamePalette[colorIdx*3 + 0], pGamma);
            double g = pIntensityBase + pIntensity * pow(gamePalette[colorIdx*3 + 1], pGamma);
            double b = pIntensityBase + pIntensity * pow(gamePalette[colorIdx*3 + 2], pGamma);
            
            // Clamp to 0-255 range
            if (r >= 256.0) r = 255.0;
            if (g >= 256.0) g = 255.0;
            if (b >= 256.0) b = 255.0;
            
            pOutPalette[colorIdx * 4 + 0] = (uint8_t)(b * 256);  // B
            pOutPalette[colorIdx * 4 + 1] = (uint8_t)(g * 256);  // G
            pOutPalette[colorIdx * 4 + 2] = (uint8_t)(r * 256);  // R
            pOutPalette[colorIdx * 4 + 3] = 0;                   // A (unused)
        }
        
        // Remaining 188 colors: fill with white/grayscale
        for (colorIdx = 68; colorIdx < 256; colorIdx++)
        {
            uint8_t gray = (uint8_t)((colorIdx - 68) * 6 / 5);  // Gradual grayscale
            pOutPalette[colorIdx * 4 + 0] = gray;  // B
            pOutPalette[colorIdx * 4 + 1] = gray;  // G
            pOutPalette[colorIdx * 4 + 2] = gray;  // R
            pOutPalette[colorIdx * 4 + 3] = 0;     // A
        }
    }

    // Export to .bmp format (8-bit indexed color)
    void ExportToBMP(FILE* pOutFile) const
    {
        if (mSubBitmaps.empty()) return;

        const auto& subBmp = mSubBitmaps[0];  // Use first sub-bitmap
        uint32_t width = subBmp.mXRes;
        uint32_t height = subBmp.mYRes;
        uint32_t rowSize = ((width + 3) / 4) * 4;  // BMP rows must be 4-byte aligned
        uint32_t paletteSize = 256 * 4;  // 256 colors * 4 bytes (BGRA)
        uint32_t imageDataSize = rowSize * height;
        uint32_t fileSize = 14 + 40 + paletteSize + imageDataSize;  // File header + DIB header + palette + data

        // BMP File Header (14 bytes)
        uint8_t fileHeader[14];
        fileHeader[0] = 'B';  fileHeader[1] = 'M';
        *(uint32_t*)&fileHeader[2] = fileSize;  // File size
        *(uint32_t*)&fileHeader[6] = 0;         // Reserved
        *(uint32_t*)&fileHeader[10] = 14 + 40 + paletteSize;  // Offset to pixel data
        fwrite(fileHeader, 1, 14, pOutFile);

        // DIB Header (40 bytes - BITMAPINFOHEADER)
        uint8_t dibHeader[40];
        *(uint32_t*)&dibHeader[0] = 40;          // Header size
        *(uint32_t*)&dibHeader[4] = width;       // Width
        *(uint32_t*)&dibHeader[8] = height;      // Height
        *(uint16_t*)&dibHeader[12] = 1;          // Planes
        *(uint16_t*)&dibHeader[14] = 8;          // Bits per pixel
        *(uint32_t*)&dibHeader[16] = 0;          // Compression (none)
        *(uint32_t*)&dibHeader[20] = imageDataSize;  // Image size
        *(uint32_t*)&dibHeader[24] = 2835;       // X pixels per meter (72 DPI)
        *(uint32_t*)&dibHeader[28] = 2835;       // Y pixels per meter (72 DPI)
        *(uint32_t*)&dibHeader[32] = 256;        // Colors used
        *(uint32_t*)&dibHeader[36] = 0;          // Important colors
        fwrite(dibHeader, 1, 40, pOutFile);

        // Palette (256 colors, 4 bytes each - BGRA) - use game palette with gamma correction
        uint8_t gamePalette[1024];  // 256 colors * 4 bytes
        GenerateGamePalette(gamePalette);
        fwrite(gamePalette, 1, paletteSize, pOutFile);

        // Write image data (bottom-up for BMP)
        for (int y = height - 1; y >= 0; y--)
        {
            fwrite(&subBmp.mBuffer[y * width], 1, width, pOutFile);
            // Pad row to 4-byte boundary if needed
            uint32_t padding = rowSize - width;
            if (padding > 0)
            {
                uint8_t pad[4] = {0};
                fwrite(pad, 1, padding, pOutFile);
            }
        }
    }
};

// Actor patch (textured quad)
struct ActorPatch
{
    uint32_t mURes, mVRes;           // Grid resolution
    uint32_t mBitmapId;              // Texture bitmap ID
    std::vector<MR_3DCoordinate> mVertices;

    void Serialize(SimpleCArchive& ar)
    {
        ar >> mURes >> mVRes >> mBitmapId;
        
        mVertices.resize(mURes * mVRes);
        for (size_t i = 0; i < mVertices.size(); i++)
        {
            ar >> mVertices[i];
        }
    }
};

// Actor frame (collection of patches)
struct ActorFrame
{
    uint32_t mNbComponent;
    std::vector<ActorPatch> mPatches;

    void Serialize(SimpleCArchive& ar)
    {
        ar >> mNbComponent;
        
        mPatches.resize(mNbComponent);
        for (size_t i = 0; i < mPatches.size(); i++)
        {
            int32_t lType;
            ar >> lType;  // Component type (should be 0 for ePatch)
            
            if (lType == 0)  // ePatch
            {
                mPatches[i].Serialize(ar);
            }
        }
    }
};

// Actor sequence (animation)
struct ActorSequence
{
    uint32_t mNbFrame;
    std::vector<ActorFrame> mFrames;

    void Serialize(SimpleCArchive& ar)
    {
        ar >> mNbFrame;
        
        mFrames.resize(mNbFrame);
        for (size_t i = 0; i < mFrames.size(); i++)
        {
            mFrames[i].Serialize(ar);
        }
    }
};

// Actor (character/object with sequences)
struct Actor
{
    uint32_t mNbSequence;
    std::vector<ActorSequence> mSequences;

    void Serialize(SimpleCArchive& ar)
    {
        ar >> mNbSequence;
        
        mSequences.resize(mNbSequence);
        for (size_t i = 0; i < mSequences.size(); i++)
        {
            mSequences[i].Serialize(ar);
        }
    }

    // Export to .msh format
    void ExportToMsh(FILE* pOutFile) const
    {
        for (uint32_t seqIdx = 0; seqIdx < mNbSequence; seqIdx++)
        {
            fprintf(pOutFile, "SEQUENCE\n");
            const auto& seq = mSequences[seqIdx];
            
            for (uint32_t frameIdx = 0; frameIdx < seq.mNbFrame; frameIdx++)
            {
                fprintf(pOutFile, "FRAME\n");
                const auto& frame = seq.mFrames[frameIdx];

                for (size_t patchIdx = 0; patchIdx < frame.mPatches.size(); patchIdx++)
                {
                    const auto& patch = frame.mPatches[patchIdx];
                    fprintf(pOutFile, "PATCH\n");
                    fprintf(pOutFile, "%u %u\n", patch.mURes, patch.mVRes);

                    for (size_t v = 0; v < patch.mVertices.size(); v++)
                    {
                        const auto& vert = patch.mVertices[v];
                        fprintf(pOutFile, "%d %d %d\n", vert.mX, vert.mY, vert.mZ);
                    }

                    // Output bitmap filename based on bitmap ID
                    fprintf(pOutFile, "%s \n\n", GetBitmapFilename(patch.mBitmapId));
                }

                fprintf(pOutFile, "FRAME\n");
            }

            fprintf(pOutFile, "SEQUENCE\n");
        }
    }

    // Helper function to get bitmap filename from ID
    static const char* GetBitmapFilename(uint32_t pBitmapId)
    {
        // Map bitmap IDs to filenames based on common HoverRace textures
        switch (pBitmapId)
        {
        case 0:   return "basic_flame.bmp";
        case 1:   return "missile.bmp";
        case 2:   return "missile_wing.bmp";
        case 3:   return "etalon.pcx";
        case 4:   return "wood1.pcx";
        case 5:   return "red_brick.pcx";
        case 6:   return "big_brick.pcx";
        case 7:   return "FirePat.pcx";
        case 8:   return "StdFloor.pcx";
        case 9:   return "Wallr.pcx";
        case 10:  return "Walll.pcx";
        case 11:  return "WallrRed.pcx";
        case 12:  return "WalllRed.pcx";
        case 13:  return "WallrGreen.pcx";
        case 14:  return "WalllGreen.pcx";
        case 15:  return "WallStep.pcx";
        case 16:  return "WallPassRight.pcx";
        case 17:  return "WallPassLeft.pcx";
        case 18:  return "WallDoNotEnter.pcx";
        default:  return "texture.bmp";
        }
    }
};
