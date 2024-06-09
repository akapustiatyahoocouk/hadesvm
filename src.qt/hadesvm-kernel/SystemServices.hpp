//
//  hadesvm-kernel/SystemServices.hpp
//
//  hadesvm-kernel system service names and versions
//
//////////

namespace hadesvm
{
    namespace kernel
    {
        class HADESVM_KERNEL_PUBLIC SystemServices final
        {
            HADESVM_UTILITY_CLASS(SystemServices)

            //  The "device manager" service
        public:
            class HADESVM_KERNEL_PUBLIC DeviceManager final
            {
                HADESVM_UTILITY_CLASS(DeviceManager)

                //////////
                //  Constants
            public:
                //  The name of the service
                static const QString        Name;

                //  The version of the service
                static const unsigned int   Version;
            };
        };
    }
}

//  End of hadesvm-kernel/SystemServices.hpp
