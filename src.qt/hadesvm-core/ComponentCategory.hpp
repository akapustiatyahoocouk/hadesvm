//
//  hadesvm-core/ComponentCategory.hpp
//
//  hadesvm-core component category support
//
//////////

namespace hadesvm
{
    namespace core
    {
        //////////
        //  Represents a general category of components (e.g. "processors",
        //  "memory", "external storage", etc.
        class HADESVM_CORE_PUBLIC ComponentCategory : public hadesvm::util::StockObject
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ComponentCategory)

            //////////
            //  Construction/destruction
        public:
            ComponentCategory();
            virtual ~ComponentCategory();

            //////////
            //  Operations
        public:
            //  An unordered list of all component types belonging to
            //  this component category.
            ComponentTypeList   componentTypes() const;

            //////////
            //  Registry
        public:
            //  Registers the specified component category.
            //  Returns true on success, false on failure.
            static bool                     register(ComponentCategory * componentCategory);

            //  Finds a registered component category by mnemonic; returns
            //  nullptr if not found.
            static ComponentCategory *      findByMnemonic(const QString & mnemonic);

            //  Returns an unordered list of all registered component categories.
            static ComponentCategoryList    all();
        };
    }
}

//  End of hadesvm-core/ComponentCategory.hpp
