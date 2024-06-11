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
        //  Parses a strinbg "s" representing a value starting from position "scan".
        //  Upon success assigns the parsed value to "value" and returns true,
        //  advansing "scan" past thew parsed portion of "s".
        //  Upon vailure leaves "scan" and "value" intact and returns false.
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, bool & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, char & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, signed char & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, unsigned char & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, signed short & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, unsigned short & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, signed int & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, unsigned int & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, signed long & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, unsigned long & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, signed long long & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, unsigned long long & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, float & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, double & value);

        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, QChar & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, QString& value);

        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, const char * crtFormat, signed char & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, const char * crtFormat, unsigned char & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, const char * crtFormat, signed short & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, const char * crtFormat, unsigned short & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, const char * crtFormat, signed int & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, const char * crtFormat, unsigned int & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, const char * crtFormat, signed long & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, const char * crtFormat, unsigned long & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, const char * crtFormat, signed long long & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, const char * crtFormat, unsigned long long & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, const char * crtFormat, float & value);
        HADESVM_UTIL_PUBLIC bool fromString(const QString & s, qsizetype & scan, const char * crtFormat, double & value);

        //  Shortcuts for parsing
        //  Parse in an assumption that the entire string must be consumed in the process.
        template <class T>
        bool fromEntireString(const QString & s, T & value)
        {
            qsizetype scan = 0;
            return hadesvm::util::fromString(s, scan, value) && scan == s.length();
        }

        template <class T>
        bool fromEntireString(const QString & s, const char * crtFormat, T & value)
        {
            qsizetype scan = 0;
            return hadesvm::util::fromString(s, scan, crtFormat, value) && scan == s.length();
        }
    }
}

//  End of hadesvm-util/FromString.hpp
