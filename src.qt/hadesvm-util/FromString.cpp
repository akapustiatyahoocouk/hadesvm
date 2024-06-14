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

    inline bool parseUnsigned(const QString & s, qsizetype & scan, int radix, unsigned long long & value)
    {
        qsizetype prescan = scan;
        unsigned long long temp = 0;
        qsizetype numDigits = 0;

        //  Handle sign
        if (prescan < s.length() && s[prescan] == '+')
        {
            prescan++;
        }
        //  Handle digits
        while (prescan < s.length())
        {
            int digit = charToDight(s[prescan]);
            if (digit < 0 || digit > radix)
            {
                break;
            }
            unsigned long long next = temp * radix + digit;
            if (next / radix != temp)
            {   //  OOPS! Overflow!
                return false;
            }
            temp = next;
            numDigits++;
            prescan++;
        }
        if (numDigits > 0)
        {
            value = temp;
            scan = prescan;
            return true;
        }
        return false;   //  no digits!
    }
}

//////////
//  toString() specializations for C++/Qt types
template <>
bool hadesvm::util::fromString<bool>(const QString & s, qsizetype & scan, bool & value)
{   //  We cater to several true/false forms when parsing, because we may
    //   be parsing e.g. a hand-written configuration file
    if (scan + 4 <= s.length() &&
        s[scan + 0].toUpper() == 'T' &&
        s[scan + 1].toUpper() == 'R' &&
        s[scan + 2].toUpper() == 'U' &&
        s[scan + 3].toUpper() == 'E')
    {
        value = true;
        scan += 4;
        return true;
    }
    if (scan + 5 <= s.length() &&
        s[scan + 0].toUpper() == 'F' &&
        s[scan + 1].toUpper() == 'A' &&
        s[scan + 2].toUpper() == 'L' &&
        s[scan + 3].toUpper() == 'S' &&
        s[scan + 4].toUpper() == 'E')
    {
        value = false;
        scan += 5;
        return true;
    }
    if (scan + 3 <= s.length() &&
        s[scan + 0].toUpper() == 'Y' &&
        s[scan + 1].toUpper() == 'E' &&
        s[scan + 2].toUpper() == 'S')
    {
        value = true;
        scan += 3;
        return true;
    }
    if (scan + 2 <= s.length() &&
        s[scan + 0].toUpper() == 'N' &&
        s[scan + 1].toUpper() == 'O')
    {
        value = false;
        scan += 2;
        return true;
    }
    if (scan <= s.length())
    {
        if (s[scan].toUpper() == 'T')
        {
            value = true;
            scan++;
            return true;
        }
        if (s[scan].toUpper() == 'F')
        {
            value = false;
            scan++;
            return true;
        }
        if (s[scan].toUpper() == 'Y')
        {
            value = true;
            scan++;
            return true;
        }
        if (s[scan].toUpper() == 'N')
        {
            value = false;
            scan++;
            return true;
        }
        if (s[scan] == '1')
        {
            value = true;
            scan++;
            return true;
        }
        if (s[scan] == '0')
        {
            value = false;
            scan++;
            return true;
        }
    }
    return false;
}

template <>
bool hadesvm::util::fromString<unsigned long>(const QString & s, qsizetype & scan, unsigned long & value)
{
    qsizetype prescan = scan;
    unsigned long long temp = 0;
    if (parseUnsigned(s, prescan, 10, temp) && temp <= ULONG_MAX)
    {
        scan = prescan;
        value = static_cast<unsigned long>(temp);
        return true;
    }
    return false;
}

template <>
bool hadesvm::util::fromString<unsigned long long>(const QString & s, qsizetype & scan, unsigned long long & value)
{
    return parseUnsigned(s, scan, 10, value);
}

//////////
//  fromString() specializations for C++ types (formatted parsing)
template <>
bool hadesvm::util::fromString<unsigned char>(const QString & s, qsizetype & scan, const char * crtFormat, unsigned char & value)
{
    qsizetype prescan = scan;
    unsigned long long temp = 0;
    if (hadesvm::util::fromString<unsigned long long>(s, prescan, crtFormat, temp) &&
        temp <= UCHAR_MAX)
    {
        scan = prescan;
        value = static_cast<unsigned char>(temp);
        return true;
    }
    return false;
}

template <>
bool hadesvm::util::fromString<unsigned long>(const QString & s, qsizetype & scan, const char * crtFormat, unsigned long & value)
{
    qsizetype prescan = scan;
    unsigned long long temp = 0;
    if (hadesvm::util::fromString<unsigned long long>(s, prescan, crtFormat, temp) &&
        temp <= ULONG_MAX)
    {
        scan = prescan;
        value = static_cast<unsigned long>(temp);
        return true;
    }
    return false;
}

template <>
bool hadesvm::util::fromString<unsigned long long>(const QString & s, qsizetype & scan, const char * crtFormat, unsigned long long & value)
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
        return parseUnsigned(s, scan, radix, value);
    }
    else
    {   //  OOPS! Be defensive in release mode, though
        Q_ASSERT(false);
        return hadesvm::util::fromString(s, scan, value);
    }
}

//  End of hadesvm-util/FromString.cpp
