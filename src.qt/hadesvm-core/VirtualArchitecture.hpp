//
//  hadesvm-core/VirtualArchitecture.hpp
//
//  hadesvm-core VA architecture support
//
//////////

namespace hadesvm
{
    namespace core
    {
        //////////
        //  Represents a VA architecture
        class HADESVM_CORE_PUBLIC VirtualArchitecture : public hadesvm::util::StockObject
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(VirtualArchitecture)

            //////////
            //  Construction/destruction
        public:
            VirtualArchitecture();
            virtual ~VirtualArchitecture();

            //////////
            //  Operations
        public:
            //  Performs validation of a Stopped VA.
            //  Concrete VirtualArchitectures will impose their own rules in
            //  addition to the basic rules enforced by the default implementation
            //  of this method; therefore an overriding method MUYST call this
            //  base method before performing extra validations.
            //  Throws VirtualApplianceException is VA validation fails.
            virtual void        validateVirtualAppliance(VirtualAppliance * virtualAppliance) throws(VirtualApplianceException);

            //////////
            //  Registry
        public:
            //  Registers the specified VA architecture.
            //  Returns true on success, false on failure.
            static bool                     register(VirtualArchitecture * virtualArchitecture);

            //  Finds a registered VA architecture by mnemonic; returns
            //  nullptr if not found.
            static VirtualArchitecture *    findByMnemonic(const QString & mnemonic);

            //  Returns an unordered list of all registered VA architectures.
            static VirtualArchitectureList  all();
        };
    }
}

//  End of hadesvm-core/VirtualArchitecture.hpp
