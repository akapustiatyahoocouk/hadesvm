//
//  hadesvm-cereon/Linkage.hpp
//
//  hadesvm-cereon linkage definitions
//
//////////

#if defined(HADESVM_CEREON_LIBRARY)
    #define HADESVM_CEREON_PUBLIC   Q_DECL_EXPORT
#else
    #define HADESVM_CEREON_PUBLIC   Q_DECL_IMPORT
#endif

//  End of hadesvm-cereon/Linkage.hpp
