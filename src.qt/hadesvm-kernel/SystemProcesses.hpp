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
        class HADESVM_KERNEL_PUBLIC DeviceManagerProcess
        {
            HADESVM_UTILITY_CLASS(DeviceManagerProcess)

            //////////
            //  Constants
        public:
            //  The name of the process
            static const QString    Name;

            //////////
            //  Threads
        public:
            //  The main thread of the "device manager" system process
            class HADESVM_KERNEL_PUBLIC MainThread : public NativeThread
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(MainThread)

                //////////
                //  Construction/destruction
            public:
                MainThread(Kernel * kernel, Process * process);
                virtual ~MainThread();

                //////////
                //  NativeThread
            public:
                virtual ExitCode    run() override;

                //////////
                //  Implementation
            private:
                //  Atoms
                Oid                 _interfaceAtomOid_IUnknown = Oid::Invalid;
                Oid                 _serviceAtomOid_IUnknown_QueryInterface = Oid::Invalid;
                Oid                 _serviceAtomOid_IUnknown_HandleOpen = Oid::Invalid;
                Oid                 _serviceAtomOid_IUnknown_HandleClosed = Oid::Invalid;

                //  Handles
                Handle              _serviceHandle = Handle::Invalid;
            };
        };
    }
}

//  End of hadesvm-kernel/SystemProcesses.hpp
