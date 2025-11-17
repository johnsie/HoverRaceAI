#include "CArchiveParser.h"
#include <cstring>

SimpleCArchive::SimpleCArchive(FILE* pFile, bool pIsReading)
    : mFile(pFile), mIsReading(pIsReading), mIsGood(pFile != nullptr)
{
}

SimpleCArchive::~SimpleCArchive()
{
    // Don't close file - caller manages it
}

SimpleCArchive& SimpleCArchive::operator>>(int32_t& val)
{
    if (mIsGood && mFile)
    {
        if (fread(&val, sizeof(val), 1, mFile) != 1)
            mIsGood = false;
    }
    return *this;
}

SimpleCArchive& SimpleCArchive::operator>>(uint32_t& val)
{
    if (mIsGood && mFile)
    {
        if (fread(&val, sizeof(val), 1, mFile) != 1)
            mIsGood = false;
    }
    return *this;
}

SimpleCArchive& SimpleCArchive::operator>>(int16_t& val)
{
    if (mIsGood && mFile)
    {
        if (fread(&val, sizeof(val), 1, mFile) != 1)
            mIsGood = false;
    }
    return *this;
}

SimpleCArchive& SimpleCArchive::operator>>(uint16_t& val)
{
    if (mIsGood && mFile)
    {
        if (fread(&val, sizeof(val), 1, mFile) != 1)
            mIsGood = false;
    }
    return *this;
}

SimpleCArchive& SimpleCArchive::operator>>(uint8_t& val)
{
    if (mIsGood && mFile)
    {
        if (fread(&val, sizeof(val), 1, mFile) != 1)
            mIsGood = false;
    }
    return *this;
}

SimpleCArchive& SimpleCArchive::operator>>(float& val)
{
    if (mIsGood && mFile)
    {
        if (fread(&val, sizeof(val), 1, mFile) != 1)
            mIsGood = false;
    }
    return *this;
}

SimpleCArchive& SimpleCArchive::operator>>(double& val)
{
    if (mIsGood && mFile)
    {
        if (fread(&val, sizeof(val), 1, mFile) != 1)
            mIsGood = false;
    }
    return *this;
}

SimpleCArchive& SimpleCArchive::operator>>(char& val)
{
    if (mIsGood && mFile)
    {
        if (fread(&val, sizeof(val), 1, mFile) != 1)
            mIsGood = false;
    }
    return *this;
}

SimpleCArchive& SimpleCArchive::operator>>(std::string& str)
{
    if (mIsGood && mFile)
    {
        uint32_t len;
        if (fread(&len, sizeof(len), 1, mFile) != 1)
        {
            mIsGood = false;
            return *this;
        }

        if (len > 10000)  // Sanity check
        {
            mIsGood = false;
            return *this;
        }

        char buffer[10001];
        if (fread(buffer, 1, len, mFile) != len)
        {
            mIsGood = false;
            return *this;
        }

        buffer[len] = '\0';
        str = buffer;
    }
    return *this;
}

size_t SimpleCArchive::Read(void* pBuffer, size_t pSize)
{
    if (mIsGood && mFile)
    {
        return fread(pBuffer, 1, pSize, mFile);
    }
    return 0;
}
bool SimpleCArchive::ReadString(char* pBuffer, int pMaxLen)
{
    if (!mIsGood || !mFile || !pBuffer || pMaxLen <= 0)
        return false;

    uint32_t len;
    if (fread(&len, sizeof(len), 1, mFile) != 1)
    {
        mIsGood = false;
        return false;
    }

    if (len >= (uint32_t)pMaxLen)
    {
        mIsGood = false;
        return false;
    }

    if (fread(pBuffer, 1, len, mFile) != len)
    {
        mIsGood = false;
        return false;
    }

    pBuffer[len] = 0;
    return true;
}

long SimpleCArchive::GetPosition() const
{
    if (mFile)
        return ftell(mFile);
    return -1;
}

void SimpleCArchive::SetPosition(long pos)
{
    if (mFile)
        fseek(mFile, pos, SEEK_SET);
}

void SimpleCArchive::Seek(long offset, int origin)
{
    if (mFile)
        fseek(mFile, offset, origin);
}
