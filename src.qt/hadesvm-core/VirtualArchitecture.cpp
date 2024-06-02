//
//  hadesvm-core/VirtualArchitecture.cpp
//
//  hadesvm::core::VirtualArchitecture class implementation
//
//////////
#include "hadesvm-core/API.hpp"
    using namespace hadesvm::core;

//////////
//  Construction/destruction
VirtualArchitecture::VirtualArchitecture()
{
}

VirtualArchitecture::~VirtualArchitecture()
{
}

//////////
//  Registry
namespace
{
    VirtualArchitectureList registry;
}

bool VirtualArchitecture::register(VirtualArchitecture * virtualArchitecture)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    Q_ASSERT(virtualArchitecture != nullptr);

    if (VirtualArchitecture * rva = findByMnemonic(virtualArchitecture->mnemonic()))
    {   //  Already registered!
        return rva == virtualArchitecture;
    }
    //  Need to register now
    registry.append(virtualArchitecture);
    return true;
}

VirtualArchitecture * VirtualArchitecture::findByMnemonic(const QString & mnemonic)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    for (VirtualArchitecture * rva : registry)
    {
        if (rva->mnemonic() == mnemonic)
        {
            return rva;
        }
    }
    return nullptr;
}

VirtualArchitectureList VirtualArchitecture::all()
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    return registry;
}

//  End of hadesvm-core/VirtualArchitecture.cpp
