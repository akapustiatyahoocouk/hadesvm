//
//  hadesvm-core/ComponentAdaptorType.hpp
//
//  hadesvm-core component adaptor type support
//
//////////

namespace hadesvm
{
    namespace core
    {
        //////////
        //  Represents a general type of component adaptors.
        //  ALSO acts as factory for component adaptors of this type.
        class HADESVM_CORE_PUBLIC ComponentAdaptorType : public hadesvm::util::StockObject
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ComponentAdaptorType)

            //////////
            //  Construction/destruction
        public:
            ComponentAdaptorType();
            virtual ~ComponentAdaptorType();

            //////////
            //  hadesvm::util::StockObject
        public:
            virtual QString         mnemonic() const override;
            virtual QString         displayName() const override;

            //////////
            //  Operations
        public:
            //  Checks if a component adaptor of this type can be "suspended".
            virtual bool            suspendable() const = 0;

            //  Returns the component type adapted by this adaptor type.
            virtual ComponentType * adaptedComponentType() const = 0;

            //  Returns the architecture to which the relevant component
            //  type is adapted.
            virtual VirtualArchitecture *   adaptedToArchitecture() const = 0;

            //  Creates a new adaptor for the specified component.
            virtual ComponentAdaptor * createComponentAdaptor(Component * component) = 0;

            //////////
            //  Registry
        public:
            //  Registers the specified component adaptor type.
            //  Returns true on success, false on failure.
            static bool                     register(ComponentAdaptorType * componentAdaptorType);

            //  Finds a registered component adaptor type by mnemonic;
            //  returns nullptr if not found.
            static ComponentAdaptorType *   findByMnemonic(const QString & mnemonic);

            //  If there exists a component adaptor type for the specified
            //  combination of component type and architecture returns it;
            //  otherwise returns nullptr.
            static ComponentAdaptorType *   find(ComponentType * componentType, VirtualArchitecture * architecture);

            //  Returns an unordered list of all registered component adaptor types.
            static ComponentAdaptorTypeList all();
        };
    }
}

//  End of hadesvm-core/ComponentAdaptorType.hpp
