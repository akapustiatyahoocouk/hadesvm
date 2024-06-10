//
//  hadesvm-kernel/StorageDevices.hpp
//
//  hadesvm kernel storage devices
//
//////////

namespace hadesvm
{
    namespace kernel
    {
        //  TODO document
        class HADESVM_KERNEL_PUBLIC MountedFolderDevice : public Device
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(MountedFolderDevice)

            //////////
            //  Construction/destruction - in "kernel locked" state only
        public:
            MountedFolderDevice(Kernel * kernel, Node * node,
                                const QString & deviceName, const QString & externalFolderPath);
            virtual ~MountedFolderDevice();

            //////////
            //  Operations - in "kernel locked" state only
        public:
            //  The full path to the external folder of the host OS.
            QString         externalFolderPath() const;

            //////////
            //  Implementation
        private:
            const QString   _externalFolderPath;
        };
    }
}

//  End of hadesvm-kernel/StorageDevices.hpp
