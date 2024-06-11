//
//  hadesvm-ieee754/Linkage.hpp
//
//  hadesvm-ieee754 linkage definitions
//
//////////

#if defined(HADESVM_IEEE754_LIBRARY)
    #define HADESVM_IEEE754_PUBLIC   Q_DECL_EXPORT
#else
    #define HADESVM_IEEE754_PUBLIC   Q_DECL_IMPORT
#endif

//  End of hadesvm-ieee754/Linkage.hpp
