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

Kis1UnitedKingdomKeyboardLayout::Kis1UnitedKingdomKeyboardLayout()
    :   _kis1ScanCodesForNativeScanCodes()
{
#if defined(Q_OS_LINUX)
    _kis1ScanCodesForNativeScanCodes[9] = 0;    //  Esc
    _kis1ScanCodesForNativeScanCodes[67] = 1;   //  F1
    _kis1ScanCodesForNativeScanCodes[68] = 2;   //  F2
    _kis1ScanCodesForNativeScanCodes[69] = 3;   //  F3
    _kis1ScanCodesForNativeScanCodes[70] = 4;   //  F4
    _kis1ScanCodesForNativeScanCodes[71] = 5;   //  F5
    _kis1ScanCodesForNativeScanCodes[72] = 6;   //  F6
    _kis1ScanCodesForNativeScanCodes[73] = 7;   //  F7
    _kis1ScanCodesForNativeScanCodes[74] = 8;   //  F8
    _kis1ScanCodesForNativeScanCodes[75] = 9;   //  F9
    _kis1ScanCodesForNativeScanCodes[76] = 10;  //  F10
    _kis1ScanCodesForNativeScanCodes[49] = 11;  //  ¬`¦
    _kis1ScanCodesForNativeScanCodes[10] = 12;  //  1!
    _kis1ScanCodesForNativeScanCodes[11] = 13;  //  2"
    _kis1ScanCodesForNativeScanCodes[12] = 14;  //  3<GBP>
    _kis1ScanCodesForNativeScanCodes[13] = 15;  //  4$
    _kis1ScanCodesForNativeScanCodes[14] = 16;  //  5%
    _kis1ScanCodesForNativeScanCodes[15] = 17;  //  6^
    _kis1ScanCodesForNativeScanCodes[16] = 18;  //  7&
    _kis1ScanCodesForNativeScanCodes[17] = 19;  //  8*
    _kis1ScanCodesForNativeScanCodes[18] = 20;  //  9(
    _kis1ScanCodesForNativeScanCodes[19] = 21;  //  0)
    _kis1ScanCodesForNativeScanCodes[20] = 22;  //  -_
    _kis1ScanCodesForNativeScanCodes[21] = 23;  //  =+
    _kis1ScanCodesForNativeScanCodes[22] = 24;  //  BkSpc
    _kis1ScanCodesForNativeScanCodes[23] = 25;  //  Tab
    _kis1ScanCodesForNativeScanCodes[24] = 26;  //  Q
    _kis1ScanCodesForNativeScanCodes[25] = 27;  //  W
    _kis1ScanCodesForNativeScanCodes[26] = 28;  //  E
    _kis1ScanCodesForNativeScanCodes[27] = 29;  //  R
    _kis1ScanCodesForNativeScanCodes[28] = 30;  //  T
    _kis1ScanCodesForNativeScanCodes[29] = 31;  //  Y
    _kis1ScanCodesForNativeScanCodes[30] = 32;  //  U
    _kis1ScanCodesForNativeScanCodes[31] = 33;  //  I
    _kis1ScanCodesForNativeScanCodes[32] = 34;  //  O
    _kis1ScanCodesForNativeScanCodes[33] = 35;  //  P
    _kis1ScanCodesForNativeScanCodes[34] = 36;  //  [{
    _kis1ScanCodesForNativeScanCodes[35] = 37;  //  ]}
    _kis1ScanCodesForNativeScanCodes[36] = 38;  //  Enter
    _kis1ScanCodesForNativeScanCodes[66] = 39;  //  Caps
    _kis1ScanCodesForNativeScanCodes[38] = 40;  //  A
    _kis1ScanCodesForNativeScanCodes[39] = 41;  //  S
    _kis1ScanCodesForNativeScanCodes[40] = 42;  //  D
    _kis1ScanCodesForNativeScanCodes[41] = 43;  //  F
    _kis1ScanCodesForNativeScanCodes[42] = 44;  //  G
    _kis1ScanCodesForNativeScanCodes[43] = 45;  //  H
    _kis1ScanCodesForNativeScanCodes[44] = 46;  //  J
    _kis1ScanCodesForNativeScanCodes[45] = 47;  //  K
    _kis1ScanCodesForNativeScanCodes[46] = 48;  //  L
    _kis1ScanCodesForNativeScanCodes[47] = 49;  //  ;:
    _kis1ScanCodesForNativeScanCodes[48] = 50;  //  '@
    _kis1ScanCodesForNativeScanCodes[51] = 51;  //  #~
    _kis1ScanCodesForNativeScanCodes[50] = 52;  //  LShift
    _kis1ScanCodesForNativeScanCodes[94] = 53;  //  \|
    _kis1ScanCodesForNativeScanCodes[52] = 54;  //  Z
    _kis1ScanCodesForNativeScanCodes[53] = 55;  //  X
    _kis1ScanCodesForNativeScanCodes[54] = 56;  //  C
    _kis1ScanCodesForNativeScanCodes[55] = 57;  //  V
    _kis1ScanCodesForNativeScanCodes[56] = 58;  //  B
    _kis1ScanCodesForNativeScanCodes[57] = 59;  //  N
    _kis1ScanCodesForNativeScanCodes[58] = 60;  //  M
    _kis1ScanCodesForNativeScanCodes[59] = 61;  //  ,<
    _kis1ScanCodesForNativeScanCodes[60] = 62;  //  .>
    _kis1ScanCodesForNativeScanCodes[61] = 63;  //  /?
    _kis1ScanCodesForNativeScanCodes[37] = 64;  //  LCtrl
    _kis1ScanCodesForNativeScanCodes[64] = 65;  //  Alt
    _kis1ScanCodesForNativeScanCodes[65] = 66;  //  Space
    _kis1ScanCodesForNativeScanCodes[108] = 67; //  AltGr
    _kis1ScanCodesForNativeScanCodes[105] = 68; //  RCtrl
    //_kis1ScanCodesForNativeScanCodes[?] = 69; // PrtSc
    _kis1ScanCodesForNativeScanCodes[78] = 70;  //  ScrlLck
    _kis1ScanCodesForNativeScanCodes[127] = 71; //  Break
    _kis1ScanCodesForNativeScanCodes[118] = 72; //  Ins
    _kis1ScanCodesForNativeScanCodes[119] = 73; //  Del
    _kis1ScanCodesForNativeScanCodes[110] = 74; //  Home
    _kis1ScanCodesForNativeScanCodes[115] = 75; //  End
    _kis1ScanCodesForNativeScanCodes[112] = 76; //  PgUp
    _kis1ScanCodesForNativeScanCodes[117] = 77; //  PgDn
    _kis1ScanCodesForNativeScanCodes[113] = 78; //  Left
    _kis1ScanCodesForNativeScanCodes[114] = 79; //  Right
    _kis1ScanCodesForNativeScanCodes[111] = 80; //  Up
    _kis1ScanCodesForNativeScanCodes[116] = 81; //  Down
    _kis1ScanCodesForNativeScanCodes[77] = 82;  //  NumLk
    _kis1ScanCodesForNativeScanCodes[106] = 83; //  Pad /
    _kis1ScanCodesForNativeScanCodes[63] = 84;  //  Pad *
    _kis1ScanCodesForNativeScanCodes[86] = 85;  //  Pad +
    _kis1ScanCodesForNativeScanCodes[82] = 86;  //  Pad -
    _kis1ScanCodesForNativeScanCodes[104] = 87; //  PadEnter
    _kis1ScanCodesForNativeScanCodes[90] = 88;  //  Pad 0
    _kis1ScanCodesForNativeScanCodes[87] = 89;  //  Pad 1
    _kis1ScanCodesForNativeScanCodes[88] = 90;  //  Pad 2
    _kis1ScanCodesForNativeScanCodes[89] = 91;  //  Pad 3
    _kis1ScanCodesForNativeScanCodes[83] = 92;  //  Pad 4
    _kis1ScanCodesForNativeScanCodes[84] = 93;  //  Pad 5
    _kis1ScanCodesForNativeScanCodes[85] = 94;  //  Pad 6
    _kis1ScanCodesForNativeScanCodes[79] = 95;  //  Pad 7
    _kis1ScanCodesForNativeScanCodes[80] = 96;  //  Pad 8
    _kis1ScanCodesForNativeScanCodes[81] = 97;  //  Pad 9
    _kis1ScanCodesForNativeScanCodes[91] = 98;  //  Pad .

    _kis1ScanCodesForNativeScanCodes[62] = 52;  //  RShift == LShift
#elif defined(Q_OS_WINDOWS)
    _kis1ScanCodesForNativeScanCodes[1] = 0;    //  Esc
    _kis1ScanCodesForNativeScanCodes[59] = 1;   //  F1
    _kis1ScanCodesForNativeScanCodes[60] = 2;   //  F2
    _kis1ScanCodesForNativeScanCodes[61] = 3;   //  F3
    _kis1ScanCodesForNativeScanCodes[62] = 4;   //  F4
    _kis1ScanCodesForNativeScanCodes[63] = 5;   //  F5
    _kis1ScanCodesForNativeScanCodes[64] = 6;   //  F6
    _kis1ScanCodesForNativeScanCodes[65] = 7;   //  F7
    _kis1ScanCodesForNativeScanCodes[66] = 8;   //  F8
    _kis1ScanCodesForNativeScanCodes[67] = 9;   //  F9
    _kis1ScanCodesForNativeScanCodes[68] = 10;  //  F10
    _kis1ScanCodesForNativeScanCodes[41] = 11;  //  ¬`¦
    _kis1ScanCodesForNativeScanCodes[2] = 12;   //  1!
    _kis1ScanCodesForNativeScanCodes[3] = 13;   //  2"
    _kis1ScanCodesForNativeScanCodes[4] = 14;   //  3<GBP>
    _kis1ScanCodesForNativeScanCodes[5] = 15;   //  4$
    _kis1ScanCodesForNativeScanCodes[6] = 16;   //  5%
    _kis1ScanCodesForNativeScanCodes[7] = 17;   //  6^
    _kis1ScanCodesForNativeScanCodes[8] = 18;   //  7&
    _kis1ScanCodesForNativeScanCodes[9] = 19;   //  8*
    _kis1ScanCodesForNativeScanCodes[10] = 20;  //  9(
    _kis1ScanCodesForNativeScanCodes[11] = 21;  //  0)
    _kis1ScanCodesForNativeScanCodes[12] = 22;  //  -_
    _kis1ScanCodesForNativeScanCodes[13] = 23;  //  =+
    _kis1ScanCodesForNativeScanCodes[14] = 24;  //  BkSpc
    _kis1ScanCodesForNativeScanCodes[15] = 25;  //  Tab
    _kis1ScanCodesForNativeScanCodes[16] = 26;  //  Q
    _kis1ScanCodesForNativeScanCodes[17] = 27;  //  W
    _kis1ScanCodesForNativeScanCodes[18] = 28;  //  E
    _kis1ScanCodesForNativeScanCodes[19] = 29;  //  R
    _kis1ScanCodesForNativeScanCodes[20] = 30;  //  T
    _kis1ScanCodesForNativeScanCodes[21] = 31;  //  Y
    _kis1ScanCodesForNativeScanCodes[22] = 32;  //  U
    _kis1ScanCodesForNativeScanCodes[23] = 33;  //  I
    _kis1ScanCodesForNativeScanCodes[24] = 34;  //  O
    _kis1ScanCodesForNativeScanCodes[25] = 35;  //  P
    _kis1ScanCodesForNativeScanCodes[26] = 36;  //  [{
    _kis1ScanCodesForNativeScanCodes[27] = 37;  //  ]}
    _kis1ScanCodesForNativeScanCodes[28] = 38;  //  Enter
    _kis1ScanCodesForNativeScanCodes[58] = 39;  //  Caps
    _kis1ScanCodesForNativeScanCodes[30] = 40;  //  A
    _kis1ScanCodesForNativeScanCodes[31] = 41;  //  S
    _kis1ScanCodesForNativeScanCodes[32] = 42;  //  D
    _kis1ScanCodesForNativeScanCodes[33] = 43;  //  F
    _kis1ScanCodesForNativeScanCodes[34] = 44;  //  G
    _kis1ScanCodesForNativeScanCodes[35] = 45;  //  H
    _kis1ScanCodesForNativeScanCodes[36] = 46;  //  J
    _kis1ScanCodesForNativeScanCodes[37] = 47;  //  K
    _kis1ScanCodesForNativeScanCodes[38] = 48;  //  L
    _kis1ScanCodesForNativeScanCodes[39] = 49;  //  ;:
    _kis1ScanCodesForNativeScanCodes[40] = 50;  //  '@
    _kis1ScanCodesForNativeScanCodes[43] = 51;  //  #~
    _kis1ScanCodesForNativeScanCodes[42] = 52;  //  LShift
    _kis1ScanCodesForNativeScanCodes[86] = 53;  //  \|
    _kis1ScanCodesForNativeScanCodes[44] = 54;  //  Z
    _kis1ScanCodesForNativeScanCodes[45] = 55;  //  X
    _kis1ScanCodesForNativeScanCodes[46] = 56;  //  C
    _kis1ScanCodesForNativeScanCodes[47] = 57;  //  V
    _kis1ScanCodesForNativeScanCodes[48] = 58;  //  B
    _kis1ScanCodesForNativeScanCodes[49] = 59;  //  N
    _kis1ScanCodesForNativeScanCodes[50] = 60;  //  M
    _kis1ScanCodesForNativeScanCodes[51] = 61;  //  ,<
    _kis1ScanCodesForNativeScanCodes[52] = 62;  //  .>
    _kis1ScanCodesForNativeScanCodes[53] = 63;  //  /?
    _kis1ScanCodesForNativeScanCodes[29] = 64;  //  LCtrl
    _kis1ScanCodesForNativeScanCodes[56] = 65;  //  Alt
    _kis1ScanCodesForNativeScanCodes[57] = 66;  //  Space
    _kis1ScanCodesForNativeScanCodes[57400] = 67;//  AltGr
    _kis1ScanCodesForNativeScanCodes[57373] = 68; //  RCtrl
    //_kis1ScanCodesForNativeScanCodes[?] = 69; // PrtSc
    _kis1ScanCodesForNativeScanCodes[70] = 70;  //  ScrlLck
    _kis1ScanCodesForNativeScanCodes[69] = 71; //  Break
    _kis1ScanCodesForNativeScanCodes[57426] = 72; //  Ins
    _kis1ScanCodesForNativeScanCodes[57427] = 73; //  Del
    _kis1ScanCodesForNativeScanCodes[57415] = 74; //  Home
    _kis1ScanCodesForNativeScanCodes[57423] = 75; //  End
    _kis1ScanCodesForNativeScanCodes[57417] = 76; //  PgUp
    _kis1ScanCodesForNativeScanCodes[57425] = 77; //  PgDn
    _kis1ScanCodesForNativeScanCodes[57419] = 78; //  Left
    _kis1ScanCodesForNativeScanCodes[57421] = 79; //  Right
    _kis1ScanCodesForNativeScanCodes[57416] = 80; //  Up
    _kis1ScanCodesForNativeScanCodes[57424] = 81; //  Down
    _kis1ScanCodesForNativeScanCodes[57413] = 82;  //  NumLk
    _kis1ScanCodesForNativeScanCodes[57397] = 83; //  Pad /
    _kis1ScanCodesForNativeScanCodes[55] = 84;  //  Pad *
    _kis1ScanCodesForNativeScanCodes[78] = 85;  //  Pad +
    _kis1ScanCodesForNativeScanCodes[74] = 86;  //  Pad -
    _kis1ScanCodesForNativeScanCodes[57372] = 87; //  PadEnter
    _kis1ScanCodesForNativeScanCodes[82] = 88;  //  Pad 0
    _kis1ScanCodesForNativeScanCodes[79] = 89;  //  Pad 1
    _kis1ScanCodesForNativeScanCodes[80] = 90;  //  Pad 2
    _kis1ScanCodesForNativeScanCodes[81] = 91;  //  Pad 3
    _kis1ScanCodesForNativeScanCodes[75] = 92;  //  Pad 4
    _kis1ScanCodesForNativeScanCodes[76] = 93;  //  Pad 5
    _kis1ScanCodesForNativeScanCodes[77] = 94;  //  Pad 6
    _kis1ScanCodesForNativeScanCodes[71] = 95;  //  Pad 7
    _kis1ScanCodesForNativeScanCodes[72] = 96;  //  Pad 8
    _kis1ScanCodesForNativeScanCodes[73] = 97;  //  Pad 9
    _kis1ScanCodesForNativeScanCodes[83] = 98;  //  Pad .

    _kis1ScanCodesForNativeScanCodes[54] = 52;  //  RShift == LShift
#else
    #error Unsupported OS
#endif
}

Kis1UnitedKingdomKeyboardLayout::~Kis1UnitedKingdomKeyboardLayout()
{
}

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

Kis1ScanCode Kis1UnitedKingdomKeyboardLayout::translateKey(uint32_t nativeScanCode, uint32_t /*nativeVirtualKey*/)
{
    if (_kis1ScanCodesForNativeScanCodes.contains(nativeScanCode))
    {
        //qDebug() << displayName()
        //         << " keyboard layout recognized scan code "
        //         << _kis1ScanCodesForNativeScanCodes[nativeScanCode];
        return static_cast<Kis1ScanCode>(_kis1ScanCodesForNativeScanCodes[nativeScanCode]);
    }
    //qDebug() << displayName() << " keyboard layout failed to translate key, "
    //         << "nativeScanCode=" << nativeScanCode
    //         << ", nativeVirtualKey=" << nativeVirtualKey;
    return Kis1ScanCode::Invalid;
}

//  End of hadesvm-cereon/Kis1UnitedKingdomKeyboardLayout.cpp
