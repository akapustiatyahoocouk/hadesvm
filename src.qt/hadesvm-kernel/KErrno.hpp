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
            RangeStart = 1,

            OutOfMemory,        //  kernel heap exhausted
            InvalidParameter,   //  invalid system call parameter
            QueueFull,          //  server queue is full
            NotImplemented,     //  interface or method not implemented

            //  Miscellaneous
            OK = 0x00000000,
            Unknown = 0xFFFFFFFF
        };
    }
}

//  End of hadesvm-kernel/KErrno.hpp
