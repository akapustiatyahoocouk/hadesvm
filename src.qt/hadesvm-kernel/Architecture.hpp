//
//  hadesvm-kernel/Architecture.hpp
//
//  hadesvm-kernel architecture
//
//////////

namespace hadesvm
{
    namespace kernel
    {
        //////////
        //  The HadesVM/kernel architecture - a HADES VM with soft kernel
        class HADESVM_KERNEL_PUBLIC Architecture final : public hadesvm::core::VirtualArchitecture
        {
            HADESVM_DECLARE_SINGLETON(Architecture)

            //////////
            //  hadesvm::util::StockObject
        public:
            virtual QString     mnemonic() const override;
            virtual QString     displayName() const override;
        };
    }
}

//  End of hadesvm-kernel/Architecture.hpp
