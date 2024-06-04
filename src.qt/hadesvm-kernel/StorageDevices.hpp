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
            //  Construction/destruction - from friends only
        protected:
            MountedFolderDevice(Kernel * kernel,
                                const QString & deviceName, const QString & externalFolderPath);
            virtual ~MountedFolderDevice();

            //////////
            //  Operations
        public:

            //////////
            //  Implementation
        private:
            const QString   _externalFolderPath;
        };
    }
}

//  End of hadesvm-kernel/StorageDevices.hpp
