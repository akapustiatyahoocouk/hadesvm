//
//  hadesvm-util/Linkage.hpp
//
//  hadesvm-util linkage definitions
//
//////////

#if defined(HADESVM_UTIL_LIBRARY)
    #define HADESVM_UTIL_PUBLIC     Q_DECL_EXPORT
#else
    #define HADESVM_UTIL_PUBLIC     Q_DECL_IMPORT
#endif

//  End of hadesvm-util/Linkage.hpp
