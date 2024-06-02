//
//  hadesvm-util/StockObject.hpp
//
//  "Stock Object" ADT
//
//////////

namespace hadesvm
{
    namespace util
    {
        //////////
        //  A "stock object" is a generic object that comes from a
        //  fixed stock of same-typed objects
        //  Concrete subclasses of this class will normally be singletons
        //  registered by plugins.
        class HADESVM_UTIL_PUBLIC StockObject
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(StockObject)

            //////////
            //  Construction/destruction
        public:
            StockObject() = default;
            virtual ~StockObject() = default;

            //////////
            //  Operations
        public:
            //  The locale-independent internal mnemonic identifier of
            //  this stock object.
            virtual QString     mnemonic() const = 0;

            //  The user-readable display name of this stock object.
            virtual QString     displayName() const = 0;
        };
    }
}

//  End of hadesvm-util/StockObject.hpp
