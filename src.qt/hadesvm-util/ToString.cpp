//
//  hadesvm-util/ToString.cpp
//
//  The hadesvm::util::toString() family implementation
//
//////////
#include "hadesvm-util/API.hpp"

#if defined(Q_CC_GNU)
    #pragma GCC diagnostic ignored "-Wformat-nonliteral"
#elif defined(Q_CC_MSVC)
#else
    #error Unsupported compiler
#endif

//////////
//  API
HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(bool value)
{
    static const QString TrueString = "true";
    static const QString FalseString = "false";

    return value ? TrueString : FalseString;
}

HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(char value)
{
    return QString(QChar(value & 0x00FF));
}

HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(signed char value)
{
    char s[32];
    sprintf(s, "%d", static_cast<int>(value));
    return s;
}

HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(unsigned char value)
{
    char s[32];
    sprintf(s, "%u", static_cast<unsigned int>(value));
    return s;
}

HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(signed short value)
{
    char s[32];
    sprintf(s, "%d", static_cast<int>(value));
    return s;
}

HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(unsigned short value)
{
    char s[32];
    sprintf(s, "%u", static_cast<unsigned int>(value));
    return s;
}

HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(signed int value)
{
    char s[32];
    sprintf(s, "%d", value);
    return s;
}

HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(unsigned int value)
{
    char s[32];
    sprintf(s, "%u", value);
    return s;
}

HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(signed long value)
{
    char s[32];
    sprintf(s, "%ld", value);
    return s;
}

HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(unsigned long value)
{
    char s[32];
    sprintf(s, "%lu", value);
    return s;
}

HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(signed long long value)
{
    char s[32];
    sprintf(s, "%lld", value);
    return s;
}

HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(unsigned long long value)
{
    char s[32];
    sprintf(s, "%llu", value);
    return s;
}

HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(float value)
{
    char s[32];
    sprintf(s, "%g", value);
    return s;
}

HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(double value)
{
    char s[32];
    sprintf(s, "%g", value);
    return s;
}

HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(const QChar & value)
{
    return value;
}

HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(const QString & value)
{
    return value;
}

//  TODO uncomment & implement
//HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(signed char value, const char * crtFormat);

HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(unsigned char value, const char * crtFormat)
{
    static QRegularExpression re("%?[-+0 #]*[0-9]*[diouxX]");

    QRegularExpressionMatch match = re.match(crtFormat);
    if (match.hasMatch())
    {
        return QString::asprintf(crtFormat, value);
    }
    else
    {   //  OOPS! Be defensive in release mode, though
        failure();
        return hadesvm::util::toString(value);
    }
}

//HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(signed short value, const char * crtFormat);
HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(unsigned short value, const char * crtFormat)
{
    static QRegularExpression re("%?[-+0 #]*[0-9]*[diouxX]");

    QRegularExpressionMatch match = re.match(crtFormat);
    if (match.hasMatch())
    {
        return QString::asprintf(crtFormat, value);
    }
    else
    {   //  OOPS! Be defensive in release mode, though
        failure();
        return hadesvm::util::toString(value);
    }
}

//HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(signed int value, const char * crtFormat);

HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(unsigned int value, const char * crtFormat)
{
    static QRegularExpression re("%?[-+0 #]*[0-9]*[diouxX]");

    QRegularExpressionMatch match = re.match(crtFormat);
    if (match.hasMatch())
    {
        return QString::asprintf(crtFormat, value);
    }
    else
    {   //  OOPS! Be defensive in release mode, though
        failure();
        return hadesvm::util::toString(value);
    }
}

//HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(signed long value, const char * crtFormat);
HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(unsigned long value, const char * crtFormat)
{
    static QRegularExpression re("(%?[-+0 #]*[0-9]*)([diouxX])");

    QRegularExpressionMatch match = re.match(crtFormat);
    if (match.hasMatch())
    {
        QString qtFormat = match.captured(1) + "l" + match.captured(2) + '\0';
        return QString::asprintf(qtFormat.toLatin1().data(), value);
    }
    else
    {   //  OOPS! Be defensive in release mode, though
        failure();
        return hadesvm::util::toString(value);
    }
}

//HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(signed long long value, const char * crtFormat);
HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(unsigned long long value, const char * crtFormat)
{
    static QRegularExpression re("(%?[-+0 #]*[0-9]*)([diouxX])");

    QRegularExpressionMatch match = re.match(crtFormat);
    if (match.hasMatch())
    {
        QString qtFormat = match.captured(1) + "ll" + match.captured(2) + '\0';
        return QString::asprintf(qtFormat.toLatin1().data(), value);
    }
    else
    {   //  OOPS! Be defensive in release mode, though
        failure();
        return hadesvm::util::toString(value);
    }
}

//HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(float value, const char * crtFormat);
//HADESVM_UTIL_PUBLIC QString hadesvm::util::toString(double value, const char * crtFormat);

//  End of hadesvm-util/ToString.cpp
