//
//  hadesvm-util/Math.cpp
//
//  The hadesvm::util::Math class implementation
//
//////////
#include "hadesvm-util/API.hpp"
using namespace hadesvm::util;

//////////
//  Operations (fp)
bool Math::isNan(float x)
{
    if (std::numeric_limits<float>::is_iec559)
    {   //  IEEE-754
        return isnan(x) != 0;
    }
    else
    {   //  Backup fits-all implementation
        return isnan(x) != 0;
    }
}

bool Math::isNan(double x)
{
    if (std::numeric_limits<double>::is_iec559)
    {   //  IEEE-754
        return isnan(x) != 0;
    }
    else
    {   //  Backup fits-all implementation
        return isnan(x) != 0;
    }
}

bool Math::isSignalingNan(float /*x*/)
{
    if (std::numeric_limits<float>::is_iec559)
    {   //  IEEE-754
        return false;   //  TODO implement properly with bit-pattern analysis
    }
    else
    {   //  Backup fits-all implementation
        return false;
    }
}

bool Math::isSignalingNan(double /*x*/)
{
    if (std::numeric_limits<double>::is_iec559)
    {   //  IEEE-754
        return false;   //  TODO implement properly with bit-pattern analysis
    }
    else
    {   //  Backup fits-all implementation
        return false;
    }
}

bool Math::isQuietNan(float x)
{
    if (std::numeric_limits<float>::is_iec559)
    {   //  IEEE-754
        return isnan(x) != 0;   //  TODO implement properly with bit-pattern analysis
    }
    else
    {   //  Backup fits-all implementation
        return isnan(x) != 0;
    }
}

bool Math::isQuietNan(double x)
{
    if (std::numeric_limits<double>::is_iec559)
    {   //  IEEE-754
        return isnan(x) != 0;   //  TODO implement properly with bit-pattern analysis
    }
    else
    {   //  Backup fits-all implementation
        return isnan(x) != 0;
    }
}

bool Math::isInfinite(float x)
{
    if (std::numeric_limits<float>::is_iec559)
    {   //  IEEE-754
        return isinf(x) != 0;   //  TODO implement properly with bit-pattern analysis
    }
    else
    {   //  Backup fits-all implementation
        return isinf(x) != 0;
    }
}

bool Math::isInfinite(double x)
{
    if (std::numeric_limits<double>::is_iec559)
    {   //  IEEE-754
        return isinf(x) != 0;   //  TODO implement properly with bit-pattern analysis
    }
    else
    {   //  Backup fits-all implementation
        return isinf(x) != 0;
    }
}

bool Math::isPositiveInfinite(float x)
{
    return isInfinite(x) && x > 0;
}

bool Math::isPositiveInfinite(double x)
{
    return isInfinite(x) && x > 0;
}

bool Math::isNegativeInfinite(float x)
{
    return isInfinite(x) && x < 0;
}

bool Math::isNegativeInfinite(double x)
{
    return isInfinite(x) && x < 0;
}

//  End of hadesvm-util/Math.cpp
