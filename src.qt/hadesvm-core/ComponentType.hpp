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
    }
}

//  End of hadesvm-core/ComponentType.hpp
