//
//  hadesvm-kernel/ComponentInterfaces.hpp
//
//  hadesvm kernel Component interfaces recognized by the Kernel
//
//////////

namespace hadesvm
{
    namespace kernel
    {
        //  An interface to a "device component" - a component or component
        //  adaptor that will be managed as a Device within a HADES instance
        class HADESVM_KERNEL_PUBLIC IDeviceComponent
        {
            //////////
            //  Construction/destruction
        public:
            IDeviceComponent() = default;
            virtual ~IDeviceComponent() = default;

            //////////
            //  Operations  (general)
        public:
            //  The name of this device, as it will be known to the HADES kernel.
            virtual QString     deviceName() const = 0;
        };

        //////////
        //  Mass storage

        //  A device component that offers external storage capabilities
        class HADESVM_KERNEL_PUBLIC IMassStorageDeviceComponent : public virtual IDeviceComponent
        {
            //////////
            //  Construction/destruction
        public:
            IMassStorageDeviceComponent() = default;
            virtual ~IMassStorageDeviceComponent() = default;

            //////////
            //  Operations  (general)
        public:
        };

        //  A mass storage device component that represents an external unit
        //  that implements its own hierarchical file system (NAS storage, etc.)
        class HADESVM_KERNEL_PUBLIC IExternalFileSystemComponent : public virtual IMassStorageDeviceComponent
        {
            //////////
            //  Construction/destruction
        public:
            IExternalFileSystemComponent() = default;
            virtual ~IExternalFileSystemComponent() = default;

            //////////
            //  Operations (general)
        public:
            //  Checke whether the external file system is case-sensitive
            //  and/or case-preserving
            virtual bool        isCaseSensitive() const = 0;
            virtual bool        isCasePreserving() const = 0;

            //  Checks whether the specified character represents a separator
            //  of path components used by the external file system.
            virtual bool        isPathComponentSeparator(QChar c) const = 0;
            //  The canonical separator of path components used by the
            //  external file system.
            virtual QChar       pathComponentSeparator() const = 0;

            //  Checks whether the specified character represents a separator
            //  between filename and extension used by the external file system.
            virtual bool        isExtensionSeparator(QChar c) const = 0;
            //  The canonical separator of filename and extension used by the
            //  external file system.
            virtual QChar       extensionSeparator() const = 0;

            //  Checks whether the specified string is valid as a single path
            //  component for the external file system.
            virtual bool        isValidPathComponent(const QString & pathComponent) const = 0;
            //  Checks whether the specified string is valid as a complete path
            //  for the external file system.
            virtual bool        isValidPath(const QString & path) const = 0;
        };
    }
}

//  End of hadesvm-kernel/ComponentInterfaces.hpp
