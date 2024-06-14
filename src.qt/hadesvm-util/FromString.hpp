//
//  hadesvm-util/FromString.hpp
//
//  Formatting
//
//////////

namespace hadesvm
{
    namespace util
    {
        //  Parses a string "s" representing a value starting from position "scan".
        //  Upon success assigns the parsed value to "value" and returns true,
        //  advansing "scan" past thew parsed portion of "s".
        //  Upon vailure leaves "scan" and "value" intact and returns false.
        template <class T>
        bool fromString(const QString & s, qsizetype & scan, T & value) = delete;

        template <class T>
        bool fromString(const QString & s, qsizetype & scan, const char * crtFormat, T & value) = delete;

        //  Shortcuts for parsing
        //  Parse in an assumption that the entire string must be consumed in the process.
        template <class T>
        bool fromString(const QString & s, T & value)
        {
            qsizetype scan = 0;
            return hadesvm::util::fromString(s, scan, value) && scan == s.length();
        }

        template <class T>
        bool fromString(const QString & s, const char * crtFormat, T & value)
        {
            qsizetype scan = 0;
            return hadesvm::util::fromString(s, scan, crtFormat, value) && scan == s.length();
        }

        //  fromString() specializations for C++/Qt types
        template <>
        HADESVM_UTIL_PUBLIC bool fromString<bool>(const QString & s, qsizetype & scan, bool & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<char>(const QString & s, qsizetype & scan, char & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<signed char>(const QString & s, qsizetype & scan, signed char & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<unsigned char>(const QString & s, qsizetype & scan, unsigned char & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<signed short>(const QString & s, qsizetype & scan, signed short & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<unsigned short>(const QString & s, qsizetype & scan, unsigned short & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<signed int>(const QString & s, qsizetype & scan, signed int & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<unsigned int>(const QString & s, qsizetype & scan, unsigned int & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<signed long>(const QString & s, qsizetype & scan, signed long & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<unsigned long>(const QString & s, qsizetype & scan, unsigned long & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<signed long long>(const QString & s, qsizetype & scan, signed long long & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<unsigned long long>(const QString & s, qsizetype & scan, unsigned long long & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<float>(const QString & s, qsizetype & scan, float & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<double>(const QString & s, qsizetype & scan, double & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<QChar>(const QString & s, qsizetype & scan, QChar & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<QString>(const QString & s, qsizetype & scan, QString & value);

        //  fromString() specializations for C++ types (formatted parsing)
        template <>
        HADESVM_UTIL_PUBLIC bool fromString<signed char>(const QString & s, qsizetype & scan, const char * crtFormat, signed char & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<unsigned char>(const QString & s, qsizetype & scan, const char * crtFormat, unsigned char & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<signed short>(const QString & s, qsizetype & scan, const char * crtFormat, signed short & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<unsigned short>(const QString & s, qsizetype & scan, const char * crtFormat, unsigned short & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<signed int>(const QString & s, qsizetype & scan, const char * crtFormat, signed int & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<unsigned int>(const QString & s, qsizetype & scan, const char * crtFormat, unsigned int & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<signed long>(const QString & s, qsizetype & scan, const char * crtFormat, signed long & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<unsigned long>(const QString & s, qsizetype & scan, const char * crtFormat, unsigned long & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<signed long long>(const QString & s, qsizetype & scan, const char * crtFormat, signed long long & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<unsigned long long>(const QString & s, qsizetype & scan, const char * crtFormat, unsigned long long & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<float>(const QString & s, qsizetype & scan, const char * crtFormat, float & value);

        template <>
        HADESVM_UTIL_PUBLIC bool fromString<double>(const QString & s, qsizetype & scan, const char * crtFormat, double & value);
    }
}

//  End of hadesvm-util/FromString.hpp
