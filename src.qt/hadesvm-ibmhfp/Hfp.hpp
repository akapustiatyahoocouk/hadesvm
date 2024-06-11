//
//  hadesvm-ibmhfp/Interface/Hfp.hpp
//
//  HFP API
//
//////////

namespace hadesvm
{
    namespace ibmhfp
    {
        //////////
        //  Types
        struct HADESVM_IBMHFP_PUBLIC Environment
        {
            //  Exceptions
            bool        overflow = false;
            bool        underflow = false;
            bool        significance = false;
            bool        divide = false;
        };

        //////////
        //  API
        HADESVM_IBMHFP_PUBLIC uint32_t  add32(uint32_t a, uint32_t b, Environment & env);
        HADESVM_IBMHFP_PUBLIC uint32_t  subtract32(uint32_t a, uint32_t b, Environment & env);
        HADESVM_IBMHFP_PUBLIC uint32_t  multiply32(uint32_t a, uint32_t b, Environment & env);
        HADESVM_IBMHFP_PUBLIC uint32_t  divide32(uint32_t a, uint32_t b, Environment & env);
        HADESVM_IBMHFP_PUBLIC int       compare32(uint32_t a, uint32_t b, Environment & env);

        HADESVM_IBMHFP_PUBLIC uint64_t  add64(uint64_t a, uint64_t b, Environment & env);
        HADESVM_IBMHFP_PUBLIC uint64_t  subtract64(uint64_t a, uint64_t b, Environment & env);
        HADESVM_IBMHFP_PUBLIC uint64_t  multiply64(uint64_t a, uint64_t b, Environment & env);
        HADESVM_IBMHFP_PUBLIC uint64_t  divide64(uint64_t a, uint64_t b, Environment & env);
        HADESVM_IBMHFP_PUBLIC int       compare64(uint64_t a, uint64_t b, Environment & env);

        HADESVM_IBMHFP_PUBLIC uint32_t  fromInt32(int a);
        HADESVM_IBMHFP_PUBLIC uint64_t  fromInt64(int a);
        HADESVM_IBMHFP_PUBLIC double    toDouble32(uint32_t a);
        HADESVM_IBMHFP_PUBLIC double    toDouble64(uint64_t a);
    }
}

//  End of hadesvm-ibmhfp/Interface/Hfp.hpp
