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
//  TODO re-design with templates in a fromString() fashion
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

        HADESVM_UTIL_PUBLIC QString toString(signed char value, const char * crtFormat);
        HADESVM_UTIL_PUBLIC QString toString(unsigned char value, const char * crtFormat);
        HADESVM_UTIL_PUBLIC QString toString(signed short value, const char * crtFormat);
        HADESVM_UTIL_PUBLIC QString toString(unsigned short value, const char * crtFormat);
        HADESVM_UTIL_PUBLIC QString toString(signed int value, const char * crtFormat);
        HADESVM_UTIL_PUBLIC QString toString(unsigned int value, const char * crtFormat);
        HADESVM_UTIL_PUBLIC QString toString(signed long value, const char * crtFormat);
        HADESVM_UTIL_PUBLIC QString toString(unsigned long value, const char * crtFormat);
        HADESVM_UTIL_PUBLIC QString toString(signed long long value, const char * crtFormat);
        HADESVM_UTIL_PUBLIC QString toString(unsigned long long value, const char * crtFormat);
        HADESVM_UTIL_PUBLIC QString toString(float value, const char * crtFormat);
        HADESVM_UTIL_PUBLIC QString toString(double value, const char * crtFormat);
    }
}

//  End of hadesvm-util/ToString.hpp
