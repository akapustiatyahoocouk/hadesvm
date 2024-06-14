//
//  hadesvm-core/ClockFrequency.cpp
//
//  ClockFrequency class implementation
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

//////////
//  Construction/destruction
ClockFrequency::ClockFrequency(uint64_t numberOfUnits, Unit unit)
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
uint64_t ClockFrequency::toHz() const
{
    return _numberOfUnits * static_cast<uint64_t>(_unit);
}

ClockFrequency ClockFrequency::hertz(uint64_t count)
{
    return ClockFrequency(count, Unit::Hz);
}

ClockFrequency ClockFrequency::kilohertz(uint64_t count)
{
    return ClockFrequency(count, Unit::KHz);
}

ClockFrequency ClockFrequency::megahertz(uint64_t count)
{
    return ClockFrequency(count, Unit::MHz);
}

ClockFrequency ClockFrequency::gigahertz(uint64_t count)
{
    return ClockFrequency(count, Unit::GHz);
}

int ClockFrequency::compare(const ClockFrequency & op2) const
{
    uint64_t b1 = this->toHz();
    uint64_t b2 = op2.toHz();

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
    struct ClockFrequencyUnitInfo
    {
        ClockFrequency::Unit    unit;
        const char *            name;
    };

    const ClockFrequencyUnitInfo clockFrequencyUnitInfos[] =
    {
        { ClockFrequency::Unit::Hz, "Hz" },
        { ClockFrequency::Unit::KHz, "KHz" },
        { ClockFrequency::Unit::MHz, "MHz" },
        { ClockFrequency::Unit::GHz, "GHz" }
    };
}

HADESVM_CORE_PUBLIC QString hadesvm::util::toString(const ClockFrequency & value)
{
    for (size_t i = 0; i < sizeof(clockFrequencyUnitInfos) / sizeof(clockFrequencyUnitInfos[0]); i++)
    {
        if (clockFrequencyUnitInfos[i].unit == value.unit())
        {
            return toString(value.numberOfUnits()) + clockFrequencyUnitInfos[i].name;
        }
    }
    Q_ASSERT(false);
    return toString(value.toHz());   //  ...to shut up the compiler
}

template <>
bool hadesvm::util::fromString<ClockFrequency>(const QString & s, qsizetype & scan, ClockFrequency & value)
{
    for (size_t i = 0; i < sizeof(clockFrequencyUnitInfos) / sizeof(clockFrequencyUnitInfos[0]); i++)
    {
        qsizetype prescan = scan;
        uint64_t numberOfUnits;
        if (fromString(s, prescan, numberOfUnits) &&
            s.mid(prescan).startsWith(clockFrequencyUnitInfos[i].name))
        {
            scan = prescan + strlen(clockFrequencyUnitInfos[i].name);
            value = ClockFrequency(numberOfUnits, clockFrequencyUnitInfos[i].unit);
            return true;
        }
    }
    return false;
}

//  End of hadesvm-core/ClockFrequency.cpp
