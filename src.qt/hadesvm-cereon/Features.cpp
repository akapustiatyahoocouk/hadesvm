//
//  hadesvm-cereon/Features.cpp
//
//  hadesvm::cereon::Features class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Operators
Features & Features::operator += (const Features & op2)
{
    _mask |= op2._mask;
    _normalize();
    return *this;
}

Features & Features::operator -= (const Features & op2)
{
    _mask &= ~op2._mask;
    _normalize();
    return *this;
}

Features Features::operator + (const Features & op2) const
{
    Features result = *this;
    result += op2;
    return result;
}

Features Features::operator - (const Features & op2) const
{
    Features result = *this;
    result -= op2;
    return result;
}

//////////
//  Implementation helpers
void Features::_normalize()
{
    _mask &= 0x7F;
    if ((_mask & static_cast<int>(Feature::ProtectedMemory)) != 0 &&
        (_mask & static_cast<int>(Feature::VirtualMemory)) != 0)
    {
        _mask &= ~static_cast<int>(Feature::ProtectedMemory);
    }
}

//  End of hadesvm-cereon/Features.cpp
