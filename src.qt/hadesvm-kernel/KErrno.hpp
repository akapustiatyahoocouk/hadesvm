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
            OutOfMemory         = 0x00000001,   //  kernel heap exhausted
            InvalidParameter    = 0x00000002,   //  invalid system call parameter
            QueueFull           = 0x00000003,   //  server queue is full
            NotImplemented      = 0x00000004,   //  interface or method not implemented
            LimitReached        = 0x00000005,   //  counter limit reached, etc.
            AlreadyExists       = 0x00000006,   //  object already exists
            DoesNotExist        = 0x00000007,   //  object does not exist

            //  Miscellaneous
            OK                  = 0x00000000,
            Unknown             = 0xFFFFFFFF
        };
    }
}

//  End of hadesvm-kernel/KErrno.hpp
