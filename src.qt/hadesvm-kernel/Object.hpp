//
//  hadesvm-kernel/Object.hpp
//
//  hadesvm kernel objects
//
//////////

namespace hadesvm
{
    namespace kernel
    {
        //  An OID of a kernel object - unique per kernel instance
        typedef uint32_t Oid;

        //  A common base class for all kernel objects
        class HADESVM_KERNEL_PUBLIC Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Object)

            //////////
            //  Construction/destruction
        public:
            Object(Kernel * kernel, Oid oid);
            virtual ~Object();

            //////////
            //  Properties
        public:
            Kernel *const   kernel;
            const Oid       oid;
        };
    }
}

//  End of hadesvm-kernel/Object.hpp
