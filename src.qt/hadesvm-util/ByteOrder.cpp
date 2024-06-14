//
//  hadesvm-util/ByteOrder.cpp
//
//  The hadesvm::util::ByteOrder enum implementation
//
//////////
#include "hadesvm-util/API.hpp"
using namespace hadesvm::util;

namespace
{
    struct ByteOrderInfo
    {
        ByteOrder       byteOrder;
        const char *    name;
    };

    const ByteOrderInfo byteOrderInfos[] =
    {
        { ByteOrder::Unknown, "Unknown" },
        { ByteOrder::BigEndian, "BigEndian" },
        { ByteOrder::LittleEndian, "LittleEndian" },
    };
}

//////////
//  Formatting and parsing
HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(ByteOrder value)
{
    for (size_t i = 0; i < sizeof(byteOrderInfos) / sizeof(byteOrderInfos[0]); i++)
    {
        if (byteOrderInfos[i].byteOrder == value)
        {
            return byteOrderInfos[i].name;
        }
    }
    return "Unknown";
}

template <>
bool hadesvm::util::fromString<ByteOrder>(const QString & s, qsizetype & scan, ByteOrder & value)
{
    for (size_t i = 0; i < sizeof(byteOrderInfos) / sizeof(byteOrderInfos[0]); i++)
    {
        if (s.mid(scan).startsWith(byteOrderInfos[i].name))
        {
            value = byteOrderInfos[i].byteOrder;
            scan += strlen(byteOrderInfos[i].name);
            return true;
        }
    }
    return false;
}

//  End of hadesvm-util/ByteOrder.cpp
