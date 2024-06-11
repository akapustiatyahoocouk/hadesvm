//
//  hadesvm-ibmhfp/Hfp.cpp
//
//  HFP implementation
//
//////////
#include "hadesvm-ibmhfp/API.hpp"
using namespace hadesvm::ibmhfp;

//////////
//  Helpers
static uint64_t _constructValue(int sign, int exponent, uint64_t mantissa, Environment & env)
{
    if (mantissa == 0)
    {
        return 0;
    }
    while ((mantissa & 0xFF00000000000000) != 0)
    {
        mantissa >>= 4;
        exponent++;
    }
    while ((mantissa & 0x00F0000000000000) == 0)
    {
        mantissa <<= 4;
        exponent--;
    }

    //  Form the result
    if (exponent < -64 - 14)
    {   //  Underflow
        env.underflow = true;
    }
    if (exponent > 63 - 14)
    {   //  Positive infinity
        env.overflow = true;
    }
    return (static_cast<uint64_t>(sign) << 63) |
           (static_cast<uint64_t>((exponent + 64 + 14) & 0x7F) << 56) |
           mantissa;
}

//////////
//  Implementation (32 bits)
uint32_t hadesvm::ibmhfp::add32(uint32_t a, uint32_t b, Environment & env)
{
    return static_cast<uint32_t>(add64(static_cast<uint64_t>(a) << 32, static_cast<uint64_t>(b) << 32, env) >> 32);
}

uint32_t hadesvm::ibmhfp::subtract32(uint32_t a, uint32_t b, Environment & env)
{
    return static_cast<uint32_t>(subtract64(static_cast<uint64_t>(a) << 32, static_cast<uint64_t>(b) << 32, env) >> 32);
}

uint32_t hadesvm::ibmhfp::multiply32(uint32_t a, uint32_t b, Environment & env)
{
    return static_cast<uint32_t>(multiply64(static_cast<uint64_t>(a) << 32, static_cast<uint64_t>(b) << 32, env) >> 32);
}

uint32_t hadesvm::ibmhfp::divide32(uint32_t a, uint32_t b, Environment & env)
{
    return static_cast<uint32_t>(divide64(static_cast<uint64_t>(a) << 32, static_cast<uint64_t>(b) << 32, env) >> 32);
}

int hadesvm::ibmhfp::compare32(uint32_t a, uint32_t b, Environment & env)
{
    return compare64(static_cast<uint64_t>(a) << 32, static_cast<uint64_t>(b) << 32, env);
}

//////////
//  API (64 bits)
uint64_t hadesvm::ibmhfp::add64(uint64_t a, uint64_t b, Environment & env)
{
    int aSign = static_cast<int>((a >> 63) & 0x01);
    int bSign = static_cast<int>((b >> 63) & 0x01);
    int cSign;
    int aExponent = static_cast<int>((a >> 56) & 0x7F) - 64 - 14;
    int bExponent = static_cast<int>((b >> 56) & 0x7F) - 64 - 14;
    int cExponent;
    uint64_t aMantissa = (a & 0x00FFFFFFFFFFFFFF);
    uint64_t bMantissa = (b & 0x00FFFFFFFFFFFFFF);
    uint64_t cMantissa;
    //  a == +/- aMantissa * 16**aExponent
    //  b == +/- bMantissa * 16**bExponent
    //  c == +/- cMantissa * 16**cExponent

    //  Handle zeros
    if (aMantissa == 0)
    {
        cSign = bSign;
        cExponent = bExponent;
        cMantissa = bMantissa;
    }
    else if (bMantissa == 0)
    {
        cSign = aSign;
        cExponent = aExponent;
        cMantissa = aMantissa;
    }
    else
    {   //  General case - equalize exponents
        while (aExponent > bExponent)
        {
            bMantissa >>= 4;
            bExponent++;
        }
        while (aExponent < bExponent)
        {
            aMantissa >>= 4;
            aExponent++;
        }
        cExponent = aExponent;
        //  ...and calculate result
        if (aSign == bSign)
        {   //  Adding values of the same sign
            cSign = aSign;
            cExponent = aExponent;
            cMantissa = aMantissa + bMantissa;
        }
        else if (aMantissa >= bMantissa)
        {   //  Keep sign of A
            cSign = aSign;
            cMantissa = aMantissa - bMantissa;
        }
        else
        {   //  Reverse sign of A
            cSign = aSign ^ 1;
            cMantissa = bMantissa - aMantissa;
        }
    }

    //  Create & return result
    return _constructValue(cSign, cExponent, cMantissa, env);
}

uint64_t hadesvm::ibmhfp::subtract64(uint64_t a, uint64_t b, Environment & env)
{
    return add64(a, b ^ 0x8000000000000000, env);
}

uint64_t hadesvm::ibmhfp::multiply64(uint64_t a, uint64_t b, Environment & env)
{
    int aSign = static_cast<int>((a >> 63) & 0x01);
    int bSign = static_cast<int>((b >> 63) & 0x01);
    int cSign;
    int aExponent = static_cast<int>((a >> 56) & 0x7F) - 64 - 14;
    int bExponent = static_cast<int>((b >> 56) & 0x7F) - 64 - 14;
    int cExponent;
    uint64_t aMantissa = (a & 0x00FFFFFFFFFFFFFF);
    uint64_t bMantissa = (b & 0x00FFFFFFFFFFFFFF);
    uint64_t cMantissa;
    //  a == +/- aMantissa * 16**aExponent
    //  b == +/- bMantissa * 16**bExponent
    //  c == +/- cMantissa * 16**cExponent

    //  Handle zeros
    if (aMantissa == 0 || bMantissa == 0)
    {
        return 0;
    }

    //  Multiply mantissas
    cMantissa = 0;
    for (int i = 0; i < 14; i++)
    {
        cMantissa += aMantissa * ((bMantissa >> 52) & 0x0F);
        aMantissa >>= 4;
        bMantissa <<= 4;
    }
    cSign = aSign ^ bSign;
    cExponent = aExponent + bExponent + 13;

    //  Create & return result
    return _constructValue(cSign, cExponent, cMantissa, env);
}

uint64_t hadesvm::ibmhfp::divide64(uint64_t a, uint64_t b, Environment & env)
{
    int aSign = static_cast<int>((a >> 63) & 0x01);
    int bSign = static_cast<int>((b >> 63) & 0x01);
    int cSign;
    int aExponent = static_cast<int>((a >> 56) & 0x7F) - 64 - 14;
    int bExponent = static_cast<int>((b >> 56) & 0x7F) - 64 - 14;
    int cExponent;
    uint64_t aMantissa = (a & 0x00FFFFFFFFFFFFFF);
    uint64_t bMantissa = (b & 0x00FFFFFFFFFFFFFF);
    uint64_t cMantissa, digit;
    //  a == +/- aMantissa * 16**aExponent
    //  b == +/- bMantissa * 16**bExponent
    //  c == +/- cMantissa * 16**cExponent

    //  Handle zeros
    if (aMantissa == 0)
    {
        return 0;
    }
    if (bMantissa == 0)
    {
        env.divide = true;
        return 0;
    }

    //  Divide mantissas
    cMantissa = 0;
    for (int i = 0; i < 14; i++)
    {
        digit = aMantissa / bMantissa;
        cMantissa = (cMantissa << 4) | digit;
        aMantissa -= bMantissa * digit;
        aMantissa = (aMantissa << 4) & 0x0FFFFFFFFFFFFFFF;
    }
    cSign = aSign ^ bSign;
    cExponent = aExponent - bExponent - 13;

    //  Create & return result
    return _constructValue(cSign, cExponent, cMantissa, env);
}

int hadesvm::ibmhfp::compare64(uint64_t a, uint64_t b, Environment & env)
{
    uint64_t diff = subtract64(a, b, env);
    if (diff == 0)
    {
        return 0;
    }
    else if ((diff & 0x8000000000000000) != 0)
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

uint64_t hadesvm::ibmhfp::fromInt64(int a)
{
    int cSign;
    int cExponent;
    uint64_t cMantissa;

    //  0 is a special case
    if (a == 0)
    {
        return 0;
    }
    if (a > 0)
    {   //  Positive
        cSign = 0;
        cMantissa = static_cast<unsigned>(a);
    }
    else
    {   //  Negative
        cSign = 1;
        cMantissa = static_cast<unsigned>(-a);
    }
    cExponent = 0;

    //  Normalize result
    while ((cMantissa & 0x00F0000000000000) == 0)
    {
        cMantissa <<= 4;
        cExponent--;
    }

    //  Form the result
    return (static_cast<uint64_t>(cSign) << 63) |
           (static_cast<uint64_t>((cExponent + 64 + 14) & 0x7F) << 56) |
           cMantissa;
}

uint32_t hadesvm::ibmhfp::fromInt32(int a)
{
    return static_cast<uint32_t>(fromInt64(a) >> 32);
}

double hadesvm::ibmhfp::toDouble32(uint32_t a)
{
    return toDouble64(static_cast<uint64_t>(a) << 32);
}

double hadesvm::ibmhfp::toDouble64(uint64_t a)
{
    int aSign = static_cast<int>((a >> 63) & 0x01);
    int aExponent = static_cast<int>((a >> 56) & 0x7F) - 64 - 14;
    uint64_t aMantissa = (a & 0x00FFFFFFFFFFFFFF);
    //  a == +/- aMantissa * 16**aExponent
    double result;

    result = static_cast<double>(aMantissa);
    for (; aExponent > 0; aExponent--)
        result *= 16;
    for (; aExponent < 0; aExponent++)
        result /= 16;
    return aSign ? -result : result;
}

//  End of hadesvm-ibmhfp/uint32_tHfp.cpp
