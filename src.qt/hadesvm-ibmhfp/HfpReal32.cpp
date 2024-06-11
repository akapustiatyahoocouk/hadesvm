//
//  hadesvm-ibmhfp/HfpReal32.cpp
//
//  HfpReal32 implementation
//
//////////
#include "hadesvm-ibmhfp/API.hpp"
using namespace hadesvm::ibmhfp;

namespace
{
    Environment defaultEnvironment;
}

//////////
//  Construction
Real32::Real32(const Real64 & src)
    :   value(static_cast<uint32_t>(src.value >> 32))
{
}

Real32::Real32(int valueParam)
    :   value(fromInt32(valueParam))
{
}

//////////
//  Operators
Real32 Real32::operator - () const
{
    Real32 result;
    result.value = subtract32(0, value, defaultEnvironment);
    return result;
}

Real32 Real32::operator + (const Real32 & op2) const
{
    Real32 result;
    result.value = add32(value, op2.value, defaultEnvironment);
    return result;
}

Real32 Real32::operator - (const Real32 & op2) const
{
    Real32 result;
    result.value = subtract32(value, op2.value, defaultEnvironment);
    return result;
}

Real32 Real32::operator * (const Real32 & op2) const
{
    Real32 result;
    result.value = multiply32(value, op2.value, defaultEnvironment);
    return result;
}

Real32 Real32::operator / (const Real32 & op2) const
{
    Real32 result;
    result.value = divide32(value, op2.value, defaultEnvironment);
    return result;
}


Real32 & Real32::operator += (const Real32 & op2)
{
    *this = *this + op2;
    return *this;
}

Real32 & Real32::operator -= (const Real32 & op2)
{
    *this = *this - op2;
    return *this;
}

Real32 & Real32::operator *= (const Real32 & op2)
{
    *this = *this * op2;
    return *this;
}

Real32 & Real32::operator /= (const Real32 & op2)
{
    *this = *this / op2;
    return *this;
}

bool Real32::operator == (const Real32 & op2) const
{
    return compare32(value, op2.value, defaultEnvironment) == 0;
}

bool Real32::operator != (const Real32 & op2) const
{
    return compare32(value, op2.value, defaultEnvironment) != 0;
}

bool Real32::operator <  (const Real32 & op2) const
{
    return compare32(value, op2.value, defaultEnvironment) < 0;
}

bool Real32::operator <= (const Real32 & op2) const
{
    return compare32(value, op2.value, defaultEnvironment) <= 0;
}

bool Real32::operator >  (const Real32 & op2) const
{
    return compare32(value, op2.value, defaultEnvironment) > 0;
}

bool Real32::operator >= (const Real32 & op2) const
{
    return compare32(value, op2.value, defaultEnvironment) >= 0;
}

Real32::operator double() const
{
    return toDouble32(value);
}

//  End of hadesvm-ibmhfp/HfpReal32.cpp
