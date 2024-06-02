//
//  hadesvm-core/ComponentCategory.cpp
//
//  hadesvm::core::ComponentCategory class implementation
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

//////////
//  Construction/destruction
ComponentCategory::ComponentCategory()
{
}

ComponentCategory::~ComponentCategory()
{
}

//////////
//  Registry
namespace
{
    ComponentCategoryList registry;
}

bool ComponentCategory::register(ComponentCategory * componentCategory)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    Q_ASSERT(componentCategory != nullptr);

    if (ComponentCategory * rcc = findByMnemonic(componentCategory->mnemonic()))
    {   //  Already registered!
        return rcc == componentCategory;
    }
    //  Need to register now
    registry.append(componentCategory);
    return true;
}

ComponentCategory * ComponentCategory::findByMnemonic(const QString & mnemonic)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    for (ComponentCategory * rcc : registry)
    {
        if (rcc->mnemonic() == mnemonic)
        {
            return rcc;
        }
    }
    return nullptr;
}

ComponentCategoryList ComponentCategory::all()
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    return registry;
}

//  End of hadesvm-core/ComponentCategory.cpp
