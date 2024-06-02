//
//  hadesvm-core/ComponentAdaptorType.cpp
//
//  hadesvm::core::ComponentAdaptorType class implementation
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

//////////
//  Construction/destruction
ComponentAdaptorType::ComponentAdaptorType()
{
}

ComponentAdaptorType::~ComponentAdaptorType()
{
}

//////////
//  hadesvm::util::StockObject
QString ComponentAdaptorType::mnemonic() const
{
    return adaptedComponentType()->mnemonic() + "/" + adaptedToArchitecture()->mnemonic();
}

QString ComponentAdaptorType::displayName() const
{
    return adaptedComponentType()->displayName() + " / " + adaptedToArchitecture()->displayName();
}

//////////
//  Registry
namespace
{
    ComponentAdaptorTypeList registry;
}

bool ComponentAdaptorType::register(ComponentAdaptorType * componentAdaptorType)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    Q_ASSERT(componentAdaptorType != nullptr);

    if (ComponentAdaptorType * rcat = findByMnemonic(componentAdaptorType->mnemonic()))
    {   //  Already registered!
        return rcat == componentAdaptorType;
    }
    //  Need to register now
    registry.append(componentAdaptorType);
    return true;
}

ComponentAdaptorType * ComponentAdaptorType::findByMnemonic(const QString & mnemonic)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    for (ComponentAdaptorType * rcat : registry)
    {
        if (rcat->mnemonic() == mnemonic)
        {
            return rcat;
        }
    }
    return nullptr;
}

ComponentAdaptorType * ComponentAdaptorType::find(ComponentType * componentType, VirtualArchitecture * architecture)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    Q_ASSERT(componentType != nullptr);
    Q_ASSERT(architecture != nullptr);

    for (ComponentAdaptorType * rcat : registry)
    {
        if (rcat->adaptedComponentType() == componentType &&
            rcat->adaptedToArchitecture() == architecture)
        {
            return rcat;
        }
    }
    return nullptr;
}

ComponentAdaptorTypeList ComponentAdaptorType::all()
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    return registry;
}

//  End of hadesvm-core/ComponentAdaptorType.cpp
