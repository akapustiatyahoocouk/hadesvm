//
//  hadesvm-core/TimeInterval.cpp
//
//  TimeInterval class implementation
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

//////////
//  Construction/destruction
TimeInterval::TimeInterval(uint64_t numberOfUnits, Unit unit)
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
uint64_t TimeInterval::toNs() const
{
    return _numberOfUnits * static_cast<uint64_t>(_unit);
}

TimeInterval TimeInterval::seconds(uint64_t count)
{
    return TimeInterval(count, Unit::S);
}

TimeInterval TimeInterval::microseconds(uint64_t count)
{
    return TimeInterval(count, Unit::Us);
}

TimeInterval TimeInterval::milliseconds(uint64_t count)
{
    return TimeInterval(count, Unit::Ms);
}

TimeInterval TimeInterval::nanoseconds(uint64_t count)
{
    return TimeInterval(count, Unit::Ns);
}

int TimeInterval::compare(const TimeInterval & op2) const
{
    uint64_t b1 = this->toNs();
    uint64_t b2 = op2.toNs();

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
    struct TimeIntervalUnitInfo
    {
        TimeInterval::Unit    unit;
        const char *            name;
    };

    const TimeIntervalUnitInfo timeIntervalUnitInfos[] =
    {
        { TimeInterval::Unit::S, "s" },
        { TimeInterval::Unit::Ms, "ms" },
        { TimeInterval::Unit::Us, "us" },
        { TimeInterval::Unit::Ns, "ns" }
};
}

HADESVM_CORE_PUBLIC QString hadesvm::util::toString(const TimeInterval & value)
{
    for (size_t i = 0; i < sizeof(timeIntervalUnitInfos) / sizeof(timeIntervalUnitInfos[0]); i++)
    {
        if (timeIntervalUnitInfos[i].unit == value.unit())
        {
            return toString(value.numberOfUnits()) + timeIntervalUnitInfos[i].name;
        }
    }
    failure();
    return toString(value.toNs());   //  ...to shut up the compiler
}

template <>
bool hadesvm::util::fromString<TimeInterval>(const QString & s, qsizetype & scan, TimeInterval & value)
{
    for (size_t i = 0; i < sizeof(timeIntervalUnitInfos) / sizeof(timeIntervalUnitInfos[0]); i++)
    {
        qsizetype prescan = scan;
        uint64_t numberOfUnits;
        if (fromString(s, prescan, numberOfUnits) &&
            s.mid(prescan).startsWith(timeIntervalUnitInfos[i].name))
        {
            scan = prescan + strlen(timeIntervalUnitInfos[i].name);
            value = TimeInterval(numberOfUnits, timeIntervalUnitInfos[i].unit);
            return true;
        }
    }
    return false;
}

//  End of hadesvm-core/TimeInterval.cpp
