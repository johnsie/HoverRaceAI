// GraphicsUtilities.h
// Modern Graphics Utilities for HoverRace
// Provides math, transforms, and memory safety utilities

#ifndef MR_GRAPHICS_UTILITIES_H
#define MR_GRAPHICS_UTILITIES_H

#include "../Util/MR_Types.h"
#include <cmath>
#include <algorithm>
#include <memory>

#ifdef MR_VIDEO_SERVICES
#define MR_DllDeclare __declspec(dllexport)
#else
#define MR_DllDeclare __declspec(dllimport)
#endif

// ============================================================================
// 2D/3D Vector Math
// ============================================================================

class MR_DllDeclare MR_Vector2D
{
public:
    double x, y;

    MR_Vector2D() : x(0), y(0) {}
    MR_Vector2D(double pX, double pY) : x(pX), y(pY) {}

    MR_Vector2D operator+(const MR_Vector2D& pOther) const { return MR_Vector2D(x + pOther.x, y + pOther.y); }
    MR_Vector2D operator-(const MR_Vector2D& pOther) const { return MR_Vector2D(x - pOther.x, y - pOther.y); }
    MR_Vector2D operator*(double pScalar) const { return MR_Vector2D(x * pScalar, y * pScalar); }
    MR_Vector2D operator/(double pScalar) const { return pScalar != 0 ? MR_Vector2D(x / pScalar, y / pScalar) : MR_Vector2D(0, 0); }

    double Dot(const MR_Vector2D& pOther) const { return x * pOther.x + y * pOther.y; }
    double Cross(const MR_Vector2D& pOther) const { return x * pOther.y - y * pOther.x; }
    double Length() const { return std::sqrt(x * x + y * y); }
    double LengthSquared() const { return x * x + y * y; }
    
    MR_Vector2D Normalized() const
    {
        double len = Length();
        return len > 0 ? *this / len : MR_Vector2D(0, 0);
    }

    double Distance(const MR_Vector2D& pOther) const { return (*this - pOther).Length(); }
};

class MR_DllDeclare MR_Vector3D
{
public:
    double x, y, z;

    MR_Vector3D() : x(0), y(0), z(0) {}
    MR_Vector3D(double pX, double pY, double pZ) : x(pX), y(pY), z(pZ) {}

    MR_Vector3D operator+(const MR_Vector3D& pOther) const { return MR_Vector3D(x + pOther.x, y + pOther.y, z + pOther.z); }
    MR_Vector3D operator-(const MR_Vector3D& pOther) const { return MR_Vector3D(x - pOther.x, y - pOther.y, z - pOther.z); }
    MR_Vector3D operator*(double pScalar) const { return MR_Vector3D(x * pScalar, y * pScalar, z * pScalar); }
    MR_Vector3D operator/(double pScalar) const { return pScalar != 0 ? MR_Vector3D(x / pScalar, y / pScalar, z / pScalar) : MR_Vector3D(0, 0, 0); }

    double Dot(const MR_Vector3D& pOther) const { return x * pOther.x + y * pOther.y + z * pOther.z; }
    MR_Vector3D Cross(const MR_Vector3D& pOther) const
    {
        return MR_Vector3D(
            y * pOther.z - z * pOther.y,
            z * pOther.x - x * pOther.z,
            x * pOther.y - y * pOther.x
        );
    }
    
    double Length() const { return std::sqrt(x * x + y * y + z * z); }
    double LengthSquared() const { return x * x + y * y + z * z; }
    
    MR_Vector3D Normalized() const
    {
        double len = Length();
        return len > 0 ? *this / len : MR_Vector3D(0, 0, 0);
    }

    double Distance(const MR_Vector3D& pOther) const { return (*this - pOther).Length(); }
};

// ============================================================================
// Matrix Transformations
// ============================================================================

class MR_DllDeclare MR_Matrix4x4
{
public:
    double m[4][4];

    MR_Matrix4x4() 
    { 
        Zero();
        m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0;
    }

    static MR_Matrix4x4 Identity()
    {
        MR_Matrix4x4 mat;
        mat.Zero();
        mat.m[0][0] = mat.m[1][1] = mat.m[2][2] = mat.m[3][3] = 1.0;
        return mat;
    }

    static MR_Matrix4x4 Translation(double pX, double pY, double pZ)
    {
        MR_Matrix4x4 mat = Identity();
        mat.m[0][3] = pX;
        mat.m[1][3] = pY;
        mat.m[2][3] = pZ;
        return mat;
    }

    static MR_Matrix4x4 RotationX(double pAngleRad)
    {
        MR_Matrix4x4 mat = Identity();
        double c = std::cos(pAngleRad);
        double s = std::sin(pAngleRad);
        mat.m[1][1] = c; mat.m[1][2] = -s;
        mat.m[2][1] = s; mat.m[2][2] = c;
        return mat;
    }

    static MR_Matrix4x4 RotationY(double pAngleRad)
    {
        MR_Matrix4x4 mat = Identity();
        double c = std::cos(pAngleRad);
        double s = std::sin(pAngleRad);
        mat.m[0][0] = c; mat.m[0][2] = s;
        mat.m[2][0] = -s; mat.m[2][2] = c;
        return mat;
    }

    static MR_Matrix4x4 RotationZ(double pAngleRad)
    {
        MR_Matrix4x4 mat = Identity();
        double c = std::cos(pAngleRad);
        double s = std::sin(pAngleRad);
        mat.m[0][0] = c; mat.m[0][1] = -s;
        mat.m[1][0] = s; mat.m[1][1] = c;
        return mat;
    }

    static MR_Matrix4x4 Scale(double pX, double pY, double pZ)
    {
        MR_Matrix4x4 mat = Identity();
        mat.m[0][0] = pX;
        mat.m[1][1] = pY;
        mat.m[2][2] = pZ;
        return mat;
    }

    MR_Matrix4x4 operator*(const MR_Matrix4x4& pOther) const
    {
        MR_Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result.m[i][j] = 0;
                for (int k = 0; k < 4; ++k)
                {
                    result.m[i][j] += m[i][k] * pOther.m[k][j];
                }
            }
        }
        return result;
    }

    MR_Vector3D Transform(const MR_Vector3D& pVec) const
    {
        double x = pVec.x * m[0][0] + pVec.y * m[0][1] + pVec.z * m[0][2] + m[0][3];
        double y = pVec.x * m[1][0] + pVec.y * m[1][1] + pVec.z * m[1][2] + m[1][3];
        double z = pVec.x * m[2][0] + pVec.y * m[2][1] + pVec.z * m[2][2] + m[2][3];
        return MR_Vector3D(x, y, z);
    }

private:
    void Zero() { std::memset(m, 0, sizeof(m)); }
};

// ============================================================================
// Safe Memory Management Utilities
// ============================================================================

namespace MR_MemorySafety
{
    // RAII wrapper for buffer allocation
    class MR_DllDeclare MR_BufferGuard
    {
    public:
        explicit MR_BufferGuard(size_t pSize) : m_buffer(nullptr), m_size(pSize)
        {
            if (pSize > 0)
            {
                try
                {
                    m_buffer = new uint8_t[pSize];
                    std::memset(m_buffer, 0, pSize);
                }
                catch (...)
                {
                    m_buffer = nullptr;
                }
            }
        }

        ~MR_BufferGuard()
        {
            if (m_buffer)
            {
                delete[] m_buffer;
                m_buffer = nullptr;
            }
        }

        uint8_t* Get() { return m_buffer; }
        const uint8_t* Get() const { return m_buffer; }
        size_t Size() const { return m_size; }
        bool IsValid() const { return m_buffer != nullptr; }

        // Prevent copying
        MR_BufferGuard(const MR_BufferGuard&) = delete;
        MR_BufferGuard& operator=(const MR_BufferGuard&) = delete;

    private:
        uint8_t* m_buffer;
        size_t m_size;
    };

    // Bounds checking utilities
    inline bool IsBoundsValid(int pX, int pY, int pWidth, int pHeight)
    {
        return pX >= 0 && pX < pWidth && pY >= 0 && pY < pHeight;
    }

    inline bool IsRectBoundsValid(int pX, int pY, int pW, int pH, int pMaxWidth, int pMaxHeight)
    {
        return pX >= 0 && pY >= 0 && (pX + pW) <= pMaxWidth && (pY + pH) <= pMaxHeight;
    }

    // Safe pointer dereferencing
    template<typename T>
    inline T* SafeGetPtr(T* pPtr)
    {
        return pPtr;  // In debug mode, this could add validation
    }

    template<typename T>
    inline const T* SafeGetPtr(const T* pPtr)
    {
        return pPtr;  // In debug mode, this could add validation
    }
}

// ============================================================================
// Interpolation and Blending
// ============================================================================

namespace MR_Interpolation
{
    MR_DllDeclare double Linear(double pStart, double pEnd, double pT);
    MR_DllDeclare double Smooth(double pStart, double pEnd, double pT);
    MR_DllDeclare double Ease(double pStart, double pEnd, double pT, double pPower = 2.0);
}

#undef MR_DllDeclare

#endif  // MR_GRAPHICS_UTILITIES_H
