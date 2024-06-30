//
//  hadesvm-cereon/Kis1UnitedKingdomKeyboardLayout.cpp
//
//  hadesvm::cereon::Kis1UnitedKingdomKeyboardLayout class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Singleton
HADESVM_IMPLEMENT_SINGLETON(Kis1UnitedKingdomKeyboardLayout)
Kis1UnitedKingdomKeyboardLayout::Kis1UnitedKingdomKeyboardLayout() {}
Kis1UnitedKingdomKeyboardLayout::~Kis1UnitedKingdomKeyboardLayout() {}

//////////
//  hadesvm::util::StockObject
QString Kis1UnitedKingdomKeyboardLayout::mnemonic() const
{
    return "UnitedKingdom";
}

QString Kis1UnitedKingdomKeyboardLayout::displayName() const
{
    return "UK";
}

//////////
//  Kis1KeyboardLayout
Kis1KeyboardLayout::Code Kis1UnitedKingdomKeyboardLayout::code() const
{
    return Code::UnitedKingdom;
}

Kis1ScanCode Kis1UnitedKingdomKeyboardLayout::translateKey(uint32_t nativeScanCode, uint32_t nativeVirtualKey)
{
    qDebug() << "Failed to translate key, "
             << "nativeScanCode=" << nativeScanCode
             << ", nativeVirtualKey=" << nativeVirtualKey;
    return Kis1ScanCode::Invalid;
}

//  End of hadesvm-cereon/Kis1UnitedKingdomKeyboardLayout.cpp
