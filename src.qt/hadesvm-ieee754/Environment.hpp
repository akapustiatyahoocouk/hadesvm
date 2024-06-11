//
//  hadesvm-ieee754/Environment.hpp
//
//  A floating-point environment
//
//////////

namespace hadesvm
{
    namespace ieee754
    {
        //////////
        //  A floating-point environment
        enum class RoundingMode
        {
            Up = 0,
            Down = 1,
            TowardsZero = 2,
            TowardsNearest = 3
        };

        enum class Exception
        {
            None = 0x00,
            Operand = 0x01,
            DivisionByZero = 0x02,
            Overflow = 0x04,
            Underflow = 0x08,
            Inexact = 0x10,
            All = 0x1F
        };

        inline Exception operator | (Exception a, Exception b)
        {
            return static_cast<Exception>(static_cast<int>(a) | static_cast<int>(b));
        }

        inline Exception operator & (Exception a, Exception b)
        {
            return static_cast<Exception>(static_cast<int>(a) & static_cast<int>(b));
        }

        inline Exception & operator |= (Exception & a, Exception b)
        {
            a = static_cast<Exception>(static_cast<int>(a) | static_cast<int>(b));
            return a;
        }

        inline Exception & operator &= (Exception & a, Exception b)
        {
            a = static_cast<Exception>(static_cast<int>(a) & static_cast<int>(b));
            return a;
        }

        struct HADESVM_IEEE754_PUBLIC Environment
        {
            //////////
            //  Construction/destruction/assignment
            Environment() : roundingMode(RoundingMode::TowardsZero), exception(Exception::None) {}

            //////////
            //  Properties
            RoundingMode        roundingMode;
            Exception           exception;

            //////////
            //  Operations
            void                reset()
            {
                roundingMode = RoundingMode::TowardsZero;
                exception = Exception::None;
            }

            //////////
            //  The "default" environment
            static Environment  Default;
        };
    }
}

//  End of hadesvm-ieee754/Environment.hpp
