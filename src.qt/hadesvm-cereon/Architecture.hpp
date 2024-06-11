//
//  hadesvm-cereon/Architecture.hpp
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
        };
    }
}

//  End of hadesvm-cereon/Architecture.hpp
