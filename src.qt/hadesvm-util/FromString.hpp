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

        //  Shortcuts for parsing
        template <class T>
        bool fromString(const QString & s, T & value)
        {
            qsizetype scan = 0;
            return fromString(s, scan, value) && scan == s.length();
        }
    }
}

//  End of hadesvm-util/FromString.hpp
