//
//  hadesvm-core/MemorySize.cpp
//
//  hadesvm::core::MemorySize class implementation
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

//////////
//  Construction/destruction
MemorySize::MemorySize(uint64_t numberOfUnits, Unit unit)
    :   _numberOfUnits(numberOfUnits),
        _unit(unit)
{
    if (_numberOfUnits > UINT64_MAX / static_cast<uint64_t>(_unit))
    {
        _numberOfUnits = UINT64_MAX / static_cast<uint64_t>(_unit);
    }
}

//////////
//  Operations
uint64_t MemorySize::toBytes() const
{
    return _numberOfUnits * static_cast<uint64_t>(_unit);
}

MemorySize MemorySize::bytes(uint64_t count)
{
    return MemorySize(count, Unit::B);
}

MemorySize MemorySize::kilobytes(uint64_t count)
{
    return MemorySize(count, Unit::KB);
}

MemorySize MemorySize::megabytes(uint64_t count)
{
    return MemorySize(count, Unit::MB);
}

MemorySize MemorySize::gigabytes(uint64_t count)
{
    return MemorySize(count, Unit::GB);
}

int MemorySize::compare(const MemorySize & op2) const
{
    uint64_t b1 = this->toBytes();
    uint64_t b2 = op2.toBytes();

    if (b1 < b2)
    {
        return -1;
    }
    if (b1 > b2)
    {
        return 1;
    }
    return 0;
}

//  End of hadesvm-core/MemorySize.cpp
