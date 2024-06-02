//
//  hadesvm-core/VirtualApplianceTemplate.hpp
//
//  hadesvm-core VA template support
//
//////////

namespace hadesvm
{
    namespace core
    {
        //////////
        //  Represents a template from which a VA can be created quickly
        class HADESVM_CORE_PUBLIC VirtualApplianceTemplate : public hadesvm::util::StockObject
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(VirtualApplianceTemplate)

            //////////
            //  Construction/destruction
        public:
            VirtualApplianceTemplate();
            virtual ~VirtualApplianceTemplate();

            //////////
            //  Operations
        public:
            //  The type of the VA created by this VA template
            virtual VirtualApplianceType *  virtualApplianceType() const = 0;

            //  The architecture of the VA created by this VA template
            virtual VirtualArchitecture *   virtualArchitecture() const = 0;

            //////////
            //  Registry
        public:
            //  Registers the specified component type.
            //  Returns true on success, false on failure.
            static bool                         register(VirtualApplianceTemplate * virtualApplianceTemplate);

            //  Finds a registered component type by mnemonic; returns
            //  nullptr if not found.
            static VirtualApplianceTemplate *   findByMnemonic(const QString & mnemonic);

            //  Returns an unordered list of all registered component types.
            static VirtualApplianceTemplateList   all();
        };
    }
}

//  End of hadesvm-core/VirtualApplianceTemplate.hpp
