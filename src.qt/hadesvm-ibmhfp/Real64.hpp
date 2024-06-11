//
//  hadesvm-ibmhfp/Interface/Real64.hpp - HFP 64-bit value
//
//////////
namespace hadesvm
{
    namespace ibmhfp
    {
        //////////
        //  The 64-bit IBM HFP value.
        struct HADESVM_IBMHFP_PUBLIC Real64
        {
            uint64_t    value;

            //////////
            //  Construction
            Real64() = default;
            explicit Real64(const Real32 & src);
            explicit Real64(int value);

            //////////
            //  Operators
            Real64      operator - () const;

            Real64      operator + (const Real64 & op2) const;
            Real64      operator - (const Real64 & op2) const;
            Real64      operator * (const Real64 & op2) const;
            Real64      operator / (const Real64 & op2) const;

            Real64 &    operator += (const Real64 & op2);
            Real64 &    operator -= (const Real64 & op2);
            Real64 &    operator *= (const Real64 & op2);
            Real64 &    operator /= (const Real64 & op2);

            bool        operator == (const Real64 & op2) const;
            bool        operator != (const Real64 & op2) const;
            bool        operator <  (const Real64 & op2) const;
            bool        operator <= (const Real64 & op2) const;
            bool        operator >  (const Real64 & op2) const;
            bool        operator >= (const Real64 & op2) const;

                        operator double() const;
        };
    }
}

//  End of hadesvm-ibmhfp/Interface/Real64.hpp
