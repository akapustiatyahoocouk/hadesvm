//
//  hadesvm-cereon/Architectures.hpp
//
//  Cereon-based architectures
//
//////////

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The "Cereon Workstation" architecture
    class HADESVM_CEREON_PUBLIC CereonWorkstationArchitecture final : public hadesvm::core::VirtualArchitecture
        {
            HADESVM_DECLARE_SINGLETON(CereonWorkstationArchitecture)

            //////////
            //  hadesvm::util::StockObject
        public:
            virtual QString     mnemonic() const override;
            virtual QString     displayName() const override;

            //////////
            //  hadesvm::core::VirtualArchitecture
        public:
            virtual void        validateVirtualAppliance(hadesvm::core::VirtualAppliance * virtualAppliance) throws(hadesvm::core::VirtualApplianceException) override;
        };
    }
}

//  End of hadesvm-cereon/Architectures.hpp
