//
//  hadesvm-cereon/Kis1KeyboardLayout.cpp
//
//  hadesvm::cereon::Kis1KeyboardLayout class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Construction/destruction
Kis1KeyboardLayout::Kis1KeyboardLayout()
{
}

Kis1KeyboardLayout::~Kis1KeyboardLayout()
{
}

//////////
//  Operations

//////////
//  Registry
namespace
{
    Kis1KeyboardLayoutList registry;
}

bool Kis1KeyboardLayout::register(Kis1KeyboardLayout * kis1KeyboardLayout)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    Q_ASSERT(kis1KeyboardLayout != nullptr);

    if (Kis1KeyboardLayout * rkkl = findByMnemonic(kis1KeyboardLayout->mnemonic()))
    {   //  Already registered!
        return rkkl == kis1KeyboardLayout;
    }
    //  Need to register now
    registry.append(kis1KeyboardLayout);
    return true;
}

Kis1KeyboardLayout * Kis1KeyboardLayout::findByMnemonic(const QString & mnemonic)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    for (Kis1KeyboardLayout * rkkl : registry)
    {
        if (rkkl->mnemonic() == mnemonic)
        {
            return rkkl;
        }
    }
    return nullptr;
}

Kis1KeyboardLayoutList Kis1KeyboardLayout::all()
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    return registry;
}

Kis1KeyboardLayout * Kis1KeyboardLayout::system()
{
    QLocale locale = qGuiApp->inputMethod()->locale();
    qDebug() << locale;
    //  TODO
    return Kis1UnitedKingdomExtendedKeyboardLayout::instance();
}

//  End of hadesvm-cereon/Kis1KeyboardLayout.cpp
