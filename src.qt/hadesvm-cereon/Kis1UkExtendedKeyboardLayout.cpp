//
//  hadesvm-cereon/Kis1UkExtendedKeyboardLayout.cpp
//
//  hadesvm::cereon::Kis1UkExtendedKeyboardLayout class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Singleton
HADESVM_IMPLEMENT_SINGLETON(Kis1UkExtendedKeyboardLayout)
Kis1UkExtendedKeyboardLayout::Kis1UkExtendedKeyboardLayout() {}
Kis1UkExtendedKeyboardLayout::~Kis1UkExtendedKeyboardLayout() {}

//////////
//  hadesvm::util::StockObject
QString Kis1UkExtendedKeyboardLayout::mnemonic() const
{
    return "UkExtended";
}

QString Kis1UkExtendedKeyboardLayout::displayName() const
{
    return "UK extended";
}

//////////
//  Kis1KeyboardLayout
Kis1KeyboardLayout::Code Kis1UkExtendedKeyboardLayout::code() const
{
    return Code::UnitedKingdomExtended;
}

Kis1ScanCode Kis1UkExtendedKeyboardLayout::translateKey(uint32_t nativeScanCode, uint32_t nativeVirtualKey)
{
    qDebug() << "Failed to translate key, "
             << "nativeScanCode=" << nativeScanCode
             << ", nativeVirtualKey=" << nativeVirtualKey;
    return Kis1ScanCode::Invalid;
}

//  End of hadesvm-cereon/Kis1UkExtendedKeyboardLayout.cpp
