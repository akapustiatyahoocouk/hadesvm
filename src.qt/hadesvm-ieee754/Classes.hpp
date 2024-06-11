//
//  hadesvm-ieee754/Classes.hpp
//
//  Forward declarations & helper types
//
//////////

namespace hadesvm
{
    namespace ieee754
    {
        //////////
        //  The "class" of a real value
        enum class ValueClass
        {
            Zero,
            PositiveDenormalized,
            NegativeDenormalized,
            PositiveNormalized,
            NegativeNormalized,
            PositiveInfinity,
            NegativeInfinity,
            QuietNan,
            SignallingNan
        };

        class HADESVM_IEEE754_PUBLIC Real32;
        class HADESVM_IEEE754_PUBLIC Real64;
    }
}

//  End of hadesavm-ieee754/Classes.hpp
