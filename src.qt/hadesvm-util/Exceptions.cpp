//
//  hadesvm-util/Exceptions.cpp
//
//  The vsm::util exceptions
//
//////////
#include "hadesvm-util/API.hpp"
using namespace hadesvm::util;

//////////
//  Throwable
Throwable::Throwable(const QString & message)
    :   _message(message),
        _ansiMessage(message.toUtf8()) {}

Throwable::Throwable(const std::string & message)
    :   _message(message.c_str()),
        _ansiMessage(message) {}

Throwable::Throwable(const char * message)
    :   _message(message),
        _ansiMessage(message) {}

//////////
//  Exception
Exception::Exception(const QString & message)
    :   Throwable(message) {}

Exception::Exception(const std::string & message)
    :   Throwable(message) {}

Exception::Exception(const char * message)
    :   Throwable(message) {}

//////////
//  Error
Error::Error(const QString & message)
    :   Throwable(message) {}

Error::Error(const std::string & message)
    :   Throwable(message) {}

Error::Error(const char * message)
    :   Throwable(message) {}

//////////
//  ProgramError
ProgramError::ProgramError(const QString & message)
    :   Error(message) {}

ProgramError::ProgramError(const std::string & message)
    :   Error(message) {}

ProgramError::ProgramError(const char * message)
    :   Error(message) {}

//////////
//  SystemError
SystemError::SystemError(const QString & message)
    :   Error(message) {}

SystemError::SystemError(const std::string & message)
    :   Error(message) {}

SystemError::SystemError(const char * message)
    :   Error(message) {}

//  End of hadesvm-util/Exceptions.cpp
