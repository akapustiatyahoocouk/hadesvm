//
//  hadesvm-util/FromString.cpp
//
//  The hadesvm::util::fromString() family implementation
//
//////////
#include "hadesvm-util/API.hpp"

namespace
{
    inline int charToDight(QChar c)
    {
        if (c >= '0' && c <= '9')
        {
            return c.unicode() - '0';
        }
        else if (c >= 'a' && c <= 'z')
        {
            return c.unicode() - 'a' + 10;
        }
        else if (c >= 'A' && c <= 'Z')
        {
            return c.unicode() - 'A' + 10;
        }
        else
        {
            return -1;
        }
    }
}

//////////
//  API
//  TODO uncomment & implement all
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, bool & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, char & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, signed char & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, unsigned char & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, signed short & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, unsigned short & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, signed int & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, unsigned int & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, signed long & value);

HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, unsigned long & value)
{
    qsizetype prescan = scan;
    unsigned long temp = 0;
    int numDigits = 0;

    while (prescan < s.length())
    {
        int digit = charToDight(s[prescan]);
        if (digit < 0 || digit > 9)
        {
            break;
        }
        unsigned long next = temp * 10 + digit;
        if (next / 10 != temp)
        {   //  OOPS! Overflow!
            return false;
        }
        temp = next;
        numDigits++;
    }
    if (numDigits > 0)
    {
        value = temp;
        scan = prescan;
        return true;
    }
    return false;   //  no digits!
}

//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, signed long long & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, unsigned long long & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, float & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, double & value);

//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, QChar & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, QString& value);

//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, const char * crtFormat, signed char & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, const char * crtFormat, unsigned char & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, const char * crtFormat, signed short & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, const char * crtFormat, unsigned short & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, const char * crtFormat, signed int & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, const char * crtFormat, unsigned int & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, const char * crtFormat, signed long & value);

HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, const char * crtFormat, unsigned long & value)
{
    static QRegularExpression re("%?[-+0 #]*[0-9]*([diouxX])");

    QRegularExpressionMatch match = re.match(crtFormat);
    if (match.hasMatch())
    {
        QString type = match.captured(1);
        int radix =
            (type == "o") ? 8 :
                (type == "x" || type == "X") ? 16 :
                10;

        qsizetype prescan = scan;
        unsigned long temp = 0;
        int numDigits = 0;

        while (prescan < s.length())
        {
            int digit = charToDight(s[prescan]);
            if (digit < 0 || digit > radix)
            {
                break;
            }
            unsigned long next = temp * 10 + digit;
            if (next / 10 != temp)
            {   //  OOPS! Overflow!
                return false;
            }
            temp = next;
            numDigits++;
        }
        if (numDigits > 0)
        {
            value = temp;
            scan = prescan;
            return true;
        }
        return false;   //  no digits!
    }
    else
    {   //  OOPS! Be defensive in release mode, though
        Q_ASSERT(false);
        return hadesvm::util::fromString(s, scan, value);
    }
}

//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, const char * crtFormat, signed long long & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, const char * crtFormat, unsigned long long & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, const char * crtFormat, float & value);
//HADESVM_UTIL_PUBLIC bool hadesvm::util::fromString(const QString & s, qsizetype & scan, const char * crtFormat, double & value);

//  End of hadesvm-util/FromString.cpp
