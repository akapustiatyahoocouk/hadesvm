//
//  hadesvm-cereon/Kis1UnitedKingdomExtendedKeyboardLayout.cpp
//
//  hadesvm::cereon::Kis1UnitedKingdomExtendedKeyboardLayout class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Singleton
HADESVM_IMPLEMENT_SINGLETON(Kis1UnitedKingdomExtendedKeyboardLayout)
Kis1UnitedKingdomExtendedKeyboardLayout::Kis1UnitedKingdomExtendedKeyboardLayout() {}
Kis1UnitedKingdomExtendedKeyboardLayout::~Kis1UnitedKingdomExtendedKeyboardLayout() {}

//////////
//  hadesvm::util::StockObject
QString Kis1UnitedKingdomExtendedKeyboardLayout::mnemonic() const
{
    return "UnitedKingdomExtended";
}

QString Kis1UnitedKingdomExtendedKeyboardLayout::displayName() const
{
    return "UK extended";
}

//////////
//  Kis1KeyboardLayout
Kis1KeyboardLayout::Code Kis1UnitedKingdomExtendedKeyboardLayout::code() const
{
    return Code::UnitedKingdomExtended;
}

Kis1ScanCode Kis1UnitedKingdomExtendedKeyboardLayout::translateKey(uint32_t nativeScanCode, uint32_t nativeVirtualKey)
{
    qDebug() << "Failed to translate key, "
             << "nativeScanCode=" << nativeScanCode
             << ", nativeVirtualKey=" << nativeVirtualKey;
    return Kis1ScanCode::Invalid;
}

//  End of hadesvm-cereon/Kis1UnitedKingdomExtendedKeyboardLayout.cpp
