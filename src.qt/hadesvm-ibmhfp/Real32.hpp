//
//  hadesvm-ibmhfp/Interface/Real32.hpp - HFP 32-bit value
//
//////////

namespace hadesvm
{
    namespace ibmhfp
    {
        struct HADESVM_IBMHFP_PUBLIC Real64;

        //////////
        //  The 32-bit IBM HFP value.
        struct HADESVM_IBMHFP_PUBLIC Real32
        {
            uint32_t    value;

            //////////
            //  Construction
            Real32() = default;
            explicit Real32(const Real64 & src);
            explicit Real32(int value);

            //////////
            //  Operators
            Real32      operator - () const;

            Real32      operator + (const Real32 & op2) const;
            Real32      operator - (const Real32 & op2) const;
            Real32      operator * (const Real32 & op2) const;
            Real32      operator / (const Real32 & op2) const;

            Real32 &    operator += (const Real32 & op2);
            Real32 &    operator -= (const Real32 & op2);
            Real32 &    operator *= (const Real32 & op2);
            Real32 &    operator /= (const Real32 & op2);

            bool        operator == (const Real32 & op2) const;
            bool        operator != (const Real32 & op2) const;
            bool        operator <  (const Real32 & op2) const;
            bool        operator <= (const Real32 & op2) const;
            bool        operator >  (const Real32 & op2) const;
            bool        operator >= (const Real32 & op2) const;

                        operator double() const;
        };
    }
}

//  End of hadesvm-ibmhfp/Interface/Real32.hpp
