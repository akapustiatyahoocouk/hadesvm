//
//  hadesvm-cereon/Alu.hpp
//
//  Cereon ALU
//
//////////

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The ALU
        class HADESVM_CEREON_PUBLIC Alu final
        {
            HADESVM_UTILITY_CLASS(Alu)

            //////////
            //  Constants
        private:
            static const uint64_t SignBit64 = UINT64_C(0x8000000000000000);

            //////////
            //  Operations (data conversion)
        public:
            static uint64_t     signExtendInt5(uint64_t x)
            {
                return ((x & 0x00000010) == 0) ?
                            (x & UINT64_C(0x000000000000001F)) :
                            (x | UINT64_C(0xFFFFFFFFFFFFFFE0));
            }

            static uint64_t     signExtendInt6(uint64_t x)
            {
                return ((x & 0x00000020) == 0) ?
                            (x & UINT64_C(0x000000000000003F)) :
                            (x | UINT64_C(0xFFFFFFFFFFFFFFC0));
            }

            static uint64_t     signExtendInt8(uint64_t x)
            {
                return ((x & 0x00000080) == 0) ?
                            (x & UINT64_C(0x00000000000000FF)) :
                            (x | UINT64_C(0xFFFFFFFFFFFFFF00));
            }

            static uint64_t     signExtendInt16(uint64_t x)
            {
                return ((x & 0x00008000) == 0) ?
                            (x & UINT64_C(0x000000000000FFFF)) :
                            (x | UINT64_C(0xFFFFFFFFFFFF0000));
            }

            static uint64_t     signExtendInt21(uint64_t x)
            {
                return ((x & 0x00100000) == 0) ?
                            (x & UINT64_C(0x00000000001FFFFF)) :
                            (x | UINT64_C(0xFFFFFFFFFFE00000));
            }

            static uint64_t     signExtendInt26(uint64_t x)
            {
                return ((x & 0x02000000) == 0) ?
                            (x & UINT64_C(0x0000000003FFFFFF)) :
                            (x | UINT64_C(0xFFFFFFFFFC000000));
            }

            static uint64_t     signExtendInt32(uint64_t x)
            {
                return ((x & 0x80000000) == 0) ?
                            (x & UINT64_C(0x00000000FFFFFFFF)) :
                            (x | UINT64_C(0xFFFFFFFF00000000));
            }

            static uint64_t     zeroExtendInt5(uint64_t x)
            {
                return x & UINT64_C(0x000000000000001F);
            }

            static uint64_t     zeroExtendInt6(uint64_t x)
            {
                return x & UINT64_C(0x000000000000003F);
            }

            static uint64_t     zeroExtendInt8(uint64_t x)
            {
                return x & UINT64_C(0x00000000000000FF);
            }

            static uint64_t     zeroExtendInt16(uint64_t x)
            {
                return x & UINT64_C(0x000000000000FFFF);
            }

            static uint64_t     zeroExtendInt32(uint64_t x)
            {
                return x & UINT64_C(0x00000000FFFFFFFF);
            }

            //////////
            //  Operations (arithmetic)
        public:
            static uint64_t     addSigned(uint64_t x, uint64_t y, bool & overflow);
            static uint64_t     subtractSigned(uint64_t x, uint64_t y, bool & overflow);
            static uint64_t     multiplySigned(uint64_t x, uint64_t y, bool & overflow);
            static uint64_t     divideSigned(uint64_t x, uint64_t y, bool & overflow, bool & divisionByZero);
            static uint64_t     moduloSigned(uint64_t x, uint64_t y, bool & divisionByZero);
            static uint64_t     absSigned(uint64_t x, bool & overflow);

            static uint64_t     addUnsigned(uint64_t x, uint64_t y, bool & overflow);
            static uint64_t     subtractUnsigned(uint64_t x, uint64_t y, bool & overflow);
            static uint64_t     multiplyUnsigned(uint64_t x, uint64_t y, bool & overflow);
            static uint64_t     divideUnsigned(uint64_t x, uint64_t y, bool & divisionByZero);
            static uint64_t     moduloUnsigned(uint64_t x, uint64_t y, bool & divisionByZero);

            static uint64_t     reverseBytes16(uint64_t x);
            static uint64_t     reverseBytes32(uint64_t x);
            static uint64_t     reverseBytes64(uint64_t x);

            static uint64_t     reverseBits8(uint64_t x);
            static uint64_t     reverseBits16(uint64_t x);
            static uint64_t     reverseBits32(uint64_t x);
            static uint64_t     reverseBits64(uint64_t x);

            static uint64_t     shl8(uint64_t x, uint64_t shiftCount);
            static uint64_t     shl16(uint64_t x, uint64_t shiftCount);
            static uint64_t     shl32(uint64_t x, uint64_t shiftCount);
            static uint64_t     shl64(uint64_t x, uint64_t shiftCount);
            static uint64_t     shr8(uint64_t x, uint64_t shiftCount);
            static uint64_t     shr16(uint64_t x, uint64_t shiftCount);
            static uint64_t     shr32(uint64_t x, uint64_t shiftCount);
            static uint64_t     shr64(uint64_t x, uint64_t shiftCount);

            static uint64_t     asl8(uint64_t x, uint64_t shiftCount);
            static uint64_t     asl16(uint64_t x, uint64_t shiftCount);
            static uint64_t     asl32(uint64_t x, uint64_t shiftCount);
            static uint64_t     asl64(uint64_t x, uint64_t shiftCount);
            static uint64_t     asr8(uint64_t x, uint64_t shiftCount);
            static uint64_t     asr16(uint64_t x, uint64_t shiftCount);
            static uint64_t     asr32(uint64_t x, uint64_t shiftCount);
            static uint64_t     asr64(uint64_t x, uint64_t shiftCount);

            static uint64_t     rol8(uint64_t x, uint64_t shiftCount);
            static uint64_t     rol16(uint64_t x, uint64_t shiftCount);
            static uint64_t     rol32(uint64_t x, uint64_t shiftCount);
            static uint64_t     rol64(uint64_t x, uint64_t shiftCount);
            static uint64_t     ror8(uint64_t x, uint64_t shiftCount);
            static uint64_t     ror16(uint64_t x, uint64_t shiftCount);
            static uint64_t     ror32(uint64_t x, uint64_t shiftCount);
            static uint64_t     ror64(uint64_t x, uint64_t shiftCount);

            static uint64_t     countLeadingZeroes(uint64_t x);
            static uint64_t     countLeadingOnes(uint64_t x);
            static uint64_t     countTailingZeroes(uint64_t x);
            static uint64_t     countTailingOnes(uint64_t x);

            static uint64_t     injectBitField(uint64_t base, uint64_t value, unsigned offset, unsigned bitCount);
            static uint64_t     extractSignedBitField(uint64_t base, unsigned offset, unsigned bitCount);
            static uint64_t     extractUnsignedBitField(uint64_t base, unsigned offset, unsigned bitCount);

            static int          compareSigned(uint64_t x, uint64_t y);
            static int          compareUnsigned(uint64_t x, uint64_t y);
        };
    }
}

//  End of hadesvm-cereon/Alu.hpp

