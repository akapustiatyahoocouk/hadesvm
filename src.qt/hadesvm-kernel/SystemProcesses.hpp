//
//  hadesvm-kernel/SystemProcesses.hpp
//
//  hadesvm kernel system processes
//
//////////

namespace hadesvm
{
    namespace kernel
    {
        //////////
        //  The "device manager" system process

        //  The main thread of the "device manager" system process
        class HADESVM_KERNEL_PUBLIC DeviceManagerMainThread : public NativeThread
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(DeviceManagerMainThread)

            //////////
            //  Construction/destruction
        public:
            DeviceManagerMainThread(Kernel * kernel, Process * process);
            virtual ~DeviceManagerMainThread();

            //////////
            //  NativeThread
        public:
            virtual ExitCode    run() override;

            //////////
            //  Implementation
        private:
        };
    }
}

//  End of hadesvm-kernel/SystemProcesses.hpp
