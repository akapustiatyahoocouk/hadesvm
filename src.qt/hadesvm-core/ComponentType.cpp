//
//  hadesvm-core/ComponentType.cpp
//
//  hadesvm::core::ComponentType class implementation
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

//////////
//  Construction/destruction
ComponentType::ComponentType()
{
}

ComponentType::~ComponentType()
{
}

//////////
//  Operations
bool ComponentType::isAdaptableTo(VirtualArchitecture * architecture) const
{
    Q_ASSERT(architecture != nullptr);

    return ComponentAdaptorType::find(const_cast<ComponentType*>(this), architecture) != nullptr;
}

//////////
//  Registry
namespace
{
    ComponentTypeList registry;
}

bool ComponentType::register(ComponentType * componentType)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    Q_ASSERT(componentType != nullptr);
    Q_ASSERT(componentType->category() != nullptr);

    if (ComponentType * rct = findByMnemonic(componentType->mnemonic()))
    {   //  Already registered!
        return rct == componentType;
    }
    //  Need to register now
    if (!ComponentCategory::register(componentType->category()))
    {   //  OOPS! No point in proceeding
        return false;
    }
    registry.append(componentType);
    return true;
}

ComponentType * ComponentType::findByMnemonic(const QString & mnemonic)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    for (ComponentType * rct : registry)
    {
        if (rct->mnemonic() == mnemonic)
        {
            return rct;
        }
    }
    return nullptr;
}

ComponentTypeList ComponentType::all()
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    return registry;
}

//  End of hadesvm-core/ComponentType.cpp
