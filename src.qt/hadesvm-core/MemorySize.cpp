//
//  hadesvm-core/MemorySize.cpp
//
//  MemorySize class implementation
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

//  Formatting and parsing
namespace
{
    struct MemorySizeUnitInfo
    {
        MemorySize::Unit    unit;
        const char *        name;
    };

    const MemorySizeUnitInfo memorySizeUnitInfos[] =
    {
        { MemorySize::Unit::B, "B" },
        { MemorySize::Unit::KB, "KB" },
        { MemorySize::Unit::MB, "MB" },
        { MemorySize::Unit::GB, "GB" }
    };
}

HADESVM_CORE_PUBLIC QString hadesvm::util::toString(const MemorySize & value)
{
    for (size_t i = 0; i < sizeof(memorySizeUnitInfos) / sizeof(memorySizeUnitInfos[0]); i++)
    {
        if (memorySizeUnitInfos[i].unit == value.unit())
        {
            return toString(value.numberOfUnits()) + memorySizeUnitInfos[i].name;
        }
    }
    Q_ASSERT(false);
    return toString(value.toBytes());   //  ...to shut up the compiler
}

template <>
bool hadesvm::util::fromString<MemorySize>(const QString & s, qsizetype & scan, MemorySize & value)
{
    for (size_t i = 0; i < sizeof(memorySizeUnitInfos) / sizeof(memorySizeUnitInfos[0]); i++)
    {
        qsizetype prescan = scan;
        uint64_t numberOfUnits;
        if (fromString(s, prescan, numberOfUnits) &&
            s.mid(prescan).startsWith(memorySizeUnitInfos[i].name))
        {
            scan = prescan + strlen(memorySizeUnitInfos[i].name);
            value = MemorySize(numberOfUnits, memorySizeUnitInfos[i].unit);
            return true;
        }
    }
    return false;
}

//  End of hadesvm-core/MemorySize.cpp
