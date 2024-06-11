//
//  hadesvm-ieee754/Real32.hpp
//
//  32-bit real values
//
//////////

namespace hadesvm
{
    namespace ieee754
    {
        //////////
        //  A 32-bit real value
        class HADESVM_IEEE754_PUBLIC Real32 final
        {
            //////////
            //  Construction/destruction/assignment
        private:
            Real32(uint32_t bitPattern, bool dummy);
        public:
            Real32() : bitPattern(0) {}

            static Real32       fromBitPattern(uint32_t bitPattern);
            static Real32       fromInt8(int8_t value, Environment & environment = Environment::Default);
            static Real32       fromUInt8(uint8_t value, Environment & environment = Environment::Default);
            static Real32       fromInt16(int16_t value, Environment & environment = Environment::Default);
            static Real32       fromUInt16(uint16_t value, Environment & environment = Environment::Default);
            static Real32       fromInt32(int32_t value, Environment & environment = Environment::Default);
            static Real32       fromUInt32(uint32_t value, Environment & environment = Environment::Default);
            static Real32       fromInt64(int64_t value, Environment & environment = Environment::Default);
            static Real32       fromUInt64(uint64_t value, Environment & environment = Environment::Default);
            static Real32       fromFloat(float value, Environment & environment = Environment::Default);
            static Real32       fromDouble(double value, Environment & environment = Environment::Default);
            static Real32       fromReal21(const Real21 & value, Environment & environment = Environment::Default);
            static Real32       fromReal64(const Real64 & value, Environment & environment = Environment::Default);

            //////////
            //  Properties
        public:
            uint32_t            bitPattern;

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
            Real21              toReal21(Environment & environment = Environment::Default) const;
            Real64              toReal64(Environment & environment = Environment::Default) const;

            Real32              add(const Real32 & op2, Environment & environment = Environment::Default) const;
            Real32              subtract(const Real32 & op2, Environment & environment = Environment::Default) const;
            Real32              multiply(const Real32 & op2, Environment & environment = Environment::Default) const;
            Real32              divide(const Real32 & op2, Environment & environment = Environment::Default) const;
            Real32              negate(Environment & environment = Environment::Default) const;
            int                 compare(const Real32 & op2, Environment & environment = Environment::Default) const;

            ValueClass          classify() const;

            Real32              sqrt(Environment & environment = Environment::Default) const;

            //////////
            //  Operators
        public:
            Real32              operator + (const Real32 & op2) const { return add(op2); }
            Real32              operator - (const Real32 & op2) const { return subtract(op2); }
            Real32              operator * (const Real32 & op2) const { return multiply(op2); }
            Real32              operator / (const Real32 & op2) const { return divide(op2); }

            Real32 &            operator += (const Real32 & op2) { *this = add(op2); return *this; }
            Real32 &            operator -= (const Real32 & op2) { *this = subtract(op2); return *this; }
            Real32 &            operator *= (const Real32 & op2) { *this = multiply(op2); return *this; }
            Real32 &            operator /= (const Real32 & op2) { *this = divide(op2); return *this; }

            bool                operator == (const Real32 & op2) const { return compare(op2) == 0; }
            bool                operator != (const Real32 & op2) const { return compare(op2) != 0; }
            bool                operator <  (const Real32 & op2) const { return compare(op2) <  0; }
            bool                operator <= (const Real32 & op2) const { return compare(op2) <= 0; }
            bool                operator >  (const Real32 & op2) const { return compare(op2) >  0; }
            bool                operator >= (const Real32 & op2) const { return compare(op2) >= 0; }
        };
    }
}

//  End of hadesvm-ieee754/Real32.hpp
