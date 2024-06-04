//
//  hadesvm-core/VirtualApplianceType.cpp
//
//  hadesvm::core::VirtualApplianceType class implementation
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

//////////
//  Construction/destruction
VirtualApplianceType::VirtualApplianceType()
{
}

VirtualApplianceType::~VirtualApplianceType()
{
}

//////////
//  Registry
namespace
{
    VirtualApplianceTypeList registry;
}

bool VirtualApplianceType::register(VirtualApplianceType * virtualApplianceType)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    Q_ASSERT(virtualApplianceType != nullptr);

    if (VirtualApplianceType * rvat = findByMnemonic(virtualApplianceType->mnemonic()))
    {   //  Already registered!
        return rvat == virtualApplianceType;
    }
    //  Need to register now
    registry.append(virtualApplianceType);
    return true;
}

VirtualApplianceType * VirtualApplianceType::findByMnemonic(const QString & mnemonic)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    for (VirtualApplianceType * rvat : registry)
    {
        if (rvat->mnemonic() == mnemonic)
        {
            return rvat;
        }
    }
    return nullptr;
}

VirtualApplianceTypeList VirtualApplianceType::all()
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    return registry;
}

//  End of hadesvm-core/VirtualApplianceType.cpp
