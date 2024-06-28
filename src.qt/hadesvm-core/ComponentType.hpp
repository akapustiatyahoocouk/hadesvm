//
//  hadesvm-core/ComponentType.hpp
//
//  hadesvm-core component type support
//
//////////

namespace hadesvm
{
    namespace core
    {
        //////////
        //  Represents a general type of components (e.g. "i8086 processor",
        //  "3'5 FDD", etc.
        //  ALSO acts as factory for components of this type.
        class HADESVM_CORE_PUBLIC ComponentType : public hadesvm::util::StockObject
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ComponentType)

            //////////
            //  Construction/destruction
        public:
            ComponentType();
            virtual ~ComponentType();

            //////////
            //  Operations
        public:
            //  The component category to which this component belongs.
            virtual ComponentCategory * category() const = 0;

            //  Checks if a component of this type can be "suspended".
            virtual bool        suspendable() const = 0;

            //  TODO document
            virtual bool        isCompatibleWith(VirtualArchitecture * architecture) const = 0;
            virtual bool        isCompatibleWith(VirtualApplianceType * type) const = 0;
            virtual bool        isAdaptableTo(VirtualArchitecture * architecture) const;

            virtual Component * createComponent() = 0;

            //////////
            //  Registry
        public:
            //  Registers the specified component type.
            //  Returns true on success, false on failure.
            static bool                 register(ComponentType * componentType);

            //  Finds a registered component type by mnemonic; returns
            //  nullptr if not found.
            static ComponentType *      findByMnemonic(const QString & mnemonic);

            //  Returns an unordered list of all registered component types.
            static ComponentTypeList    all();
        };

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
            QString                 mnemonic() const override;
            QString                 displayName() const override;

            //////////
            //  Operations
        public:
            //  Returns the component type adapted by this adaptor type.
            virtual ComponentType * adaptedComponentType() const = 0;

            //  Returns the architecture to which the relevant component
            //  type is adapted.
            virtual VirtualArchitecture *   adaptedToArchitecture() const = 0;

            //  The component category to which this component adaptor belongs.
            virtual ComponentCategory * category() const = 0;

            //  Checks if a component adaptor of this type can be "suspended".
            virtual bool        suspendable() const = 0;

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

//  End of hadesvm-core/ComponentType.hpp
