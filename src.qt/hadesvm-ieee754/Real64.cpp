//
//  hadesvm-ieee754/Real64.cpp
//
//  hadesvm::ieee754::Real64 class implementation
//
//////////
#include "hadesvm-ieee754/API.hpp"
#include "hadesvm-ieee754/FpValue.hxx"
using namespace hadesvm::ieee754;

//////////
//  Construction/destruction/assignment
Real64::Real64(uint64_t bitPatternParam, bool /*dummy*/)
    :   bitPattern(bitPatternParam)
{
}

Real64 Real64::fromBitPattern(uint64_t bitPattern)
{
    return Real64(bitPattern, true);
}

Real64 Real64::fromInt8(int8_t value, Environment & environment)
{
    FpValue fpValue(false, false, false, static_cast<uint8_t>((value < 0) ? 1 : 0), 64, static_cast<uint64_t>((value >= 0) ? value : (-value)));
    return Real64(fpValue.build64(environment), true);
}

Real64 Real64::fromUInt8(uint8_t value, Environment & environment)
{
    FpValue fpValue(false, false, false, 0, 64, value);
    return Real64(fpValue.build64(environment), true);
}

Real64 Real64::fromInt16(int16_t value, Environment & environment)
{
    FpValue fpValue(false, false, false, static_cast<uint8_t>((value < 0) ? 1 : 0), 64, static_cast<uint64_t>((value >= 0) ? value : (-value)));
    return Real64(fpValue.build64(environment), true);
}

Real64 Real64::fromUInt16(uint16_t value, Environment & environment)
{
    FpValue fpValue(false, false, false, 0, 64, value);
    return Real64(fpValue.build64(environment), true);
}

Real64 Real64::fromInt32(int32_t value, Environment & environment)
{
    FpValue fpValue(false, false, false, static_cast<uint8_t>((value < 0) ? 1 : 0), 64, static_cast<uint64_t>((value >= 0) ? value : (-value)));
    return Real64(fpValue.build64(environment), true);
}

Real64 Real64::fromUInt32(uint32_t value, Environment & environment)
{
    FpValue fpValue(false, false, false, 0, 64, value);
    return Real64(fpValue.build64(environment), true);
}

Real64 Real64::fromInt64(int64_t value, Environment & environment)
{
    FpValue fpValue(false, false, false, static_cast<uint8_t>((value < 0) ? 1 : 0), 64, static_cast<uint64_t>((value >= 0) ? value : (-value)));
    return Real64(fpValue.build64(environment), true);
}

Real64 Real64::fromUInt64(uint64_t value, Environment & environment)
{
    FpValue fpValue(false, false, false, 0, 64, value);
    return Real64(fpValue.build64(environment), true);
}

Real64 Real64::fromFloat(float value, Environment & environment)
{
    FpValue fpValue(value);
    return Real64(fpValue.build64(environment), true);
}

Real64 Real64::fromDouble(double value, Environment & environment)
{
    FpValue fpValue(value);
    return Real64(fpValue.build64(environment), true);
}

Real64 Real64::fromReal21(const Real21 & value, Environment & environment)
{
    bool denormalized = false;
    FpValue fpValue = FpValue::parse21(denormalized, value.bitPattern);
    return Real64(fpValue.build64(environment), true);
}

Real64 Real64::fromReal32(const Real32 & value, Environment & environment)
{
    bool denormalized = false;
    FpValue fpValue = FpValue::parse32(denormalized, value.bitPattern);
    return Real64(fpValue.build64(environment), true);
}

//////////
//  Operations
int8_t Real64::toInt8(Environment & environment) const
{
    int64_t temp = toInt64(environment);
    if (temp < INT8_MIN)
    {
        environment.exception |= Exception::Overflow;
        return INT8_MIN;
    }
    else if (temp > INT8_MAX)
    {
        environment.exception |= Exception::Overflow;
        return INT8_MAX;
    }
    else
    {
        return static_cast<int8_t>(temp);
    }
}

uint8_t Real64::toUInt8(Environment & environment) const
{
    uint64_t temp = toUInt64(environment);
    if (temp > UINT8_MAX)
    {
        environment.exception |= Exception::Overflow;
        return UINT8_MAX;
    }
    else
    {
        return static_cast<uint8_t>(temp);
    }
}

int16_t Real64::toInt16(Environment & environment) const
{
    int64_t temp = toInt64(environment);
    if (temp < INT16_MIN)
    {
        environment.exception |= Exception::Overflow;
        return INT16_MIN;
    }
    else if (temp > INT16_MAX)
    {
        environment.exception |= Exception::Overflow;
        return INT16_MAX;
    }
    else
    {
        return static_cast<int16_t>(temp);
    }
}

uint16_t Real64::toUInt16(Environment & environment) const
{
    uint64_t temp = toUInt64(environment);
    if (temp > UINT16_MAX)
    {
        environment.exception |= Exception::Overflow;
        return UINT16_MAX;
    }
    else
    {
        return static_cast<uint16_t>(temp);
    }
}

int32_t Real64::toInt32(Environment & environment) const
{
    int64_t temp = toInt64(environment);
    if (temp < INT32_MIN)
    {
        environment.exception |= Exception::Overflow;
        return INT32_MIN;
    }
    else if (temp > INT32_MAX)
    {
        environment.exception |= Exception::Overflow;
        return INT32_MAX;
    }
    else
    {
        return static_cast<int32_t>(temp);
    }
}

uint32_t Real64::toUInt32(Environment & environment) const
{
    uint64_t temp = toUInt64(environment);
    if (temp > UINT32_MAX)
    {
        environment.exception |= Exception::Overflow;
        return UINT32_MAX;
    }
    else
    {
        return static_cast<uint32_t>(temp);
    }
}

int64_t Real64::toInt64(Environment & environment) const
{
    bool denormalized = false;
    FpValue fpValue = FpValue::parse64(denormalized, this->bitPattern);
    return FpValue::toSignedInteger(environment, fpValue);
}

uint64_t Real64::toUInt64(Environment & environment) const
{
    bool denormalized = false;
    FpValue fpValue = FpValue::parse64(denormalized, this->bitPattern);
    return FpValue::toUnsignedInteger(environment, fpValue);
}

float Real64::toFloat(Environment & environment) const
{
    double temp = toDouble(environment);
    float result = static_cast<float>(temp);
    if (hadesvm::util::Math::isNan(temp) || hadesvm::util::Math::isInfinite(temp))
    {   //  Already not a real number
        return result;
    }
    else if (hadesvm::util::Math::isInfinite(result))
    {   //  Narrowing double to float caused an overflow
        environment.exception |= Exception::Overflow;
        return result;
    }
    else if (temp != 0.0 && result == 0.0)
    {   //  Narrowing double to float caused an underflow
        environment.exception |= Exception::Underflow;
        return result;
    }
    else
    {   //  All good
        return result;
    }
}

double Real64::toDouble(Environment & environment) const
{
    bool denormalized = false;
    FpValue value = FpValue::parse64(denormalized, this->bitPattern);
    return static_cast<double>(FpValue::toReal(environment, value));
}

Real21 Real64::toReal21(Environment & environment) const
{
    bool denormalized = false;
    FpValue fpValue = FpValue::parse64(denormalized, this->bitPattern);
    return Real21::fromBitPattern(fpValue.build21(environment));
}

Real32 Real64::toReal32(Environment & environment) const
{
    bool denormalized = false;
    FpValue fpValue = FpValue::parse64(denormalized, this->bitPattern);
    return Real32::fromBitPattern(fpValue.build32(environment));
}

Real64 Real64::add(const Real64 & op2, Environment & environment) const
{
    bool xDenormalized = false, yDenormalized = false;
    FpValue xValue = FpValue::parse64(xDenormalized, this->bitPattern);
    FpValue yValue = FpValue::parse64(yDenormalized, op2.bitPattern);
    FpValue resultValue = FpValue::add(environment, xValue, yValue);
    return Real64(resultValue.build64(environment), true);
}

Real64 Real64::subtract(const Real64 & op2, Environment & environment) const
{
    bool xDenormalized = false, yDenormalized = false;
    FpValue xValue = FpValue::parse64(xDenormalized, this->bitPattern);
    FpValue yValue = FpValue::parse64(yDenormalized, op2.bitPattern);
    FpValue resultValue = FpValue::subtract(environment, xValue, yValue);
    return Real64(resultValue.build64(environment), true);
}

Real64 Real64::multiply(const Real64 & op2, Environment & environment) const
{
    bool xDenormalized = false, yDenormalized = false;
    FpValue xValue = FpValue::parse64(xDenormalized, this->bitPattern);
    FpValue yValue = FpValue::parse64(yDenormalized, op2.bitPattern);
    FpValue resultValue = FpValue::multiply(environment, xValue, yValue);
    return Real64(resultValue.build64(environment), true);
}

Real64 Real64::divide(const Real64 & op2, Environment & environment) const
{
    bool xDenormalized = false, yDenormalized = false;
    FpValue xValue = FpValue::parse64(xDenormalized, this->bitPattern);
    FpValue yValue = FpValue::parse64(yDenormalized, op2.bitPattern);
    FpValue resultValue = FpValue::divide(environment, xValue, yValue);
    return Real64(resultValue.build64(environment), true);
}

Real64 Real64::negate(Environment & environment) const
{
    bool xDenormalized = false;
    FpValue xValue = FpValue::parse64(xDenormalized, this->bitPattern);
    FpValue resultValue = FpValue::negate(environment, xValue);
    return Real64(resultValue.build64(environment), true);
}

int Real64::compare(const Real64 & op2, Environment & environment) const
{
    bool xDenormalized = false, yDenormalized = false;
    FpValue xValue = FpValue::parse64(xDenormalized, this->bitPattern);
    FpValue yValue = FpValue::parse64(yDenormalized, op2.bitPattern);
    return FpValue::compare(environment, xValue, yValue);
}

ValueClass Real64::classify() const
{
    bool denormalized = false;
    FpValue value = FpValue::parse64(denormalized, this->bitPattern);
    return FpValue::classify(value, denormalized);
}

Real64 Real64::sqrt(Environment & environment) const
{
    bool denormalized = false;
    FpValue thisValue = FpValue::parse64(denormalized, this->bitPattern);
    //  Handle special cases
    if (thisValue.isSignalingNan)
    {   //  SNaN
        environment.exception |= Exception::Operand;
        return *this;
    }
    else if (thisValue.isQuietNan)
    {   //  QNaN
        return *this;
    }
    else if (thisValue.isInfinity)
    {   //  +/- inf
        return *this;
    }
    else if (thisValue.signBit != 0)
    {   //  OOPS! Negative!
        environment.exception |= Exception::Operand;
        return *this;
    }
    //  Start guessing at 1.0
    FpValue oneHalf(false, false, false, 0, 63, 1);
    FpValue xValue(false, false, false, 0, 64, 1);
    for (int iteration = 0; iteration < 1000; iteration++)   //  ...to make sure it terminates
    {   //  Is "x*x" same as "this" to within 14 digits ?
        FpValue xSquaredValue = FpValue::multiply(environment, xValue, xValue);
        uint64_t largerMantissa = qMax(xSquaredValue.mantissa, thisValue.mantissa);
        uint64_t smallerMantissa = qMin(xSquaredValue.mantissa, thisValue.mantissa);
        if (xSquaredValue.signBit == thisValue.signBit &&
            xSquaredValue.exponent == thisValue.exponent &&
            (largerMantissa - smallerMantissa) < UINT64_C(0x0000000000000100))
        {
            break;
        }
        //  No - determine the next best guess
        FpValue xNextValue = FpValue::multiply(environment, oneHalf, FpValue::add(environment, xValue, FpValue::divide(environment, thisValue, xValue)));
        if (xNextValue.signBit == xValue.signBit &&
            xNextValue.exponent == xValue.exponent &&
            xNextValue.mantissa == xValue.mantissa)
        {   //  No improvement!
            break;
        }
        xValue = xNextValue;
    }
    //  We have the result
    return Real64(xValue.build64(environment), true);
}

//  End of hadesvm-ieee754/Real64.cpp
