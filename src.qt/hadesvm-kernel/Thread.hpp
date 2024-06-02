//
//  hadesvm-kernel/Thread.hpp
//
//  hadesvm threads
//
//////////

namespace hadesvm
{
    namespace kernel
    {
        //  A Hades VM kernel thread
        class HADESVM_KERNEL_PUBLIC Thread : public Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Thread)

            //////////
            //  Construction/destruction
        public:
            Thread(Kernel * kernel, Oid oid);
            virtual ~Thread();

            //////////
            //  Properties
        public:
            Process *const  process;    //  ...to which this thread belongs
        };
    }
}

//  End of hadesvm-kernel/Thread.hpp
