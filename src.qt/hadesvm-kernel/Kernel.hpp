//
//  hadesvm-kernel/Kernel.hpp
//
//  hadesvm kernel
//
//////////

namespace hadesvm
{
    namespace kernel
    {
        //  A Hades VM kernel
        class HADESVM_KERNEL_PUBLIC Kernel final : public hadesvm::core::Component
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Kernel)

            //////////
            //  Types
        public:
            //  The type of a Kernel component
            class HADESVM_KERNEL_PUBLIC Type final : public hadesvm::core::ComponentType
            {
                HADESVM_DECLARE_SINGLETON(Type);

                //////////
                //  hadesvm::util::StockObject
            public:
                virtual QString     mnemonic() const override;
                virtual QString     displayName() const override;

                //////////
                //  hadesvn::core::ComponentType
            public:
                virtual hadesvm::core::ComponentCategory * category() const override;
                virtual bool        suspendable() const override;
                virtual bool        isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const override;
                virtual bool        isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const override;
                virtual hadesvm::core::Component * createComponent() override;
            };

            //////////
            //  Construction/destruction
        public:
            Kernel();
            virtual ~Kernel();

            //////////
            //  hadesvm::core::Component
        public:
            virtual Type *      type() const override;
            virtual QString     displayName() const override;
            virtual void        serialiseConfiguration(QDomElement componentElement) override;
            virtual void        deserialiseConfiguration(QDomElement componentElement) override;

            //////////
            //  Implementation
        private:
            QRecursiveMutex     _guard;     //  for synchronizing access to all kernel data
            QMap<Oid, Object*>  _objects;   //  all existing+live kernel objects
        };
    }
}

//  End of hadesvm-kernel/Kernel.hpp
