//
//  hadesvm-kernel/Classes.hpp
//
//  hadesvm-kernel forward declarations and typedefs
//
//////////

namespace hadesvm
{
    namespace kernel
    {
        class HADESVM_KERNEL_PUBLIC Kernel;
        class HADESVM_KERNEL_PUBLIC KernelEditor;
        class HADESVM_KERNEL_PUBLIC MountedFolderDialog;

        class HADESVM_KERNEL_PUBLIC Object;
        class HADESVM_KERNEL_PUBLIC Node;
        class HADESVM_KERNEL_PUBLIC Process;
        class HADESVM_KERNEL_PUBLIC Thread;

        using ThreadList = QList<Thread*>;
    }
}

//  End of hadesvm-kernel/Classes.hpp
