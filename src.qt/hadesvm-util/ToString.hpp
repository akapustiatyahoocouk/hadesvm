//
//  hadesvm-util/ToString.hpp
//
//  Formatting
//
//////////

namespace hadesvm
{
    namespace util
    {
        //  Converts "value" to a string representation
        HADESVM_UTIL_PUBLIC QString toString(bool value);
        HADESVM_UTIL_PUBLIC QString toString(char value);
        HADESVM_UTIL_PUBLIC QString toString(signed char value);
        HADESVM_UTIL_PUBLIC QString toString(unsigned char value);
        HADESVM_UTIL_PUBLIC QString toString(signed short value);
        HADESVM_UTIL_PUBLIC QString toString(unsigned short value);
        HADESVM_UTIL_PUBLIC QString toString(signed int value);
        HADESVM_UTIL_PUBLIC QString toString(unsigned int value);
        HADESVM_UTIL_PUBLIC QString toString(signed long value);
        HADESVM_UTIL_PUBLIC QString toString(unsigned long value);
        HADESVM_UTIL_PUBLIC QString toString(signed long long value);
        HADESVM_UTIL_PUBLIC QString toString(unsigned long long value);
        HADESVM_UTIL_PUBLIC QString toString(float value);
        HADESVM_UTIL_PUBLIC QString toString(double value);

        HADESVM_UTIL_PUBLIC QString toString(const QChar & value);
        HADESVM_UTIL_PUBLIC QString toString(const QString & value);
    }
}

//  End of hadesvm-util/ToString.hpp
