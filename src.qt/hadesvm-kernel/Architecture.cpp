//
//  hadesvm-kernel/Architecture.cpp
//
//  hadesvm-kernel Architecture
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Singleton
HADESVM_IMPLEMENT_SINGLETON(Architecture)
Architecture::Architecture() {}
Architecture::~Architecture() {}

//////////
//  hadesvm::util::StockObject
QString Architecture::mnemonic() const
{
    return "HadesVM";
}

QString Architecture::displayName() const
{
    return "HADES VM";
}

//  End of hadesvm-kernel/Architecture.cpp
