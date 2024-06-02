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
        class HADESVM_KERNEL_PUBLIC Kernel final
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Kernel)

            //////////
            //  Construction/destruction
        public:
            Kernel();
            virtual ~Kernel();

            //////////
            //  Implementation
        private:
            QRecursiveMutex     _guard;     //  for synchronizing access to all kernel data
            QMap<Oid, Object*>  _objects;   //  all existing+live kernel objects
        };
    }
}

//  End of hadesvm-kernel/Kernel.hpp
