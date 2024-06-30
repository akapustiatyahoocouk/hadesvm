//
//  hadesvm-cereon/Templates.hpp
//
//  Cereon-based VA templates
//
//////////

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The "Cereon Workstation Micro" VM template
        class HADESVM_CEREON_PUBLIC CereonWorkstationMicroTemplate final : public hadesvm::core::VirtualApplianceTemplate
        {
            HADESVM_DECLARE_SINGLETON(CereonWorkstationMicroTemplate)

            //////////
            //  hadesvm::util::StockObject
        public:
            virtual QString     mnemonic() const override;
            virtual QString     displayName() const override;

            //////////
            //  hadesvm::core::VirtualApplianceTemplate
        public:
            virtual hadesvm::core::VirtualApplianceType *   virtualApplianceType() const override;
            virtual CereonWorkstationArchitecture *         virtualArchitecture() const override;
        };
    }
}

//  End of hadesvm-cereon/Templates.hpp
