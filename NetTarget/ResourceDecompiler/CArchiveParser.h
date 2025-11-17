#pragma once

#include <cstdio>
#include <cstdint>
#include <vector>
#include <string>
#include <cstring>

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

    // Export to .tga format (8-bit indexed color)
    void ExportToTGA(FILE* pOutFile) const
    {
        if (mSubBitmaps.empty()) return;

        const auto& subBmp = mSubBitmaps[0];  // Use first sub-bitmap
        uint32_t width = subBmp.mXRes;
        uint32_t height = subBmp.mYRes;

        // TGA header (24 bytes)
        uint8_t header[18] = {0};
        header[0] = 0;                          // ID length
        header[1] = 1;                          // Color map type (1 = yes)
        header[2] = 1;                          // Image type (1 = indexed)
        header[3] = 0; header[4] = 0;          // Color map start
        header[5] = 0; header[6] = 1;          // Color map length (256)
        header[7] = 8;                          // Color map entry size (8 bits)
        header[8] = 0; header[9] = 0;          // X origin
        header[10] = 0; header[11] = 0;        // Y origin
        header[12] = width & 0xFF;
        header[13] = (width >> 8) & 0xFF;      // Image width
        header[14] = height & 0xFF;
        header[15] = (height >> 8) & 0xFF;     // Image height
        header[16] = 8;                         // Bits per pixel
        header[17] = 0;                         // Image descriptor

        fwrite(header, 1, 18, pOutFile);

        // Write 256-color palette (grayscale)
        for (int i = 0; i < 256; i++)
        {
            uint8_t rgb[3] = {(uint8_t)i, (uint8_t)i, (uint8_t)i};
            fwrite(rgb, 1, 3, pOutFile);
        }

        // Write image data (bottom-up for TGA)
        for (int y = height - 1; y >= 0; y--)
        {
            fwrite(&subBmp.mBuffer[y * width], 1, width, pOutFile);
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
        fprintf(pOutFile, "# HoverRace Actor Mesh File\n");
        fprintf(pOutFile, "# Decompiled from binary resource library\n");
        fprintf(pOutFile, "# Sequences: %u\n\n", mNbSequence);

        for (uint32_t seqIdx = 0; seqIdx < mNbSequence; seqIdx++)
        {
            fprintf(pOutFile, "SEQUENCE %u\n", seqIdx);
            const auto& seq = mSequences[seqIdx];
            
            for (uint32_t frameIdx = 0; frameIdx < seq.mNbFrame; frameIdx++)
            {
                fprintf(pOutFile, "  FRAME %u\n", frameIdx);
                const auto& frame = seq.mFrames[frameIdx];

                for (size_t patchIdx = 0; patchIdx < frame.mPatches.size(); patchIdx++)
                {
                    const auto& patch = frame.mPatches[patchIdx];
                    fprintf(pOutFile, "    PATCH %zu\n", patchIdx);
                    fprintf(pOutFile, "      BITMAP_ID %u\n", patch.mBitmapId);
                    fprintf(pOutFile, "      GRID %u %u\n", patch.mURes, patch.mVRes);
                    fprintf(pOutFile, "      VERTICES\n");

                    for (size_t v = 0; v < patch.mVertices.size(); v++)
                    {
                        const auto& vert = patch.mVertices[v];
                        fprintf(pOutFile, "        %d %d %d\n", vert.mX, vert.mY, vert.mZ);
                    }

                    fprintf(pOutFile, "      VERTICES_END\n");
                    fprintf(pOutFile, "    PATCH_END\n");
                }

                fprintf(pOutFile, "  FRAME_END\n");
            }

            fprintf(pOutFile, "SEQUENCE_END\n\n");
        }
    }
};
