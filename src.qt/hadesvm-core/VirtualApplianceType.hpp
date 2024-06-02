//
//  hadesvm-core/VirtualApplianceTemplateType.hpp
//
//  hadesvm-core VA type support
//
//////////

namespace hadesvm
{
    namespace core
    {
        //////////
        //  Represents a VA type
        class HADESVM_CORE_PUBLIC VirtualApplianceType : public hadesvm::util::StockObject
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(VirtualApplianceType)

            //////////
            //  Construction/destruction
        public:
            VirtualApplianceType();
            virtual ~VirtualApplianceType();

            //////////
            //  Operations
        public:
            //  Creates a new VA of this type, initially without any components
            virtual VirtualAppliance *  createVirtualAppliance(const QString & name, const QString & location,
                                                               VirtualArchitecture * architecture) = 0;

            //////////
            //  Registry
        public:
            //  Registers the specified VA type.
            //  Returns true on success, false on failure.
            static bool                         register(VirtualApplianceType * virtualApplianceType);

            //  Finds a registered VA type by mnemonic; returns
            //  nullptr if not found.
            static VirtualApplianceType *       findByMnemonic(const QString & mnemonic);

            //  Returns an unordered list of all registered VA types.
            static VirtualApplianceTypeList     all();
        };

        //////////
        //  Represents a "virtual machine" VA type
        class HADESVM_CORE_PUBLIC VirtualMachineType final : public VirtualApplianceType
        {
            HADESVM_DECLARE_SINGLETON(VirtualMachineType)

            //////////
            //  hadesvm::util::StockObject
        public:
            virtual QString     mnemonic() const override;
            virtual QString     displayName() const override;

            //////////
            //  VirtualApplianceType
        public:
            virtual VirtualAppliance *  createVirtualAppliance(const QString & name, const QString & location,
                                                               VirtualArchitecture * architecture);

            //////////
            //  Operations
        public:
            //  Creates a new WM of this type, initially without any components.
            //  Default implementation creates an instance of VirtualMachine, but
            //  overriding implementation may subclass VirtualMachine and return that.
            virtual VirtualMachine *    createVirtualMachine(const QString & name, const QString & location,
                                                             VirtualArchitecture * architecture);

            //////////
            //  Registry
        public:
            //  Registers the specified VM type.
            //  Returns true on success, false on failure.
            static bool                     register(VirtualMachineType * virtualMachineType);

            //  Finds a registered VM type by mnemonic; returns
            //  nullptr if not found.
            static VirtualMachineType *     findByMnemonic(const QString & mnemonic);

            //  Returns an unordered list of all registered VM types.
            static VirtualMachineTypeList   all();
        };

        //////////
        //  Represents a "remote terminal" VA type
        class HADESVM_CORE_PUBLIC RemoteTerminalType final : public VirtualApplianceType
        {
            HADESVM_DECLARE_SINGLETON(RemoteTerminalType)

            //////////
            //  hadesvm::util::StockObject
        public:
            virtual QString     mnemonic() const override;
            virtual QString     displayName() const override;

            //////////
            //  VirtualApplianceType
        public:
            virtual VirtualAppliance *  createVirtualAppliance(const QString & name, const QString & location,
                                                               VirtualArchitecture * architecture);

            //////////
            //  Operations
        public:
            //  Creates a new RT of this type, initially without any components
            //  Default implementation creates an instance of RemoteTerminal, but
            //  overriding implementation may subclass RemoteTerminal and return that.
            virtual RemoteTerminal *    createRemoteTerminal(const QString & name, const QString & location,
                                                             VirtualArchitecture * architecture);

            //////////
            //  Registry
        public:
            //  Registers the specified RT type.
            //  Returns true on success, false on failure.
            static bool                     register(RemoteTerminalType * remoteTerminalType);

            //  Finds a registered RT type by mnemonic; returns
            //  nullptr if not found.
            static RemoteTerminalType *     findByMnemonic(const QString & mnemonic);

            //  Returns an unordered list of all registered RT types.
            static RemoteTerminalTypeList   all();
        };
    }
}

//  End of hadesvm-core/VirtualApplianceType.hpp
