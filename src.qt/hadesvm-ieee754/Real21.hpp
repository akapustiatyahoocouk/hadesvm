//
//  hadesvm-ieee754/Real21.hpp
//
//  21-bit real values
//
//////////

namespace hadesvm
{
    namespace ieee754
    {
        //////////
        //  A 21-bit real value (1 bit sign, 6 bits exponent, 14 bits mantissa)
        class HADESVM_IEEE754_PUBLIC Real21 final
        {
            //////////
            //  Construction/destruction/assignment
        private:
            Real21(uint32_t bitPattern, bool dummy);
        public:
            Real21() : bitPattern(0) {}

            static Real21       fromBitPattern(uint32_t bitPattern);
            static Real21       fromInt8(int8_t value, Environment & environment = Environment::Default);
            static Real21       fromUInt8(uint8_t value, Environment & environment = Environment::Default);
            static Real21       fromInt16(int16_t value, Environment & environment = Environment::Default);
            static Real21       fromUInt16(uint16_t value, Environment & environment = Environment::Default);
            static Real21       fromInt32(int32_t value, Environment & environment = Environment::Default);
            static Real21       fromUInt32(uint32_t value, Environment & environment = Environment::Default);
            static Real21       fromInt64(int64_t value, Environment & environment = Environment::Default);
            static Real21       fromUInt64(uint64_t value, Environment & environment = Environment::Default);
            static Real21       fromFloat(float value, Environment & environment = Environment::Default);
            static Real21       fromDouble(double value, Environment & environment = Environment::Default);
            static Real21       fromReal32(const Real32 & value, Environment & environment = Environment::Default);
            static Real21       fromReal64(const Real64 & value, Environment & environment = Environment::Default);

            //////////
            //  Properties
        public:
            uint32_t            bitPattern; //  only uses the lower 21 bits

            //////////
            //  Operations
        public:
            int8_t              toInt8(Environment & environment = Environment::Default) const;
            uint8_t             toUInt8(Environment & environment = Environment::Default) const;
            int16_t             toInt16(Environment & environment = Environment::Default) const;
            uint16_t            toUInt16(Environment & environment = Environment::Default) const;
            int32_t             toInt32(Environment & environment = Environment::Default) const;
            uint32_t            toUInt32(Environment & environment = Environment::Default) const;
            int64_t             toInt64(Environment & environment = Environment::Default) const;
            uint64_t            toUInt64(Environment & environment = Environment::Default) const;
            float               toFloat(Environment & environment = Environment::Default) const;
            double              toDouble(Environment & environment = Environment::Default) const;
            Real32              toReal32(Environment & environment = Environment::Default) const;
            Real64              toReal64(Environment & environment = Environment::Default) const;

            Real21              add(const Real21 & op2, Environment & environment = Environment::Default) const;
            Real21              subtract(const Real21 & op2, Environment & environment = Environment::Default) const;
            Real21              multiply(const Real21 & op2, Environment & environment = Environment::Default) const;
            Real21              divide(const Real21 & op2, Environment & environment = Environment::Default) const;
            Real21              negate(Environment & environment = Environment::Default) const;
            int                 compare(const Real21 & op2, Environment & environment = Environment::Default) const;

            ValueClass          classify() const;

            //////////
            //  Operators
        public:
            Real21              operator + (const Real21 & op2) const { return add(op2); }
            Real21              operator - (const Real21 & op2) const { return subtract(op2); }
            Real21              operator * (const Real21 & op2) const { return multiply(op2); }
            Real21              operator / (const Real21 & op2) const { return divide(op2); }

            Real21 & operator += (const Real21 & op2) { *this = add(op2); return *this; }
            Real21 & operator -= (const Real21 & op2) { *this = subtract(op2); return *this; }
            Real21 & operator *= (const Real21 & op2) { *this = multiply(op2); return *this; }
            Real21 & operator /= (const Real21 & op2) { *this = divide(op2); return *this; }

            bool                operator == (const Real21 & op2) const { return compare(op2) == 0; }
            bool                operator != (const Real21 & op2) const { return compare(op2) != 0; }
            bool                operator <  (const Real21 & op2) const { return compare(op2) < 0; }
            bool                operator <= (const Real21 & op2) const { return compare(op2) <= 0; }
            bool                operator >  (const Real21 & op2) const { return compare(op2) > 0; }
            bool                operator >= (const Real21 & op2) const { return compare(op2) >= 0; }
        };
    }
}

//  End of hadesvm-ieee754/Real21.hpp
