//
//  hadesvm-kernel/Linkage.hpp
//
//  hadesvm-kernel linkage definitions
//
//////////

#if defined(HADESVM_KERNEL_LIBRARY)
    #define HADESVM_KERNEL_PUBLIC   Q_DECL_EXPORT
#else
    #define HADESVM_KERNEL_PUBLIC   Q_DECL_IMPORT
#endif

//  End of hadesvm-kernel/Linkage.hpp
