//
//  hadesvm-ieee754/FpValue.cpp
//
//  hadesvm::ieee754::FpValue class implementation
//
//////////
#include "hadesvm-ieee754/API.hpp"
#include "hadesvm-ieee754/FpValue.hxx"
using namespace hadesvm::ieee754;

//////////
//  Construction/destruction/assignment
FpValue::FpValue()
    :   isSignalingNan(false),
        isQuietNan(false),
        isInfinity(false),
        signBit(0),
        exponent(0),
        mantissa(0)
{
}

FpValue::FpValue(bool isSignalingNanParam, bool isQuietNanParam, bool isInfinityParam,
                 uint8_t signBitParam, int16_t exponentParam, uint64_t mantissaParam)
    :   isSignalingNan(isSignalingNanParam),
        isQuietNan(isQuietNanParam),
        isInfinity(isInfinityParam),
        signBit(signBitParam),
        exponent(exponentParam),
        mantissa(mantissaParam)
{
    Q_ASSERT(signBit == 0 || signBit == 1);

    //  Normalize
    if (!isSignalingNan && !isQuietNan && !isInfinity)
    {   //  The value is not a special value
        if (mantissa == 0)
        {   //  Zero
            exponent = 0;
        }
        else
        {   //  Normalize the mantissa
            while ((mantissa & UINT64_C(0x8000000000000000)) == 0)
            {
                mantissa <<= 1;
                exponent--;
            }
        }
    }
}

FpValue::FpValue(double value)
    :   isSignalingNan(false),
        isQuietNan(false),
        isInfinity(false),
        signBit(0),
        exponent(0),
        mantissa(0)
{
    if (hadesvm::util::Math::isSignalingNan(value))
    {
        *this = FpValue(true, false, false, 0, 0, UINT64_C(0x4000000000000000));
    }
    else if (hadesvm::util::Math::isNan(value))
    {
        *this = FpValue(true, false, false, 0, 0, UINT64_C(0x8000000000000000));
    }
    else if (hadesvm::util::Math::isPositiveInfinite(value))
    {
        *this = FpValue(false, false, true, 0, 0, 0);
    }
    else if (hadesvm::util::Math::isNegativeInfinite(value))
    {
        *this = FpValue(false, false, true, 1, 0, 0);
    }
    else
    {   //  Value is finite - extract the sign bit...
        if (value < 0)
        {
            signBit = 0;
            value = -value;
        }
        //  ...scale to range [0.5 .. 1)...
        while (value < 0.5)
        {
            value *= 2;
            exponent--;
        }
        while (value >= 1.0)
        {
            value /= 2;
            exponent++;
        }
        //  ...and extract 64 bits of mantissa
        for (int i = 0; i < 64; i++)
        {
            value *= 2;
            int digit = (value >= 1.0) ? 1 : 0;
            value -= digit;
            mantissa = (mantissa << 1) | digit;
        }
    }
}

//////////
//  Operations
void FpValue::equalizeExponents(Environment & environment, FpValue & x, FpValue & y)
{
    int16_t shiftCount;
    uint64_t shiftedOutBitsMask;

    if (x.exponent != y.exponent)
    {
        if (x.exponent > y.exponent)
        {   //  Reverse operands
            equalizeExponents(environment, y, x);
        }
        else
        {   /*  The 1st operand must be shifted */
            shiftCount = static_cast<int16_t>(y.exponent - x.exponent);
            if (shiftCount >= 64)
            {   //  All bits of "x" will be shifted out - result is +0.0
                if (x.exponent != 0 || x.mantissa != 0)
                {   //  ...but "x" was NOT +0.0 before shift
                    environment.exception |= Exception::Inexact;
                }
                x.exponent = y.exponent;
                x.mantissa = 0;
            }
            else
            {   //  Some bits of "x" will be left in
                shiftedOutBitsMask = UINT64_C(0xFFFFFFFFFFFFFFFF) >> (64 - shiftCount);
                if ((x.mantissa & shiftedOutBitsMask) != 0)
                {   //  Some "1" bits will be shifted out
                    environment.exception |= Exception::Inexact;
                }
                x.mantissa >>= shiftCount;
                x.exponent = static_cast<int16_t>(x.exponent + shiftCount);
            }
        }
    }
}

FpValue FpValue::addMagnitudes(Environment & environment, const FpValue & x, const FpValue & y)
{
    uint64_t sum1, sum2;

    Q_ASSERT(!x.isSignalingNan && !x.isQuietNan && !x.isInfinity);
    Q_ASSERT(!y.isSignalingNan && !y.isQuietNan && !y.isInfinity);
    Q_ASSERT(x.exponent == y.exponent);

    sum1 = x.mantissa + y.mantissa;
    sum2 = (x.mantissa >> 1) + (y.mantissa >> 1);
    if ((sum2 & UINT64_C(0x8000000000000000)) != 0)
    {   //  Adding magnitudes creates a 1-bit overflow!
        if ((sum1 & 0x01) != 0)
        {   //  ...and we're losing a lowest bit of the result
            environment.exception |= Exception::Inexact;
        }
        return FpValue(false, false, false, x.signBit, static_cast<int16_t>(x.exponent + 1), sum2);
    }
    else
    {   //  Adding magnitudes creates no overflow
        return FpValue(false, false, false, x.signBit, x.exponent, sum1);
    }
}

FpValue FpValue::subtractMagnitudes(const FpValue & x, const FpValue & y)
{
    Q_ASSERT(!x.isSignalingNan && !x.isQuietNan && !x.isInfinity);
    Q_ASSERT(!y.isSignalingNan && !y.isQuietNan && !y.isInfinity);
    Q_ASSERT(x.exponent == y.exponent);

    if (x.mantissa >= y.mantissa)
    {   //  1st operand's magnitude is larger than that of the 2nd operand, keep the sign
        return FpValue(false, false, false, x.signBit, x.exponent, x.mantissa - y.mantissa);
    }
    else
    {   //  1st operand's magnitude is smaller than that of the 2nd operand, change the sign
        return FpValue(false, false, false, static_cast<uint8_t>(1 - x.signBit), x.exponent, y.mantissa - x.mantissa);
    }
}

FpValue FpValue::parse64(bool & denormalized, uint64_t value)
{
    uint8_t signBit;
    int16_t exponent;
    uint64_t mantissa;

    signBit = static_cast<uint8_t>(value >> 63);
    exponent = static_cast<int16_t>((value >> 52) & 0x07FF);
    mantissa = value & UINT64_C(0x000FFFFFFFFFFFFF);
    denormalized = false;
    //  Build the value
    if (exponent == 0 && mantissa == 0)
    {   //  +0.0 or -0.0 both become 0
        return FpValue(false, false, false, 0, 0, 0);
    }
    else if (exponent == 0 && mantissa != 0)
    {   //  Denormalized
        denormalized = true;
        exponent = -1023;
        mantissa <<= 12;
        return FpValue(false, false, false, signBit, exponent, mantissa);
    }
    else if (exponent != 2047)
    {   //  Normalized
        exponent = static_cast<int16_t>(exponent - 1023);
        mantissa |= UINT64_C(0x0010000000000000);
        return FpValue(false, false, false, signBit, static_cast<int16_t>(exponent + 1), mantissa << 11);
    }
    else if (mantissa == 0)
    {   //  Infinity
        return FpValue(false, false, true, signBit, 0, 0);
    }
    else if ((mantissa & UINT64_C(0x0008000000000000)) != 0)
    {   //  Quiet NaN
        return FpValue(false, true, false, signBit, 0, mantissa << 12);
    }
    else
    {   //  Signaling NaN
        return FpValue(true, false, false, signBit, 0, mantissa << 12);
    }
}

FpValue FpValue::parse32(bool & denormalized, uint32_t value)
{
    uint8_t signBit;
    int16_t exponent;
    uint64_t mantissa;

    signBit = static_cast<uint8_t>(value >> 31);
    exponent = static_cast<int16_t>((value >> 23) & 0x00FF);
    mantissa = (value & 0x007FFFFF);
    denormalized = false;
    //  Build the value
    if (exponent == 0 && mantissa == 0)
    {   //  +0.0 and -0.0 both become 0
        return FpValue(false, false, false, 0, 0, 0);
    }
    else if (exponent == 0 && mantissa != 0)
    {   //  Denormalized
        denormalized = true;
        exponent = -127;
        mantissa <<= 9 + 32;
        return FpValue(false, false, false, signBit, exponent, mantissa);
    }
    else if (exponent != 255)
    {   //  Normalized
        exponent = static_cast<int16_t>(exponent - 127);
        mantissa |= UINT64_C(0x0000000000800000);
        return FpValue(false, false, false, signBit, static_cast<int16_t>(exponent + 1), mantissa << 40);
    }
    else if (mantissa == 0)
    {   //  Infinity
        return FpValue(false, false, true, signBit, 0, 0);
    }
    else if ((mantissa & UINT64_C(0x0000000000400000)) != 0)
    {   //  Quiet NaN
        return FpValue(false, true, false, signBit, 0, mantissa << 41);
    }
    else
    {   //  Signaling NaN
        return FpValue(true, false, false, signBit, 0, mantissa << 41);
    }
}

FpValue FpValue::parse21(bool & denormalized, uint32_t value)
{
    uint8_t signBit;
    int16_t exponent;
    uint64_t mantissa;

    signBit = static_cast<uint8_t>(value >> 20);
    exponent = static_cast<int16_t>((value >> 14) & 0x003F);
    mantissa = (value & 0x00003FFF);
    denormalized = false;
    /*  Build the value */
    if (exponent == 0 && mantissa == 0)
    {   //  +0.0 and -0.0 both become 0
        return FpValue(false, false, false, 0, 0, 0);
    }
    else if (exponent == 0 && mantissa != 0)
    {   //  Denormalized
        denormalized = true;
        exponent = -31;
        mantissa <<= 16 + 32;
        return FpValue(false, false, false, signBit, exponent, mantissa);
    }
    else if (exponent != 63)
    {   //  Normalized
        exponent = static_cast<int16_t>(exponent - 31);
        mantissa |= UINT64_C(0x0000000000004000);
        return FpValue(false, false, false, signBit, static_cast<int16_t>(exponent + 1), mantissa << 49);
    }
    else if (mantissa == 0)
    {   //  Infinity
        return FpValue(false, false, true, signBit, 0, 0);
    }
    else if ((mantissa & UINT64_C(0x0000000000002000)) != 0)
    {   //  Quiet NaN
        return FpValue(false, true, false, signBit, 0, mantissa << 50);
    }
    else
    {   //  Signaling NaN
        return FpValue(true, false, false, signBit, 0, mantissa << 50);
    }
}

uint64_t FpValue::build64(Environment & environment) const
{
    if (isSignalingNan)
    {
        return (static_cast<uint64_t>(signBit) << 63) |
               UINT64_C(0x7FF0000000000000) |
               ((mantissa >> 12) & UINT64_C(0xFFF7FFFFFFFFFFFF));
    }
    else if (isQuietNan)
    {
        return (static_cast<uint64_t>(signBit) << 63) |
               UINT64_C(0x7FF0000000000000) |
               ((mantissa >> 12) | UINT64_C(0x0008000000000000));
    }
    else if (isInfinity)
    {
        return (static_cast<uint64_t>(signBit) << 63) |
               UINT64_C(0x7FF0000000000000);
    }
    else if (mantissa == 0)
    {
        return 0;
    }
    else if (exponent > 1024)
    {   //  Overflow; result is an infinity
        environment.exception |= Exception::Overflow;
        return (static_cast<uint64_t>(signBit) << 63) |
               UINT64_C(0x7FF0000000000000);
    }
    else if (exponent >= -1021)
    {   //  Can represent value in normalized form
        uint64_t resultExponent = static_cast<uint64_t>(exponent + 1023 - 1);
        if ((mantissa & UINT64_C(0x00000000000007FF)) != 0)
        {
            environment.exception |= Exception::Inexact;
        }
        return (static_cast<uint64_t>(signBit) << 63) |
               (resultExponent << 52) |
               ((mantissa >> 11) & UINT64_C(0x000FFFFFFFFFFFFF));
    }
    else if (exponent >= -1022 - 52)
    {   //  Can represent value in denormalized form
        int16_t resultExponent = static_cast<int16_t>(exponent + 1023 - 1);
        int shiftCount = -resultExponent + 11;
        uint64_t shiftedOutBitsMask = UINT64_C(0xFFFFFFFFFFFFFFFF) >> (64 - shiftCount);
        if ((mantissa & shiftedOutBitsMask) != 0)
        {
            environment.exception |= Exception::Inexact;
        }
        return (static_cast<uint64_t>(signBit) << 63) |
               ((mantissa >> (shiftCount)) & UINT64_C(0x000FFFFFFFFFFFFF));
    }
    else
    {   //  Underflow
        environment.exception |= Exception::Underflow;
        return 0;
    }
}

uint32_t FpValue::build32(Environment & environment) const
{
    if (isSignalingNan)
    {
        return (static_cast<uint32_t>(signBit) << 31) |
               0x7F800000 |
               (static_cast<uint32_t>(mantissa >> (9 + 32)) & 0xFFBFFFFF);
    }
    else if (isQuietNan)
    {
        return (static_cast<uint32_t>(signBit) << 31) |
               0x7F800000 |
               (static_cast<uint32_t>(mantissa >> (9 + 32)) | 0x00400000);
    }
    else if (isInfinity)
    {
        return (static_cast<uint32_t>(signBit) << 31) |
               0x7F800000;
    }
    else if (mantissa == 0)
    {
        return 0;
    }
    else if (exponent > 128)
    {   //  Overflow; result is an infinity
        environment.exception |= Exception::Overflow;
        return (static_cast<uint32_t>(signBit) << 31) |
               0x7F800000;
    }
    else if (exponent >= -125)
    {   //  Can represent value in normalized form
        uint32_t resultExponent = static_cast<uint32_t>(exponent + 127 - 1);
        if ((mantissa & UINT64_C(0x000000FFFFFFFFFF)) != 0)
        {
            environment.exception |= Exception::Inexact;
        }
        return (static_cast<uint32_t>(signBit) << 31) |
               (resultExponent << 23) |
               (static_cast<uint32_t>(mantissa >> 40) & 0x007FFFFF);
    }
    else if (exponent >= -126 - 23)
    {   //  Can represent value in denormalized form
        int16_t resultExponent = static_cast<int16_t>(exponent + 127 - 1);
        int shiftCount = -resultExponent + 8 + 32;
        uint64_t shiftedOutBitsMask = UINT64_C(0xFFFFFFFFFFFFFFFF) >> (64 - shiftCount);
        if ((mantissa & shiftedOutBitsMask) != 0)
        {
            environment.exception |= Exception::Inexact;
        }
        return (static_cast<uint32_t>(signBit) << 31) |
               (static_cast<uint32_t>(mantissa >> (shiftCount)) & 0x007FFFFF);
    }
    else
    {   //  Underflow
        environment.exception |= Exception::Underflow;
        return 0;
    }
}

uint32_t FpValue::build21(Environment & environment) const
{
    if (isSignalingNan)
    {
        return (static_cast<uint32_t>(signBit) << 20) |
               0x000FC000 |
               (static_cast<uint32_t>(mantissa >> (18 + 32)) & 0xFFFFDFFF);
    }
    else if (isQuietNan)
    {
        return (static_cast<uint32_t>(signBit) << 20) |
               0x000FC000 |
               (static_cast<uint32_t>(mantissa >> (18 + 32)) | 0x00002000);
    }
    else if (isInfinity)
    {
        return (static_cast<uint32_t>(signBit) << 20) |
               0x000FC000;
    }
    else if (exponent == 0 && mantissa == 0)
    {
        return 0;
    }
    else if (exponent > 32)
    {   //  Overflow; result is an infinity
        environment.exception |= Exception::Overflow;
        return (static_cast<uint32_t>(signBit) << 20) |
               0x000FC000;
    }
    else if (exponent >= -29)
    {   //  Can represent value in normalized form
        uint32_t resultExponent = static_cast<uint32_t>(exponent + 31 - 1);
        if ((mantissa & UINT64_C(0x0001FFFFFFFFFFFF)) != 0)
        {
            environment.exception |= Exception::Inexact;
        }
        return (static_cast<uint32_t>(signBit) << 20) |
               (resultExponent << 14) |
               (static_cast<uint32_t>(mantissa >> 49) & 0x00003FFF);
    }
    else if (exponent >= -30 - 14)
    {   //  Can represent value in denormalized form
        int16_t resultExponent = static_cast<int16_t>(exponent + 31 - 1);
        int shiftCount = -resultExponent + 6 + 11 + 32;
        uint64_t shiftedOutBitsMask = UINT64_C(0xFFFFFFFFFFFFFFFF) >> (64 - shiftCount);
        if ((mantissa & shiftedOutBitsMask) != 0)
        {
            environment.exception |= Exception::Inexact;
        }
        return (static_cast<uint32_t>(signBit) << 20) |
               (static_cast<uint32_t>(mantissa >> (shiftCount)) & 0x00003FFF);
    }
    else
    {   //  Underflow
        environment.exception |= Exception::Underflow;
        return 0;
    }
}

FpValue FpValue::add(Environment & environment, const FpValue & x, const FpValue & y)
{
    if (x.isSignalingNan)
    {   //  Operand #1 is a signaling NaN
        environment.exception |= Exception::Operand;
        return x;
    }
    else if (y.isSignalingNan)
    {   //  Operand #2 is a signaling NaN
        environment.exception |= Exception::Operand;
        return y;
    }
    else if (x.isQuietNan)
    {   //  Operand #1 is a quiet NaN
        return x;
    }
    else if (y.isQuietNan)
    {   //  Operand #2 is a quiet NaN
        return y;
    }
    else if (x.isInfinity)
    {   //  Operand #1 is an infinity
        if (y.isInfinity)
        {   //  Operand #2 is an infinity as well
            if (x.signBit == y.signBit)
            {   //  Adding two infinities of the same sign
                return x;
            }
            else
            {   //  Adding two infinities of different signs
                return FpValue(false, true, false, 0, 0, UINT64_C(0x8000000000000000));
            }
        }
        else
        {   //  Operand #2 is finite
            return x;
        }
    }
    else if (y.isInfinity)
    {   //  Operand #1 is finite; operand #2 is an infinity
        return y;
    }
    else
    {   //  Both operands are finite
        if (x.exponent == 0 && x.mantissa == 0)
        {   //  Operand #1 is +0.0
            return y;
        }
        else if (y.exponent == 0 && y.mantissa == 0)
        {   //  Operand #2 is +0.0
            return x;
        }
        else
        {   /*  Both operands are finite and not zero */
            FpValue preparedX = x, preparedY = y;
            equalizeExponents(environment, preparedX, preparedY);
            if (x.signBit == y.signBit)
            {   //  Adding 2 operands of the same sign
                return addMagnitudes(environment, preparedX, preparedY);
            }
            else
            {   //  Adding 2 operands of different signs
                return subtractMagnitudes(preparedX, preparedY);
            }
        }
    }
}

FpValue FpValue::subtract(Environment & environment, const FpValue & x, const FpValue & y)
{
    if (x.isSignalingNan)
    {   //  Operand #1 is a signaling NaN
        environment.exception |= Exception::Operand;
        return x;
    }
    else if (y.isSignalingNan)
    {   //  Operand #2 is a signaling NaN
        environment.exception |= Exception::Operand;
        return y;
    }
    else if (x.isQuietNan)
    {   //  Operand #1 is a quiet NaN
        return x;
    }
    else if (y.isQuietNan)
    {   //  Operand #2 is a quiet NaN
        return y;
    }
    else if (x.isInfinity)
    {   //  Operand #1 is an infinity
        if (y.isInfinity)
        {   //  Operand #1 is an infinity as well
            if (x.signBit != y.signBit)
            {   //  Subtracting two infinities of different signs
                return x;
            }
            else
            {   //  Subtracting two infinities of the same sign
                return FpValue(false, true, false, 0, 0, UINT64_C(0x8000000000000000));
            }
        }
        else
        {   //  Operand #2 is finite
            return x;
        }
    }
    else if (y.isInfinity)
    {   //  Operand #1 is finite; operand #2 is an infinity
        return FpValue(false, false, true, static_cast<uint8_t>(1 - y.signBit), 0, 0);
    }
    else
    {   //  Both operands are finite.
        if (y.exponent == 0 && y.mantissa == 0)
        {   //  Operand #2 is +0.0
            return x;
        }
        else if (x.exponent == 0 && x.mantissa == 0)
        {   //  Operand #1 is +0.0, Operand #2 is NOT 0
            FpValue result = y;
            result.signBit = static_cast<uint8_t>(1 - result.signBit);
            return result;
        }
        else
        {   //  Both operands are finite and not zero
            FpValue preparedX = x, preparedY = y;
            equalizeExponents(environment, preparedX, preparedY);
            if (x.signBit == y.signBit)
            {   //  Subtracting 2 operands of the same sign */
                return subtractMagnitudes(preparedX, preparedY);
            }
            else
            {   //  Subtracting 2 operands of different signs
                return addMagnitudes(environment, preparedX, preparedY);
            }
        }
    }
}

FpValue FpValue::multiply(Environment & environment, const FpValue & x, const FpValue & y)
{
    uint64_t partialProducts[4];
    int16_t resultExponent;
    int i;

    if (x.isSignalingNan)
    {   //  Operand #1 is a signaling NaN
        environment.exception |= Exception::Operand;
        return x;
    }
    else if (y.isSignalingNan)
    {   //  Operand #2 is a signaling NaN
        environment.exception |= Exception::Operand;
        return y;
    }
    else if (x.isQuietNan)
    {   //  Operand #1 is a quiet NaN
        return x;
    }
    else if (y.isQuietNan)
    {   //  Operand #2 is a quiet NaN
        return y;
    }
    else if (x.isInfinity)
    {   //  Operand #1 is an infinity
        if (y.isInfinity)
        {   //  Operand #1 is an infinity as well
            if (x.signBit == y.signBit)
            {   //  Multiplying two infinities of the same sign
                return x;
            }
            else
            {   //  Multiplying two infinities of different signs
                return FpValue(false, true, false, 0, 0, UINT64_C(0x8000000000000000));
            }
        }
        else
        {   //  Operand #2 is finite
            if (x.signBit == y.signBit)
            {   //  Multiplying an infinity by a finite value of the same sign
                return x;
            }
            else
            {   //  Multiplying an infinity by a finite value of a different sign
                return FpValue(false, false, true, static_cast<uint8_t>(1 - x.signBit), 0, 0);
            }
        }
    }
    else if (y.isInfinity)
    {   //  Operand #1 is finite; operand #2 is an infinity
        if (x.signBit == y.signBit)
        {   //  Multiplying an infinity by a finite value of the same sign
            return y;
        }
        else
        {   //  Multiplying an infinity by a finite value of a different sign
            return FpValue(false, false, true, static_cast<uint8_t>(1 - y.signBit), 0, 0);
        }
    }
    else if (x.exponent == 0 && x.mantissa == 0)
    {   //  Operand #1 is +0.0
        return FpValue(false, false, false, 0, 0, 0);
    }
    else if (y.exponent == 0 && y.mantissa == 0)
    {   //  Operand #2 is +0.0
        return FpValue(false, false, false, 0, 0, 0);
    }
    else
    {   //  Both operands are finite and not zero
        partialProducts[0] = 0;
        partialProducts[1] = (x.mantissa >> 32) * (y.mantissa >> 32);
        partialProducts[2] = (x.mantissa & UINT64_C(0x00000000FFFFFFFF)) * (y.mantissa >> 32) +
                             (y.mantissa & UINT64_C(0x00000000FFFFFFFF)) * (x.mantissa >> 32);
        partialProducts[3] = (x.mantissa & UINT64_C(0x00000000FFFFFFFF)) * (y.mantissa & UINT64_C(0x00000000FFFFFFFF));
        for (i = 3; i > 0; i--)
        {
            if ((partialProducts[i] & UINT64_C(0xFFFFFFFF00000000)) != 0)
            {
                partialProducts[i - 1] += (partialProducts[i] >> 32);
                partialProducts[i] &= UINT64_C(0x00000000FFFFFFFF);
            }
        }
        resultExponent = static_cast<int16_t>(x.exponent + y.exponent);
        return FpValue(false, false, false, static_cast<uint8_t>((x.signBit == y.signBit) ? 0 : 1), resultExponent, (partialProducts[0] << 32) | partialProducts[1]);
    }
}

FpValue FpValue::divide(Environment & environment, const FpValue & x, const FpValue & y)
{
    uint64_t dividend, divisor, resultMantissa;
    int16_t resultExponent;
    int i, bit;

    if (x.isSignalingNan)
    {   //  Operand #1 is a signaling NaN
        environment.exception |= Exception::Operand;
        return x;
    }
    else if (y.isSignalingNan)
    {   //  Operand #2 is a signaling NaN
        environment.exception |= Exception::Operand;
        return y;
    }
    else if (x.isQuietNan)
    {   //  Operand #1 is a quiet NaN
        return x;
    }
    else if (y.isQuietNan)
    {   //  Operand #2 is a quiet NaN
        return y;
    }
    else if (x.isInfinity)
    {   //  Operand #1 is an infinity
        if (y.isInfinity)
        {   //  Operand #1 is an infinity as well
            return FpValue(false, true, false, 0, 0, UINT64_C(0x8000000000000000));
        }
        else if (y.exponent == 0 && y.mantissa == 0)
        {   //  Dividing infinity by +0.0
            environment.exception |= Exception::DivisionByZero;
            return x;
        }
        else
        {   //  Operand #2 is finite
            if (x.signBit == y.signBit)
            {   //  Dividing an infinity by a finite value of the same sign
                return x;
            }
            else
            {   //  Dividing an infinity by a finite value of a different sign
                return FpValue(false, false, true, static_cast<uint8_t>(1 - x.signBit), 0, 0);
            }
        }
    }
    else if (y.isInfinity)
    {   //  Operand #1 is finite; operand #2 is an infinity - the result is always 0
        return FpValue(false, false, false, 0, 0, 0);
    }
    else if (x.exponent == 0 && x.mantissa == 0)
    {   //  Operand #1 is +0.0
        return FpValue(false, false, false, 0, 0, 0);
    }
    else if (y.exponent == 0 && y.mantissa == 0)
    {   //  Operand #2 is +0.0
        environment.exception |= Exception::DivisionByZero;
        return FpValue(false, false, false, 0, 0, 0);
    }
    else
    {   //  Both operands are finite and not zero
        dividend = x.mantissa;
        divisor = y.mantissa;
        resultMantissa = 0;
        for (i = 0; i < 64; i++)
        {
            bit = 0;
            if (dividend >= divisor)
            {
                dividend -= divisor;
                bit = 1;
            }
            resultMantissa = (resultMantissa << 1) | bit;
            if (i == 0)
            {
                divisor >>= 1;
            }
            else
            {
                dividend <<= 1;
            }
        }
        resultExponent = static_cast<int16_t>(x.exponent - y.exponent + 1);
        return FpValue(false, false, false, static_cast<uint8_t>((x.signBit == y.signBit) ? 0 : 1), resultExponent, resultMantissa);
    }
}

FpValue FpValue::negate(Environment & environment, const FpValue & x)
{
    if (x.isSignalingNan)
    {   //  Operand is a signaling NaN
        environment.exception |= Exception::Operand;
        return x;
    }
    else if (x.isQuietNan)
    {   //  Operand is a quiet NaN
        return x;
    }
    else if (x.isInfinity)
    {   //  Operand is an infinity
        return FpValue(false, false, true, static_cast<uint8_t>(1 - x.signBit), 0, 0);
    }
    else
    {   //  Operand is finite
        return FpValue(false, false, true, static_cast<uint8_t>(1 - x.signBit), x.exponent, x.mantissa);
    }
}

int FpValue::compare(Environment & environment, const FpValue & x, const FpValue & y)
{
    if (x.isSignalingNan || y.isSignalingNan)
    {   //  One of the operands is a signaling NaN
        environment.exception |= Exception::Operand;
        return 0;
    }
    else if (x.isQuietNan || y.isQuietNan)
    {   //  One of the operands is a quiet NaN
        return 0;
    }
    else if (x.isInfinity && y.isInfinity)
    {   //  Both operands are infinities
        return static_cast<int>(y.signBit) - static_cast<int>(x.signBit);
    }
    else
    {   //  Both operands are finite
        FpValue difference = subtract(environment, x, y);
        if (difference.exponent == 0 && difference.mantissa == 0)
        {   //  The difference is +0.0
            return 0;
        }
        else if (difference.signBit == 0)
        {   //  The difference is positive
            return 1;
        }
        else
        {   //  The difference is negative
            return -1;
        }
    }
}

uint64_t FpValue::toUnsignedInteger(Environment & environment, const FpValue & x)
{
    if (x.isSignalingNan)
    {   /*  NaN -> 0    */
        environment.exception |= Exception::Operand;
        return 0;
    }
    else if (x.isQuietNan)
    {   //  NaN -> 0
        return 0;
    }
    else if (x.isInfinity)
    {   //  -inf -> 0, +inf -> MAX
        environment.exception |= Exception::Overflow;
        return x.signBit ? 0 : UINT64_MAX;
    }
    else if (x.mantissa == 0)
    {   //  0
        return 0;
    }
    else if (x.signBit != 0)
    {   //  negative -> 0
        environment.exception |= Exception::Overflow;
        return 0;
    }
    else
    {   //  A positive nonzero number
        uint64_t result = x.mantissa;
        int16_t shift = static_cast<int16_t>(64 - x.exponent);

        for (; shift > 0; shift--)
        {
            uint64_t newResult = result >> 1;
            if ((newResult << 1) != result)
            {   //  Least significant bit is lost
                environment.exception |= Exception::Inexact;
            }
            result = newResult;
        }
        for (; shift < 0; shift++)
        {
            uint64_t newResult = result << 1;
            if ((newResult >> 1) != result)
            {   //  Most significant bit is lost
                environment.exception |= Exception::Overflow;
            }
            result = newResult;
        }
        //  Done
        return result;
    }
}

int64_t FpValue::toSignedInteger(Environment & environment, const FpValue & x)
{
    if (x.isSignalingNan)
    {   //  NaN -> 0
        environment.exception |= Exception::Operand;
        return 0;
    }
    else if (x.isQuietNan)
    {   //  NaN -> 0
        return 0;
    }
    else if (x.isInfinity)
    {   //  -inf -> MIN, +inf -> MAX  */
        environment.exception |= Exception::Overflow;
        return x.signBit ? INT64_MIN : INT64_MAX;
    }
    else if (x.mantissa == 0)
    {   //  0
        return 0;
    }
    else if (x.signBit != 0)
    {   //  Negative nonzero - negate...
        FpValue minusValue = x;
        minusValue.signBit = 0;
        //  ...convert as unsigned...
        int64_t result = -(static_cast<int64_t>(toUnsignedInteger(environment, minusValue)));
        //  ...and bring to range. As the original value was negative, we
        //  have an overflow if the result is positive at this point
        if (result >= 0)
        {
            environment.exception |= Exception::Overflow;
            return INT64_MIN;
        }
        return result;
    }
    else
    {   //  Positive nonzero - convert as unsigned...
        uint64_t result = toUnsignedInteger(environment, x);
        //  ...and bring to range
        if (result > static_cast<uint64_t>(INT64_MAX))
        {   //  Too large!
            environment.exception |= Exception::Overflow;
            return INT64_MAX;
        }
        return static_cast<int64_t>(result);
    }
}

long double FpValue::toReal(Environment & environment, const FpValue & x)
{
    //  Not all doubles support NaNs
    if (x.isSignalingNan || x.isQuietNan)
    {
        environment.exception |= Exception::Operand;
        return 0;
    }
    //  Infinity - emulate by saturation
    else if (x.isInfinity)
    {
        long double result = 1;
        for (; ; )
        {
            long double newResult = result * 2;
            if ((newResult <= result) && (newResult >= result))
            {   /*  Don't use "==" to avoid GCC warnings. But, in essence, we are checking
                    if a value is equal to its double - and that's infinity OR saturation   */
                environment.exception |= Exception::Overflow;
                break;
            }
            result = newResult;
        }
        return (x.signBit == 0) ? result : (-result);
    }
    //  Finite - evaluate
    else
    {
        long double result = static_cast<long double>(x.mantissa);
        int32_t scale = x.exponent - 64;
        for (; scale > 0; scale--)
        {
            result *= 2;
        }
        for (; scale < 0; scale++)
        {
            result /= 2;
        }
        return result;
    }
}

ValueClass FpValue::classify(const FpValue & x, bool denormalized)
{
    if (x.isSignalingNan)
    {
        return ValueClass::SignallingNan;
    }
    else if (x.isQuietNan)
    {
        return ValueClass::QuietNan;
    }
    else if (x.isInfinity)
    {
        return (x.signBit == 0) ? ValueClass::PositiveInfinity : ValueClass::NegativeInfinity;
    }
    else if (x.mantissa == 0 && x.exponent == 0)
    {
        return ValueClass::Zero;
    }
    else if (x.signBit == 0)
    {
        return denormalized ? ValueClass::PositiveDenormalized : ValueClass::PositiveNormalized;
    }
    else
    {
        return denormalized ? ValueClass::NegativeDenormalized : ValueClass::NegativeNormalized;
    }
}

//  End of hadesvm-ieee754/FpValue.cpp
