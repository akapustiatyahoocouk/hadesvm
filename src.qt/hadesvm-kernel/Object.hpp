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

            friend class Kernel;

            //////////
            //  Construction/destruction - from friends only
        protected:
            explicit Object(Kernel * kernel);
            virtual ~Object();

            //////////
            //  Properties
        public:
            //  The Kernel managing this Object.
            Kernel *        kernel() const { return _kernel; }

            //  The OID of this Object; unique per Kernel.
            const Oid       oid() const { return _oid; }

            //  True if this Object is "live" (and can be used), false
            //  if it is "dead" (and must be recycled).
            bool            live() const { return _live; }

            //////////
            //  Implementation
        private:
            Kernel *const   _kernel;
            const Oid       _oid;
            bool            _live;
        };
    }
}

//  End of hadesvm-kernel/Object.hpp
