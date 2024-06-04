//
//  hadesvm-kernel/KErrno.hpp
//
//  hadesvm-kernel error codes
//
//////////

namespace hadesvm
{
    namespace kernel
    {
        //  kernel error codes
        enum class KErrno : uint32_t
        {
            //  Miscellaneous
            OK = 0x00000000,
            Unknown = 0xFFFFFFFF
        };
    }
}

//  End of hadesvm-kernel/KErrno.hpp
