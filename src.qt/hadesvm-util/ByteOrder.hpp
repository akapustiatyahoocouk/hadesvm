//
//  hadesvm-util/ByteOrder.hpp
//
//  Byte order handling
//
//////////

namespace hadesvm
{
    namespace util
    {
        enum class ByteOrder : uint8_t
        {
            Unknown = 0,
            BigEndian = 1,
            LittleEndian = 2,
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
            Native = BigEndian
#elif Q_BYTE_ORDER == Q_LITTLE_ENDIAN
            Native = LittleEndian
#endif
        };

        //////////
        //  Formatting and parsing
        HADESVM_UTIL_PUBLIC QString toString(ByteOrder value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<ByteOrder>(const QString & s, qsizetype & scan, ByteOrder & value);
    }
}

//  End of hadesvm-util/ByteOrder.hpp
