//
//  hadesvm-util/Math.hpp
//
//  Math helpers
//
//////////

namespace hadesvm
{
    namespace util
    {
        class HADESVM_UTIL_PUBLIC Math final
        {
            HADESVM_UTILITY_CLASS(Math)

            //////////
            //  Operations (fp)
        public:
            //  Checks if a floating-point value is a NaN.
            static bool                 isNan(float x);
            static bool                 isNan(double x);

            //  Checks if a floating-point value is a signaling NaN.
            static bool                 isSignalingNan(float x);
            static bool                 isSignalingNan(double x);

            //  Checks if a floating-point value is a quiet NaN.
            static bool                 isQuietNan(float x);
            static bool                 isQuietNan(double x);

            //  Checks if a floating-point value is an infinity.
            static bool                 isInfinite(float x);
            static bool                 isInfinite(double x);

            //  Checks if a floating-point value is a positive infinity.
            static bool                 isPositiveInfinite(float x);
            static bool                 isPositiveInfinite(double x);

            //  Checks if a floating-point value is a negative infinity.
            static bool                 isNegativeInfinite(float x);
            static bool                 isNegativeInfinite(double x);
        };
    }
}

//  End of hadesvm-util/Math.hpp
