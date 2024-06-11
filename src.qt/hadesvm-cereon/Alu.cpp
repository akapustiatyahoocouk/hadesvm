//
//  hadesvm-cereon/Alu.cpp
//
//  hadesvm::cereon::Alu class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Operations (arithmetic)
uint64_t Alu::addSigned(uint64_t x, uint64_t y, bool & overflow)
{
    uint64_t result = x + y;

    if ((x & SignBit64) == (y & SignBit64) &&
        (result & SignBit64) != (x & SignBit64))
    {
        overflow = true;
    }
    return result;
}

uint64_t Alu::subtractSigned(uint64_t x, uint64_t y, bool & overflow)
{
    uint64_t result = x - y;

    if ((x & SignBit64) != (y & SignBit64) &&
        (result & SignBit64) != (x & SignBit64))
    {
        overflow = true;
    }
    return result;
}

uint64_t Alu::multiplySigned(uint64_t x, uint64_t y, bool & overflow)
{
    int64_t ix = static_cast<int64_t>(x),
            iy = static_cast<int64_t>(y);
    int64_t result = ix * iy;

    if (iy != 0 && result / iy != ix)
    {
        overflow = true;
    }
    return static_cast<uint64_t>(result);
}

uint64_t Alu::divideSigned(uint64_t x, uint64_t y, bool & overflow, bool & divisionByZero)
{
    int64_t ix = static_cast<int64_t>(x),
            iy = static_cast<int64_t>(y);
    int64_t result;

    if (iy == 0)
    {
        result = 0;
        divisionByZero = true;
    }
    else if (ix == INT64_MIN && iy == -1)
    {
        result = INT64_MIN;
        overflow = true;
    }
    else
    {
        result = ix / iy;
    }
    return static_cast<uint64_t>(result);
}

uint64_t Alu::moduloSigned(uint64_t x, uint64_t y, bool & divisionByZero)
{
    int64_t ix = static_cast<int64_t>(x),
            iy = static_cast<int64_t>(y);
    int64_t result;

    if (iy == 0)
    {
        result = 0;
        divisionByZero = true;
    }
    else
    {
        bool overflow = false;
        int64_t quotient = static_cast<int64_t>(Alu::divideSigned(x, y, overflow, divisionByZero));
        result = ix - (quotient * iy);
    }
    return static_cast<uint64_t>(result);
}

uint64_t Alu::absSigned(uint64_t x, bool & overflow)
{
    int64_t ix = static_cast<int64_t>(x);
    int64_t result = ((x & SignBit64) != 0) ? (-ix) : ix;
    if ((ix & SignBit64) != 0 && (result & SignBit64) != 0)
    {
        overflow = true;
    }
    return static_cast<uint64_t>(result);
}

uint64_t Alu::addUnsigned(uint64_t x, uint64_t y, bool & overflow)
{
    uint64_t result = x + y;

    if (result < x)
    {
        overflow = true;
    }
    return result;
}

uint64_t Alu::subtractUnsigned(uint64_t x, uint64_t y, bool & overflow)
{
    uint64_t result = x - y;

    if (result > x)
    {
        overflow = true;
    }
    return result;
}

uint64_t Alu::multiplyUnsigned(uint64_t x, uint64_t y, bool & overflow)
{
    uint64_t result = x * y;

    if (y != 0 && result / y != x)
    {
        overflow = true;
    }
    return result;
}

uint64_t Alu::divideUnsigned(uint64_t x, uint64_t y, bool & divisionByZero)
{
    uint64_t result;

    if (y == 0)
    {
        result = 0;
        divisionByZero = true;
    }
    else
    {
        result = x / y;
    }
    return result;
}

uint64_t Alu::moduloUnsigned(uint64_t x, uint64_t y, bool & divisionByZero)
{
    uint64_t result;

    if (y == 0)
    {
        result = 0;
        divisionByZero = true;
    }
    else
    {
        result = x - (x / y) * y;
    }
    return result;
}

uint64_t Alu::reverseBytes16(uint64_t x)
{
    return (x & UINT64_C(0xFFFFFFFFFFFF0000)) |
           std::byteswap<uint16_t>(static_cast<uint16_t>(x));
}

uint64_t Alu::reverseBytes32(uint64_t x)
{
    return (x & UINT64_C(0xFFFFFFFF00000000)) |
           std::byteswap<uint32_t>(static_cast<uint32_t>(x));
}

uint64_t Alu::reverseBytes64(uint64_t x)
{
    return (x & UINT64_C(0xFFFFFFFF00000000)) |
           std::byteswap<uint64_t>(x);
}

uint64_t Alu::reverseBits8(uint64_t x)
{
    uint8_t result = static_cast<uint8_t>(x);
    result = static_cast<uint8_t>(((result & 0xAA) >> 1) | ((result & 0x55) << 1));
    result = static_cast<uint8_t>(((result & 0xCC) >> 2) | ((result & 0x33) << 2));
    result = static_cast<uint8_t>((result >> 4) | (result << 4));
    return (x & UINT64_C(0xFFFFFFFFFFFFFF00)) | result;
}

uint64_t Alu::reverseBits16(uint64_t x)
{
    uint16_t result = static_cast<uint16_t>(x);
    result = static_cast<uint16_t>(((result & 0xAAAA) >> 1) | ((result & 0x5555) << 1));
    result = static_cast<uint16_t>(((result & 0xCCCC) >> 2) | ((result & 0x3333) << 2));
    result = static_cast<uint16_t>(((result & 0xF0F0) >> 4) | ((result & 0x0F0F) << 4));
    result = static_cast<uint16_t>((result >> 8) | (result << 8));
    return (x & UINT64_C(0xFFFFFFFFFFFF0000)) | result;
}

uint64_t Alu::reverseBits32(uint64_t x)
{
    uint32_t result = static_cast<uint32_t>(x);
    result = ((result & 0xAAAAAAAA) >> 1) | ((result & 0x55555555) << 1);
    result = ((result & 0xCCCCCCCC) >> 2) | ((result & 0x33333333) << 2);
    result = ((result & 0xF0F0F0F0) >> 4) | ((result & 0x0F0F0F0F) << 4);
    result = ((result & 0xFF00FF00) >> 8) | ((result & 0x00FF00FF) << 8);
    result = (result >> 16) | (result << 16);
    return (x & UINT64_C(0xFFFFFFFF00000000)) | result;
}

uint64_t Alu::reverseBits64(uint64_t x)
{
    uint64_t result = x;
    result = ((result & UINT64_C(0xAAAAAAAAAAAAAAAA)) >> 1) | ((result & UINT64_C(0x5555555555555555)) << 1);
    result = ((result & UINT64_C(0xCCCCCCCCCCCCCCCC)) >> 2) | ((result & UINT64_C(0x3333333333333333)) << 2);
    result = ((result & UINT64_C(0xF0F0F0F0F0F0F0F0)) >> 4) | ((result & UINT64_C(0x0F0F0F0F0F0F0F0F)) << 4);
    result = ((result & UINT64_C(0xFF00FF00FF00FF00)) >> 8) | ((result & UINT64_C(0x00FF00FF00FF00FF)) << 8);
    result = ((result & UINT64_C(0xFFFF0000FFFF0000)) >> 16) | ((result & UINT64_C(0x0000FFFF0000FFFF)) << 16);
    result = (result >> 32) | (result << 32);
    return (x & UINT64_C(0xFFFFFFFF00000000)) | result;
}

uint64_t Alu::shl8(uint64_t x, uint64_t shiftCount)
{
    uint8_t result;

    if (shiftCount >= 8)
    {   //  Entirely shifted out
        result = 0;
    }
    else
    {   //  Partially shifted out
        result = static_cast<uint8_t>(x);
        result <<= static_cast<unsigned>(shiftCount);
    }
    return (x & UINT64_C(0xFFFFFFFFFFFFFF00)) | result;
}

uint64_t Alu::shl16(uint64_t x, uint64_t shiftCount)
{
    uint16_t result;

    if (shiftCount >= 16)
    {   //  Entirely shifted out
        result = 0;
    }
    else
    {   //  Partially shifted out
        result = static_cast<uint16_t>(x);
        result <<= static_cast<unsigned>(shiftCount);
    }
    return (x & UINT64_C(0xFFFFFFFFFFFF0000)) | result;
}

uint64_t Alu::shl32(uint64_t x, uint64_t shiftCount)
{
    uint32_t result;

    if (shiftCount >= 32)
    {   //  Entirely shifted out
        result = 0;
    }
    else
    {   //  Partially shifted out
        result = static_cast<uint32_t>(x);
        result <<= static_cast<unsigned>(shiftCount);
    }
    return (x & UINT64_C(0xFFFFFFFF00000000)) | result;
}

uint64_t Alu::shl64(uint64_t x, uint64_t shiftCount)
{
    if (shiftCount >= 64)
    {   //  Entirely shifted out
        return 0;
    }
    else
    {   //  Partially shifted out
        return x << static_cast<unsigned>(shiftCount);
    }
}

uint64_t Alu::shr8(uint64_t x, uint64_t shiftCount)
{
    uint8_t result;

    if (shiftCount >= 8)
    {   //  Entirely shifted out
        result = 0;
    }
    else
    {   //  Partially shifted out
        result = static_cast<uint8_t>(x);
        result >>= static_cast<unsigned>(shiftCount);
    }
    return (x & UINT64_C(0xFFFFFFFFFFFFFF00)) | result;
}

uint64_t Alu::shr16(uint64_t x, uint64_t shiftCount)
{
    uint16_t result;

    if (shiftCount >= 16)
    {   //  Entirely shifted out
        result = 0;
    }
    else
    {   //  Partially shifted out
        result = static_cast<uint16_t>(x);
        result >>= static_cast<unsigned>(shiftCount);
    }
    return (x & UINT64_C(0xFFFFFFFFFFFF0000)) | result;
}

uint64_t Alu::shr32(uint64_t x, uint64_t shiftCount)
{
    uint32_t result;

    if (shiftCount >= 32)
    {   //  Entirely shifted out
        result = 0;
    }
    else
    {   //  Partially shifted out
        result = static_cast<uint32_t>(x);
        result >>= static_cast<unsigned>(shiftCount);
    }
    return (x & UINT64_C(0xFFFFFFFF00000000)) | result;
}

uint64_t Alu::shr64(uint64_t x, uint64_t shiftCount)
{
    if (shiftCount >= 64)
    {   //  Entirely shifted out
        return 0;
    }
    else
    {   //  Partially shifted out
        return x >> static_cast<unsigned>(shiftCount);
    }
}

uint64_t Alu::asl8(uint64_t x, uint64_t shiftCount)
{
    uint8_t result = static_cast<uint8_t>(x);
    for (unsigned normalizedShiftCount = (shiftCount >= 8) ? 8 : static_cast<unsigned>(shiftCount);
         normalizedShiftCount != 0;
         normalizedShiftCount--)
    {
        result =
            static_cast<uint8_t>(
                ((result << 1) & UINT8_C(0x7F)) |
                (result & UINT8_C(0x80)));
    }
    return (x & UINT64_C(0xFFFFFFFFFFFFFF00)) | result;
}

uint64_t Alu::asl16(uint64_t x, uint64_t shiftCount)
{
    uint16_t result = static_cast<uint16_t>(x);
    for (unsigned normalizedShiftCount = (shiftCount >= 16) ? 16 : static_cast<unsigned>(shiftCount);
         normalizedShiftCount != 0;
         normalizedShiftCount--)
    {
        result =
            static_cast<uint16_t>(
                ((result << 1) & UINT16_C(0x7FFF)) |
                (result & UINT16_C(0x8000)));
    }
    return (x & UINT64_C(0xFFFFFFFFFFFF0000)) | result;
}

uint64_t Alu::asl32(uint64_t x, uint64_t shiftCount)
{
    uint32_t result = static_cast<uint32_t>(x);
    for (unsigned normalizedShiftCount = (shiftCount >= 32) ? 32 : static_cast<unsigned>(shiftCount);
         normalizedShiftCount != 0;
         normalizedShiftCount--)
    {
        result = ((result << 1) & UINT32_C(0x7FFFFFFF)) |
                 (result & UINT32_C(0x80000000));
    }
    return (x & UINT64_C(0xFFFFFFFF00000000)) | result;
}

uint64_t Alu::asl64(uint64_t x, uint64_t shiftCount)
{
    uint64_t result = x;
    for (unsigned normalizedShiftCount = (shiftCount >= 64) ? 64 : static_cast<unsigned>(shiftCount);
         normalizedShiftCount != 0;
         normalizedShiftCount--)
    {
        result = ((result << 1) & UINT64_C(0x7FFFFFFFFFFFFFFF)) |
                 (result & UINT64_C(0x8000000000000000));
    }
    return result;
}

uint64_t Alu::asr8(uint64_t x, uint64_t shiftCount)
{
    uint8_t result = static_cast<uint8_t>(x);
    for (unsigned normalizedShiftCount = (shiftCount >= 8) ? 8 : static_cast<unsigned>(shiftCount);
         normalizedShiftCount != 0;
         normalizedShiftCount--)
    {
        result =
            static_cast<uint8_t>(
                (result >> 1) |
                (result & UINT8_C(0x80)));
    }
    return (x & UINT64_C(0xFFFFFFFFFFFFFF00)) | result;
}

uint64_t Alu::asr16(uint64_t x, uint64_t shiftCount)
{
    uint16_t result = static_cast<uint16_t>(x);
    for (unsigned normalizedShiftCount = (shiftCount >= 16) ? 16 : static_cast<unsigned>(shiftCount);
         normalizedShiftCount != 0;
         normalizedShiftCount--)
    {
        result =
            static_cast<uint16_t>(
                (result >> 1) |
                (result & UINT16_C(0x8000)));
    }
    return (x & UINT64_C(0xFFFFFFFFFFFF0000)) | result;
}

uint64_t Alu::asr32(uint64_t x, uint64_t shiftCount)
{
    uint32_t result = static_cast<uint32_t>(x);
    for (unsigned normalizedShiftCount = (shiftCount >= 32) ? 32 : static_cast<unsigned>(shiftCount);
         normalizedShiftCount != 0;
         normalizedShiftCount--)
    {
        result = (result >> 1) |
                 (result & UINT32_C(0x80000000));
    }
    return (x & UINT64_C(0xFFFFFFFF00000000)) | result;
}

uint64_t Alu::asr64(uint64_t x, uint64_t shiftCount)
{
    uint64_t result = x;
    for (unsigned normalizedShiftCount = (shiftCount >= 64) ? 64 : static_cast<unsigned>(shiftCount);
         normalizedShiftCount != 0;
         normalizedShiftCount--)
    {
        result = (result >> 1) |
                 (result & UINT64_C(0x8000000000000000));
    }
    return result;
}

uint64_t Alu::rol8(uint64_t x, uint64_t shiftCount)
{
    uint8_t result = static_cast<uint8_t>(x);
    for (unsigned normalizedShiftCount = static_cast<unsigned>(shiftCount) & 0x07;
         normalizedShiftCount != 0;
         normalizedShiftCount--)
    {
        result = static_cast<uint8_t>((result << 1) | (result >> 7));
    }
    return (x & UINT64_C(0xFFFFFFFFFFFFFF00)) | result;
}

uint64_t Alu::rol16(uint64_t x, uint64_t shiftCount)
{
    uint16_t result = static_cast<uint16_t>(x);
    for (unsigned normalizedShiftCount = static_cast<unsigned>(shiftCount) & 0x0F;
         normalizedShiftCount != 0;
         normalizedShiftCount--)
    {
        result = static_cast<uint16_t>((result << 1) | (result >> 15));
    }
    return (x & UINT64_C(0xFFFFFFFFFFFF0000)) | result;
}

uint64_t Alu::rol32(uint64_t x, uint64_t shiftCount)
{
    uint32_t result = static_cast<uint32_t>(x);
    for (unsigned normalizedShiftCount = static_cast<unsigned>(shiftCount) & 0x1F;
         normalizedShiftCount != 0;
         normalizedShiftCount--)
    {
        result = ((result << 1) | (result >> 31));
    }
    return (x & UINT64_C(0xFFFFFFFF00000000)) | result;
}

uint64_t Alu::rol64(uint64_t x, uint64_t shiftCount)
{
    uint64_t result = x;
    for (unsigned normalizedShiftCount = static_cast<unsigned>(shiftCount) & 0x3F;
         normalizedShiftCount != 0;
         normalizedShiftCount--)
    {
        result = ((result << 1) | (result >> 63));
    }
    return result;
}

uint64_t Alu::ror8(uint64_t x, uint64_t shiftCount)
{
    uint8_t result = static_cast<uint8_t>(x);
    for (unsigned normalizedShiftCount = static_cast<unsigned>(shiftCount) & 0x07;
         normalizedShiftCount != 0;
         normalizedShiftCount--)
    {
        result = static_cast<uint8_t>((result >> 1) | (result << 7));
    }
    return (x & UINT64_C(0xFFFFFFFFFFFFFF00)) | result;
}

uint64_t Alu::ror16(uint64_t x, uint64_t shiftCount)
{
    uint16_t result = static_cast<uint16_t>(x);
    for (unsigned normalizedShiftCount = static_cast<unsigned>(shiftCount) & 0x0F;
         normalizedShiftCount != 0;
         normalizedShiftCount--)
    {
        result = static_cast<uint16_t>((result >> 1) | (result << 15));
    }
    return (x & UINT64_C(0xFFFFFFFFFFFF0000)) | result;
}

uint64_t Alu::ror32(uint64_t x, uint64_t shiftCount)
{
    uint32_t result = static_cast<uint32_t>(x);
    for (unsigned normalizedShiftCount = static_cast<unsigned>(shiftCount) & 0x1F;
         normalizedShiftCount != 0;
         normalizedShiftCount--)
    {
        result = ((result >> 1) | (result << 31));
    }
    return (x & UINT64_C(0xFFFFFFFF00000000)) | result;
}

uint64_t Alu::ror64(uint64_t x, uint64_t shiftCount)
{
    uint64_t result = x;
    for (unsigned normalizedShiftCount = static_cast<unsigned>(shiftCount) & 0x3F;
         normalizedShiftCount != 0;
         normalizedShiftCount--)
    {
        result = ((result >> 1) | (result << 63));
    }
    return result;
}

uint64_t Alu::countLeadingZeroes(uint64_t x)
{
    unsigned result = 0;
    for (uint64_t mask = UINT64_C(0x8000000000000000); mask != 0; mask >>= 1)
    {
        if ((x & mask) != 0)
        {
            break;
        }
        result++;
    }
    return result;
}

uint64_t Alu::countLeadingOnes(uint64_t x)
{
    unsigned result = 0;
    for (uint64_t mask = UINT64_C(0x8000000000000000); mask != 0; mask >>= 1)
    {
        if ((x & mask) == 0)
        {
            break;
        }
        result++;
    }
    return result;
}

uint64_t Alu::countTailingZeroes(uint64_t x)
{
    unsigned result = 0;
    for (uint64_t mask = 0x01; mask != 0; mask <<= 1)
    {
        if ((x & mask) != 0)
        {
            break;
        }
        result++;
    }
    return result;
}

uint64_t Alu::countTailingOnes(uint64_t x)
{
    unsigned result = 0;
    for (uint64_t mask = 0x01; mask != 0; mask <<= 1)
    {
        if ((x & mask) == 0)
        {
            break;
        }
        result++;
    }
    return result;
}

uint64_t Alu::injectBitField(uint64_t base, uint64_t value, unsigned offset, unsigned bitCount)
{
    Q_ASSERT(offset <= 63);
    Q_ASSERT(bitCount >= 1 && bitCount <= 63);
    Q_ASSERT(bitCount + offset <= 64);

    uint64_t bitFieldMask = UINT64_C(0xFFFFFFFFFFFFFFFF) >> (64 - bitCount);
    return (base & (~bitFieldMask << offset)) |
           (value & bitFieldMask) << offset;
}

uint64_t Alu::extractSignedBitField(uint64_t base, unsigned offset, unsigned bitCount)
{
    Q_ASSERT(offset <= 63);
    Q_ASSERT(bitCount >= 1 && bitCount <= 63);
    Q_ASSERT(bitCount + offset <= 64);

    uint64_t bitFieldMask = UINT64_C(0xFFFFFFFFFFFFFFFF) >> (64 - bitCount);
    uint64_t signBit = bitFieldMask ^ (bitFieldMask >> 1);
    uint64_t result = (base >> offset) & bitFieldMask;
    return ((result & signBit) == 0) ? result : (result | ~bitFieldMask);
}

uint64_t Alu::extractUnsignedBitField(uint64_t base, unsigned offset, unsigned bitCount)
{
    Q_ASSERT(offset <= 63);
    Q_ASSERT(bitCount >= 1 && bitCount <= 63);
    Q_ASSERT(bitCount + offset <= 64);

    uint64_t bitFieldMask = UINT64_C(0xFFFFFFFFFFFFFFFF) >> (64 - bitCount);
    return (base >> offset) & bitFieldMask;
}

int Alu::compareSigned(uint64_t x, uint64_t y)
{
    int64_t ix = static_cast<int64_t>(x),
            iy = static_cast<int64_t>(y);

    if (ix < iy)
    {
        return -1;
    }
    else if (ix > iy)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int Alu::compareUnsigned(uint64_t x, uint64_t y)
{
    if (x < y)
    {
        return -1;
    }
    else if (x > y)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//  End of hadesvm-cereon/Alu.cpp
