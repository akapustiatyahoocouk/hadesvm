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
        enum class ByteOrder
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
    }
}

//  End of hadesvm-util/ByteOrder.hpp
