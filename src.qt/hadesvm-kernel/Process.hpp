//
//  hadesvm-kernel/Process.hpp
//
//  hadesvm processes
//
//////////

namespace hadesvm
{
    namespace kernel
    {
        //////////
        //  A generic Hades VM kernel process
        class HADESVM_KERNEL_PUBLIC Process : public Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Process)

            //////////
            //  Types
        public:
            enum class State
            {
                Constructed,    //  process conbstructed, but not yet started
                Running,        //  process conbstructed, and started, but not yet finished
                Suspended,      //  process conbstructed, and started, but is currently suspended
                Finished        //  process has finished; exit code available
            };

            //////////
            //  Construction/destruction
        public:
            Process(Kernel * kernel, Oid oid);
            virtual ~Process();

            //////////
            //  Implementation
        private:
            ThreadList      _threads;   //  all threads of this process, dead or alive
        };
    }
}

//  End of hadesvm-kernel/Process.hpp
