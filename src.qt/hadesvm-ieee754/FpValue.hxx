//
//  hadesvm-ieee754/FpValue.hpp
//
//  An internal form of a real value
//
//////////

namespace hadesvm
{
    namespace ieee754
    {
        //////////
        //  An internal (parsed) representation of an ieee754 value.
        //  The following format is used:
        //  *   "isSignalingNan" == true -> the value is a signaling NaN.
        //      The "signBit" contains the value's sign bit. The "mantissa"
        //      contains the left-aligned mantissa of the NaN (i.e. the highest
        //      bit of "mantissa" is the highest bit of the actual mantissa, etc.)
        //  *   "isQuietNan" == true -> the value is a quiet NaN.
        //      The "signBit" contains the value's sign bit. The "mantissa"
        //      contains the left-aligned mantissa of the NaN (i.e. the highest
        //      bit of "mantissa" is the highest bit of the actual mantissa, etc.)
        //  *   "isInfinity" == true -> the value is an infinity. The "signBit"
        //      is 0 for positive infinity, 0 for negative infinity
        //  *   "exponent" == 0 && "mantissa" == 0 -> value is +0.0.
        //  *   "mantissa" != 0 -> value is {+|-}0."mantissa" * 2**"exponent". The
        //      "signBit" is 0 for positive values, 1 for negative values. The
        //      "mantissa" is always normalized (i.e. its highest bit is always 1).
        struct FpValue
        {
            //////////
            //  Construction/destruction/assignment
            FpValue();
            FpValue(bool isSignalingNan, bool isQuietNan, bool isInfinity,
                    uint8_t signBit, int16_t exponent, uint64_t mantissa);
            explicit FpValue(double value);

            //////////
            //  Properties
            bool                isSignalingNan;
            bool                isQuietNan;
            bool                isInfinity;
            uint8_t             signBit;   //   0 or 1
            int16_t             exponent;
            uint64_t            mantissa;

            //////////
            //  Operations

            //  Equalizes exponents of 2 FP values
            static void         equalizeExponents(Environment & environment, FpValue & x, FpValue & y);

            //  Adds magnitudes of 2 FP values whose exponents have already been equialized
            static FpValue      addMagnitudes(Environment & environment, const FpValue & x, const FpValue & y);

            //  Subtracts magnitudes of 2 FP values whose exponents have already been equialized
            static FpValue      subtractMagnitudes(const FpValue & x, const FpValue & y);

            //  Parses a 64/32/21-bit IEEE-754 value representation
            static FpValue      parse64(bool & denormalized, uint64_t value);
            static FpValue      parse32(bool & denormalized, uint32_t value);
            static FpValue      parse21(bool & denormalized, uint32_t value);

            //  Builds a 64/32/21-bit IEEE-754 value representation
            uint64_t            build64(Environment & environment) const;
            uint32_t            build32(Environment & environment) const;
            uint32_t            build21(Environment & environment) const;

            //  Adds two FP values and returns the result
            static FpValue      add(Environment & environment, const FpValue & x, const FpValue & y);

            //  Subtracts two FP values and stores the result
            static FpValue      subtract(Environment & environment, const FpValue & x, const FpValue & y);

            //  Multiplies two FP values and stores the result
            static FpValue      multiply(Environment & environment, const FpValue & x, const FpValue & y);

            //  Divides two FP values and stores the result
            static FpValue      divide(Environment & environment, const FpValue & x, const FpValue & y);

            //  Negates an FP value and stores the result
            static FpValue      negate(Environment & environment, const FpValue & x);

            //  Compares 2 FP values, returning negative, zero or positive result if
            //  "x" is, respectively, less-than, equal-to or greater-than "y"
            static int          compare(Environment & environment, const FpValue & x, const FpValue & y);

            //  Converts FP value to [un]signed integer
            static uint64_t     toUnsignedInteger(Environment & environment, const FpValue & x);
            static int64_t      toSignedInteger(Environment & environment, const FpValue & x);

            //  Converts FP value to real
            static long double  toReal(Environment & environment, const FpValue & x);

            //  Classifies a FP value
            static ValueClass   classify(const FpValue & x, bool denormalized);
        };
    }
}

//  End of hadesvm-ieee754/FpValue.hpp
