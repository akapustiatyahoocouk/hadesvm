//
//  hadesvm-ieee754/Real21.cpp
//
//  hadesvm::ieee754::Real21 class implementation
//
//////////
#include "hadesvm-ieee754/API.hpp"
#include "hadesvm-ieee754/FpValue.hxx"
using namespace hadesvm::ieee754;

//////////
//  Construction/destruction/assignment
Real21::Real21(uint32_t bitPatternParam, bool /*dummy*/)
    :   bitPattern(bitPatternParam)
{
}

Real21 Real21::fromBitPattern(uint32_t bitPattern)
{
    return Real21(bitPattern & 0x001FFFFF, true);
}

Real21 Real21::fromInt8(int8_t value, Environment & environment)
{
    FpValue fpValue(false, false, false, static_cast<uint8_t>((value < 0) ? 1 : 0), 64, static_cast<uint64_t>((value >= 0) ? value : (-value)));
    return Real21(fpValue.build21(environment), true);
}

Real21 Real21::fromUInt8(uint8_t value, Environment & environment)
{
    FpValue fpValue(false, false, false, 0, 64, value);
    return Real21(fpValue.build21(environment), true);
}

Real21 Real21::fromInt16(int16_t value, Environment & environment)
{
    FpValue fpValue(false, false, false, static_cast<uint8_t>((value < 0) ? 1 : 0), 64, static_cast<uint64_t>((value >= 0) ? value : (-value)));
    return Real21(fpValue.build21(environment), true);
}

Real21 Real21::fromUInt16(uint16_t value, Environment & environment)
{
    FpValue fpValue(false, false, false, 0, 64, value);
    return Real21(fpValue.build21(environment), true);
}

Real21 Real21::fromInt32(int32_t value, Environment & environment)
{
    FpValue fpValue(false, false, false, static_cast<uint8_t>((value < 0) ? 1 : 0), 64, static_cast<uint64_t>((value >= 0) ? value : (-value)));
    return Real21(fpValue.build21(environment), true);
}

Real21 Real21::fromUInt32(uint32_t value, Environment & environment)
{
    FpValue fpValue(false, false, false, 0, 64, value);
    return Real21(fpValue.build21(environment), true);
}

Real21 Real21::fromInt64(int64_t value, Environment & environment)
{
    FpValue fpValue(false, false, false, static_cast<uint8_t>((value < 0) ? 1 : 0), 64, static_cast<uint64_t>((value >= 0) ? value : (-value)));
    return Real21(fpValue.build21(environment), true);
}

Real21 Real21::fromUInt64(uint64_t value, Environment & environment)
{
    FpValue fpValue(false, false, false, 0, 64, value);
    return Real21(fpValue.build21(environment), true);
}

Real21 Real21::fromFloat(float value, Environment & environment)
{
    FpValue fpValue(value);
    return Real21(fpValue.build21(environment), true);
}

Real21 Real21::fromDouble(double value, Environment & environment)
{
    FpValue fpValue(value);
    return Real21(fpValue.build21(environment), true);
}

Real21 Real21::fromReal32(const Real32 & value, Environment & environment)
{
    bool denormalized = false;
    FpValue fpValue = FpValue::parse32(denormalized, value.bitPattern);
    return Real21(fpValue.build21(environment), true);
}

Real21 Real21::fromReal64(const Real64 & value, Environment & environment)
{
    bool denormalized = false;
    FpValue fpValue = FpValue::parse64(denormalized, value.bitPattern);
    return Real21(fpValue.build21(environment), true);
}

//////////
//  Operations
int8_t Real21::toInt8(Environment & environment) const
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

uint8_t Real21::toUInt8(Environment & environment) const
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

int16_t Real21::toInt16(Environment & environment) const
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
        return static_cast<int8_t>(temp);
    }
}

uint16_t Real21::toUInt16(Environment & environment) const
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

int32_t Real21::toInt32(Environment & environment) const
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

uint32_t Real21::toUInt32(Environment & environment) const
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

int64_t Real21::toInt64(Environment & environment) const
{
    bool denormalized = false;
    FpValue fpValue = FpValue::parse21(denormalized, this->bitPattern);
    return FpValue::toSignedInteger(environment, fpValue);
}

uint64_t Real21::toUInt64(Environment & environment) const
{
    bool denormalized = false;
    FpValue fpValue = FpValue::parse21(denormalized, this->bitPattern);
    return FpValue::toUnsignedInteger(environment, fpValue);
}

float Real21::toFloat(Environment & environment) const
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

double Real21::toDouble(Environment & environment) const
{
    bool denormalized = false;
    FpValue value = FpValue::parse21(denormalized, this->bitPattern);
    return static_cast<double>(FpValue::toReal(environment, value));
}

Real32 Real21::toReal32(Environment & environment) const
{
    bool denormalized = false;
    FpValue fpValue = FpValue::parse21(denormalized, this->bitPattern);
    return Real32::fromBitPattern(fpValue.build32(environment));
}

Real64 Real21::toReal64(Environment & environment) const
{
    bool denormalized = false;
    FpValue fpValue = FpValue::parse21(denormalized, this->bitPattern);
    return Real64::fromBitPattern(fpValue.build64(environment));
}

Real21 Real21::add(const Real21 & op2, Environment & environment) const
{
    bool xDenormalized = false, yDenormalized = false;
    FpValue xValue = FpValue::parse21(xDenormalized, this->bitPattern);
    FpValue yValue = FpValue::parse21(yDenormalized, op2.bitPattern);
    FpValue resultValue = FpValue::add(environment, xValue, yValue);
    return Real21(resultValue.build21(environment), true);
}

Real21 Real21::subtract(const Real21 & op2, Environment & environment) const
{
    bool xDenormalized = false, yDenormalized = false;
    FpValue xValue = FpValue::parse21(xDenormalized, this->bitPattern);
    FpValue yValue = FpValue::parse21(yDenormalized, op2.bitPattern);
    FpValue resultValue = FpValue::subtract(environment, xValue, yValue);
    return Real21(resultValue.build21(environment), true);
}

Real21 Real21::multiply(const Real21 & op2, Environment & environment) const
{
    bool xDenormalized = false, yDenormalized = false;
    FpValue xValue = FpValue::parse21(xDenormalized, this->bitPattern);
    FpValue yValue = FpValue::parse21(yDenormalized, op2.bitPattern);
    FpValue resultValue = FpValue::multiply(environment, xValue, yValue);
    return Real21(resultValue.build21(environment), true);
}

Real21 Real21::divide(const Real21 & op2, Environment & environment) const
{
    bool xDenormalized = false, yDenormalized = false;
    FpValue xValue = FpValue::parse21(xDenormalized, this->bitPattern);
    FpValue yValue = FpValue::parse21(yDenormalized, op2.bitPattern);
    FpValue resultValue = FpValue::divide(environment, xValue, yValue);
    return Real21(resultValue.build21(environment), true);
}

Real21 Real21::negate(Environment & environment) const
{
    bool xDenormalized = false;
    FpValue xValue = FpValue::parse21(xDenormalized, this->bitPattern);
    FpValue resultValue = FpValue::negate(environment, xValue);
    return Real21(resultValue.build21(environment), true);
}

int Real21::compare(const Real21 & op2, Environment & environment) const
{
    bool xDenormalized = false, yDenormalized = false;
    FpValue xValue = FpValue::parse21(xDenormalized, this->bitPattern);
    FpValue yValue = FpValue::parse21(yDenormalized, op2.bitPattern);
    return FpValue::compare(environment, xValue, yValue);
}

ValueClass Real21::classify() const
{
    bool denormalized = false;
    FpValue value = FpValue::parse21(denormalized, this->bitPattern);
    return FpValue::classify(value, denormalized);
}

//  End of hadesvm-ieee754/Real21.cpp
