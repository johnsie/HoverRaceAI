// GraphicsUtilities.cpp
// Implementation of graphics utilities

#include "stdafx.h"
#include "GraphicsUtilities.h"

// Undefine Windows macros that conflict with std::min/max
#undef min
#undef max

namespace MR_Interpolation
{
    double Linear(double pStart, double pEnd, double pT)
    {
        // Clamp T to [0, 1]
        double t = std::max(0.0, std::min(1.0, pT));
        return pStart + (pEnd - pStart) * t;
    }

    double Smooth(double pStart, double pEnd, double pT)
    {
        // Smooth step interpolation: 3t^2 - 2t^3
        double t = std::max(0.0, std::min(1.0, pT));
        double smoothT = t * t * (3.0 - 2.0 * t);
        return pStart + (pEnd - pStart) * smoothT;
    }

    double Ease(double pStart, double pEnd, double pT, double pPower)
    {
        // Power-based easing
        double t = std::max(0.0, std::min(1.0, pT));
        double easeT = std::pow(t, pPower);
        return pStart + (pEnd - pStart) * easeT;
    }
}
