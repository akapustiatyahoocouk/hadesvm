//
//  hadesvm-ibmhfp/HfpReal64.cpp
//
//  HfpReal64 implementation
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
Real64::Real64(const Real32 & src)
    :   value(static_cast<uint64_t>(src.value) << 32)
{
}

Real64::Real64(int valueParam)
    :   value(fromInt64(valueParam))
{
}

//////////
//  Operators
Real64 Real64::operator - () const
{
    Real64 result;
    result.value = subtract64(0, value, defaultEnvironment);
    return result;
}

Real64 Real64::operator + (const Real64 & op2) const
{
    Real64 result;
    result.value = add64(value, op2.value, defaultEnvironment);
    return result;
}

Real64 Real64::operator - (const Real64 & op2) const
{
    Real64 result;
    result.value = subtract64(value, op2.value, defaultEnvironment);
    return result;
}

Real64 Real64::operator * (const Real64 & op2) const
{
    Real64 result;
    result.value = multiply64(value, op2.value, defaultEnvironment);
    return result;
}

Real64 Real64::operator / (const Real64 & op2) const
{
    Real64 result;
    result.value = divide64(value, op2.value, defaultEnvironment);
    return result;
}

Real64 & Real64::operator += (const Real64 & op2)
{
    *this = *this + op2;
    return *this;
}

Real64 & Real64::operator -= (const Real64 & op2)
{
    *this = *this - op2;
    return *this;
}

Real64 & Real64::operator *= (const Real64 & op2)
{
    *this = *this * op2;
    return *this;
}

Real64 & Real64::operator /= (const Real64 & op2)
{
    *this = *this / op2;
    return *this;
}


bool Real64::operator == (const Real64 & op2) const
{
    return compare64(value, op2.value, defaultEnvironment) == 0;
}

bool Real64::operator != (const Real64 & op2) const
{
    return compare64(value, op2.value, defaultEnvironment) != 0;
}

bool Real64::operator <  (const Real64 & op2) const
{
    return compare64(value, op2.value, defaultEnvironment) < 0;
}

bool Real64::operator <= (const Real64 & op2) const
{
    return compare64(value, op2.value, defaultEnvironment) <= 0;
}

bool Real64::operator >  (const Real64 & op2) const
{
    return compare64(value, op2.value, defaultEnvironment) > 0;
}

bool Real64::operator >= (const Real64 & op2) const
{
    return compare64(value, op2.value, defaultEnvironment) >= 0;
}

Real64::operator double() const
{
    return toDouble64(value);
}

//  End of hadesvm-ibmhfp/HfpReal64.cpp
