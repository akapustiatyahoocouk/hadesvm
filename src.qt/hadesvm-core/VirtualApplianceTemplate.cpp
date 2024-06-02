//
//  hadesvm-core/VirtualApplianceTemplate.cpp
//
//  hadesvm::core::VirtualApplianceTemplate class implementation
//
//////////
#include "hadesvm-core/API.hpp"
    using namespace hadesvm::core;

//////////
//  Construction/destruction
VirtualApplianceTemplate::VirtualApplianceTemplate()
{
}

VirtualApplianceTemplate::~VirtualApplianceTemplate()
{
}

//////////
//  Registry
namespace
{
    VirtualApplianceTemplateList registry;
}

bool VirtualApplianceTemplate::register(VirtualApplianceTemplate * virtualApplianceTemplate)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    Q_ASSERT(virtualApplianceTemplate != nullptr);

    if (VirtualApplianceTemplate * rvat = findByMnemonic(virtualApplianceTemplate->mnemonic()))
    {   //  Already registered!
        return rvat == virtualApplianceTemplate;
    }
    //  Need to register now
    registry.append(virtualApplianceTemplate);
    return true;
}

VirtualApplianceTemplate * VirtualApplianceTemplate::findByMnemonic(const QString & mnemonic)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    for (VirtualApplianceTemplate * rvat : registry)
    {
        if (rvat->mnemonic() == mnemonic)
        {
            return rvat;
        }
    }
    return nullptr;
}

VirtualApplianceTemplateList VirtualApplianceTemplate::all()
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    return registry;
}

//  End of hadesvm-core/VirtualApplianceTemplate.cpp
