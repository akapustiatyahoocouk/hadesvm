//
//  hadesvm-ibmhfp/Linkage.hpp
//
//  hadesvm-ibmhfp linkage definitions
//
//////////

#if defined(HADESVM_IBMHFP_LIBRARY)
    #define HADESVM_IBMHFP_PUBLIC   Q_DECL_EXPORT
#else
    #define HADESVM_IBMHFP_PUBLIC   Q_DECL_IMPORT
#endif

//  End of hadesvm-ibmhfp/Linkage.hpp
