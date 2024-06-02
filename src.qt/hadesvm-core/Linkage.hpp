//
//  hadesvm-core/Linkage.hpp
//
//  hadesvm-core linkage definitions
//
//////////

#if defined(HADESVM_CORE_LIBRARY)
    #define HADESVM_CORE_PUBLIC     Q_DECL_EXPORT
#else
    #define HADESVM_CORE_PUBLIC     Q_DECL_IMPORT
#endif

//  End of hadesvm-core/Linkage.hpp
