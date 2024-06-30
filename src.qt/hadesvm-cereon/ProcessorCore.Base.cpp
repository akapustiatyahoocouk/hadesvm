//
//  hadesvm-cereon/ProcessorCore.Base.cpp
//
//  hadesvm::cereon::ProcessorCore (BASE feature instructions)
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  BASE (data movement)
unsigned ProcessorCore::_handleMovL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000240);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _r[r1] = _r[r2];
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleMovCR(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000200);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _r[r1] = *_cPtr[r2];
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleMovRC(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000201);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    if (!_state.isInKernelMode())
    {
        _raiseProgramInterrupt(ProgramInterrupt::PRIVILEGED);
    }
    switch (r1)
    {
        case 0: //  $state
            {   //  interpret r[r2] as "new state" and validate state changes
                _StateRegister newState(_r[r2]);
                //  Are we changing the byte order ?
                if (_state.getByteOrder() != newState.getByteOrder() &&
                    !_processor->_canChangeByteOrder)
                {   //  OOPS! Can't!
                    throw ProgramInterrupt::OPERAND;
                }
                //  Are we switching to Virtual mode ?
                if (_state.isInRealMode() && newState.isInVirtualMode() &&
                    !_features.has(Feature::ProtectedMemory) && !_features.has(Feature::VirtualMemory))
                {   //  OOPS! Can't!
                    throw ProgramInterrupt::OPERAND;
                }
            }
            //  Switch state and update replicated fields
            _state = _r[r2];
            break;
        case 1: //  $pth
            _pth = _r[r2];
            throw hadesvm::core::VirtualApplianceException("Not yet implemented");
        default:
            //  All other control registers require no special actions when assigned-to
            *_cPtr[r1] = _r[r2];
            break;
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleLiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x00000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    uint64_t imm = Alu::signExtendInt21(instruction);
    //  Execute
    _r[r1] = imm;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSwapH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400024F);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _r[r1] = Alu::signExtendInt16(Alu::reverseBytes16(_r[r2]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSwapUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000257);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _r[r1] = Alu::zeroExtendInt16(Alu::reverseBytes16(_r[r2]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSwapW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400025F);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _r[r1] = Alu::signExtendInt32(Alu::reverseBytes32(_r[r2]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSwapUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000267);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _r[r1] = Alu::zeroExtendInt32(Alu::reverseBytes32(_r[r2]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSwapL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400026F);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _r[r1] = Alu::reverseBytes64(_r[r2]);
    //  Done
    return 1;
}

//////////
//  BASE (arithmetic)
unsigned ProcessorCore::_handleAddB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000280);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeAddSigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAddUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000288);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeAddUnsigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAddH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000290);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeAddSigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAddUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000298);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeAddUnsigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAddW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002A0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeAddSigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAddUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002A8);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeAddUnsigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAddL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002B0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeAddSigned<_LongWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAddUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002B8);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeAddUnsigned<_LongWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000281);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeSubtractSigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000289);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeSubtractUnsigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000291);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeSubtractSigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000299);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeSubtractUnsigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002A1);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeSubtractSigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002A9);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeSubtractUnsigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002B1);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeSubtractSigned<_LongWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002B9);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeSubtractUnsigned<_LongWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleMulB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000282);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeMultiplySigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 10;
}

unsigned ProcessorCore::_handleMulUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400028A);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeMultiplyUnsigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 10;
}

unsigned ProcessorCore::_handleMulH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000292);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeMultiplySigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 10;
}

unsigned ProcessorCore::_handleMulUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400029A);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeMultiplyUnsigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 10;
}

unsigned ProcessorCore::_handleMulW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002A2);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeMultiplySigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 10;
}

unsigned ProcessorCore::_handleMulUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002AA);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeMultiplyUnsigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 10;
}

unsigned ProcessorCore::_handleMulL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002B2);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeMultiplySigned<_LongWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 10;
}

unsigned ProcessorCore::_handleMulUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002BA);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeMultiplyUnsigned<_LongWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 10;
}

unsigned ProcessorCore::_handleDivB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000283);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeDivideSigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleDivUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400028B);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeDivideUnsigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleDivH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000293);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeDivideSigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleDivUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400029B);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeDivideUnsigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleDivW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002A3);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeDivideSigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleDivUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002AB);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeDivideUnsigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleDivL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002B3);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeDivideSigned<_LongWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleDivUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002BB);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeDivideUnsigned<_LongWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleModB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000284);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeModuloSigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleModUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400028C);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeModuloUnsigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleModH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000294);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeModuloSigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleModUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400029C);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeModuloUnsigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleModW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002A4);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeModuloSigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleModUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002AC);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeModuloUnsigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleModL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002B4);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeModuloSigned<_LongWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleModUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002BC);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeModuloUnsigned<_LongWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleAbsB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000285);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeAbsSigned<_Byte>(_r[r1], _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAbsH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000295);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeAbsSigned<_HalfWord>(_r[r1], _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAbsW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x040002A5);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeAbsSigned<_Word>(_r[r1], _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAbsL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x040002B5);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeAbsSigned<_LongWord>(_r[r1], _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleNegB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000286);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeSubtractSigned<_Byte>(_r[r1], 0, _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCpl2UB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400028E);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeCpl2Unsigned<_Byte>(_r[r1], _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleNegH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000296);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeSubtractSigned<_HalfWord>(_r[r1], 0, _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCpl2UH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400029E);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeCpl2Unsigned<_HalfWord>(_r[r1], _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleNegW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x040002A6);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeSubtractSigned<_Word>(_r[r1], 0, _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCpl2UW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x040002AE);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeCpl2Unsigned<_Word>(_r[r1], _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleNegL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x040002B6);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeSubtractSigned<_LongWord>(_r[r1], 0, _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCpl2UL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x040002BE);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeCpl2Unsigned<_LongWord>(_r[r1], _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAddiB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x040002C0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt6(instruction);
    //  Execute
    _executeAddSigned<_Byte>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAddiUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04000AC0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt6(instruction);
    //  Execute
    _executeAddUnsigned<_Byte>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAddiH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x040012C0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt6(instruction);
    //  Execute
    _executeAddSigned<_HalfWord>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAddiUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04001AC0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt6(instruction);
    //  Execute
    _executeAddUnsigned<_HalfWord>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAddiW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x040022C0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt6(instruction);
    //  Execute
    _executeAddSigned<_Word>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAddiUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04002AC0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt6(instruction);
    //  Execute
    _executeAddUnsigned<_Word>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAddiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x08000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _executeAddSigned<_LongWord>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAddiUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x28000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt16(instruction);
    //  Execute
    _executeAddUnsigned<_LongWord>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubiB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x040042C0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt6(instruction);
    //  Execute
    _executeSubtractSigned<_Byte>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubiUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04004AC0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt6(instruction);
    //  Execute
    _executeSubtractUnsigned<_Byte>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubiH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x040052C0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt6(instruction);
    //  Execute
    _executeSubtractSigned<_HalfWord>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubiUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04005AC0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt6(instruction);
    //  Execute
    _executeSubtractUnsigned<_HalfWord>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubiW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x040062C0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt6(instruction);
    //  Execute
    _executeSubtractSigned<_Word>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubiUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04006AC0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt6(instruction);
    //  Execute
    _executeSubtractUnsigned<_Word>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x0C000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _executeSubtractSigned<_LongWord>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubiUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x2C000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt16(instruction);
    //  Execute
    _executeSubtractUnsigned<_LongWord>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleMuliB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x040082C0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt6(instruction);
    //  Execute
    _executeMultiplySigned<_Byte>(_r[r1], _r[r2], imm);
    //  Done
    return 10;
}

unsigned ProcessorCore::_handleMuliUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04008AC0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt6(instruction);
    //  Execute
    _executeMultiplyUnsigned<_Byte>(_r[r1], _r[r2], imm);
    //  Done
    return 10;
}

unsigned ProcessorCore::_handleMuliH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x040092C0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt6(instruction);
    //  Execute
    _executeMultiplySigned<_HalfWord>(_r[r1], _r[r2], imm);
    //  Done
    return 10;
}

unsigned ProcessorCore::_handleMuliUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04009AC0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt6(instruction);
    //  Execute
    _executeMultiplyUnsigned<_HalfWord>(_r[r1], _r[r2], imm);
    //  Done
    return 10;
}

unsigned ProcessorCore::_handleMuliW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400A2C0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt6(instruction);
    //  Execute
    _executeMultiplySigned<_Word>(_r[r1], _r[r2], imm);
    //  Done
    return 10;
}

unsigned ProcessorCore::_handleMuliUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400AAC0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt6(instruction);
    //  Execute
    _executeMultiplyUnsigned<_Word>(_r[r1], _r[r2], imm);
    //  Done
    return 10;
}

unsigned ProcessorCore::_handleMuliL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x10000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _executeMultiplySigned<_LongWord>(_r[r1], _r[r2], imm);
    //  Done
    return 10;
}

unsigned ProcessorCore::_handleMuliUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x30000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt16(instruction);
    //  Execute
    _executeMultiplyUnsigned<_LongWord>(_r[r1], _r[r2], imm);
    //  Done
    return 10;
}

unsigned ProcessorCore::_handleDiviB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400C2C0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt6(instruction);
    //  Execute
    _executeDivideSigned<_Byte>(_r[r1], _r[r2], imm);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleDiviUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400CAC0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt6(instruction);
    //  Execute
    _executeDivideUnsigned<_Byte>(_r[r1], _r[r2], imm);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleDiviH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400D2C0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt6(instruction);
    //  Execute
    _executeDivideSigned<_HalfWord>(_r[r1], _r[r2], imm);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleDiviUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400DAC0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt6(instruction);
    //  Execute
    _executeDivideUnsigned<_HalfWord>(_r[r1], _r[r2], imm);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleDiviW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400E2C0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt6(instruction);
    //  Execute
    _executeDivideSigned<_Word>(_r[r1], _r[r2], imm);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleDiviUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400EAC0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt6(instruction);
    //  Execute
    _executeDivideUnsigned<_Word>(_r[r1], _r[r2], imm);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleDiviL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x14000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _executeDivideSigned<_LongWord>(_r[r1], _r[r2], imm);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleDiviUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x34000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt16(instruction);
    //  Execute
    _executeDivideUnsigned<_LongWord>(_r[r1], _r[r2], imm);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleModiB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x040032C0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt6(instruction);
    //  Execute
    _executeModuloSigned<_Byte>(_r[r1], _r[r2], imm);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleModiUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04003AC0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt6(instruction);
    //  Execute
    _executeModuloUnsigned<_Byte>(_r[r1], _r[r2], imm);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleModiH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x040072C0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt6(instruction);
    //  Execute
    _executeModuloSigned<_HalfWord>(_r[r1], _r[r2], imm);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleModiUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04007AC0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt6(instruction);
    //  Execute
    _executeModuloUnsigned<_HalfWord>(_r[r1], _r[r2], imm);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleModiW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400B2C0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt6(instruction);
    //  Execute
    _executeModuloSigned<_Word>(_r[r1], _r[r2], imm);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleModiUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400BAC0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt6(instruction);
    //  Execute
    _executeModuloUnsigned<_Word>(_r[r1], _r[r2], imm);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleModiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x18000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _executeModuloSigned<_LongWord>(_r[r1], _r[r2], imm);
    //  Done
    return 32;
}

unsigned ProcessorCore::_handleModiUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x38000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt16(instruction);
    //  Execute
    _executeModuloUnsigned<_LongWord>(_r[r1], _r[r2], imm);
    //  Done
    return 32;
}

//////////
//  Base (boolean)
unsigned ProcessorCore::_handleAndB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000248);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeAndSigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAndUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000249);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeAndUnsigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAndH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400024A);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeAndSigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAndUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400024B);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeAndUnsigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAndW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400024C);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeAndSigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAndUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400024D);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeAndUnsigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAndL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400024E);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeAndUnsigned<_LongWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleOrB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000250);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeOrSigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleOrUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000251);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeOrUnsigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleOrH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000252);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeOrSigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleOrUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000253);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeOrUnsigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleOrW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000254);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeOrSigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleOrUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000255);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeOrUnsigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleOrL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000256);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeOrUnsigned<_LongWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleXorB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000258);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeXorSigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleXorUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000259);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeXorUnsigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleXorH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400025A);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeXorSigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleXorUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400025B);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeXorUnsigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleXorW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400025C);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeXorSigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleXorUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400025D);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeXorUnsigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleXorL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400025E);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeXorUnsigned<_LongWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleImplB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000287);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeImplSigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleImplUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400028F);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeImplUnsigned<_Byte>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleImplH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000297);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeImplSigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleImplUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400029F);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeImplUnsigned<_HalfWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleImplW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002A7);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeImplSigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleImplUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002AF);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeImplUnsigned<_Word>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleImplL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x040002B7);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _executeImplUnsigned<_LongWord>(_r[r1], _r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAndiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x60000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt16(instruction);
    //  Execute
    _executeAndUnsigned<_LongWord>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleOriL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x64000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt16(instruction);
    //  Execute
    _executeOrUnsigned<_LongWord>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleXoriL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x68000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt16(instruction);
    //  Execute
    _executeXorUnsigned<_LongWord>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleImpliL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x6C000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt16(instruction);
    //  Execute
    _executeImplUnsigned<_LongWord>(_r[r1], _r[r2], imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleNotB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000260);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeXorSigned<_Byte>(_r[r1], _r[r2], UINT64_C(0xFFFFFFFFFFFFFFFF));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleNotUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000261);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeXorUnsigned<_Byte>(_r[r1], _r[r2], UINT64_C(0xFFFFFFFFFFFFFFFF));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleNotH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000262);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeXorSigned<_HalfWord>(_r[r1], _r[r2], UINT64_C(0xFFFFFFFFFFFFFFFF));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleNotUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000263);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeXorUnsigned<_HalfWord>(_r[r1], _r[r2], UINT64_C(0xFFFFFFFFFFFFFFFF));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleNotW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000264);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeXorSigned<_Word>(_r[r1], _r[r2], UINT64_C(0xFFFFFFFFFFFFFFFF));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleNotUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000265);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeXorUnsigned<_Word>(_r[r1], _r[r2], UINT64_C(0xFFFFFFFFFFFFFFFF));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleNotL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000266);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeXorUnsigned<_LongWord>(_r[r1], _r[r2], UINT64_C(0xFFFFFFFFFFFFFFFF));
    //  Done
    return 1;
}

//////////
//  BASE (shifts)
unsigned ProcessorCore::_handleShlB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000300);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt8(Alu::shl8(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShlUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000318);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt8(Alu::shl8(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShlH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000308);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt16(Alu::shl16(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShlUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000320);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt16(Alu::shl16(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShlW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000310);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt32(Alu::shl32(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShlUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000328);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt32(Alu::shl32(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShlL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000330);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::shl64(_r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShrB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000301);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt8(Alu::shr8(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShrUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000319);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt8(Alu::shr8(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShrH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000309);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt16(Alu::shr16(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShrUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000321);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt16(Alu::shr16(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShrW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000311);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt32(Alu::shr32(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShrUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000329);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt32(Alu::shr32(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShrL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000331);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::shr64(_r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAslB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000302);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt8(Alu::asl8(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAslUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400031A);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt8(Alu::asl8(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAslH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400030A);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt16(Alu::asl16(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAslUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000322);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt16(Alu::asl16(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAslW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000312);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt32(Alu::asl32(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAslUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400032A);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt32(Alu::asl32(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAslL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000332);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::asl64(_r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsrB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000303);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt8(Alu::asr8(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsrUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400031B);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt8(Alu::asr8(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsrH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400030B);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt16(Alu::asr16(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsrUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000323);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt16(Alu::asr16(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsrW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000313);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt32(Alu::asr32(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsrUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400032B);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt32(Alu::asr32(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsrL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000333);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::asr64(_r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRolB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000304);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt8(Alu::rol8(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRolUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400031C);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt8(Alu::rol8(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRolH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400030C);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt16(Alu::rol16(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRolUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000324);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt16(Alu::rol16(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRolW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000314);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt32(Alu::rol32(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRolUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400032C);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt32(Alu::rol32(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRolL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000334);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::rol64(_r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRorB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000305);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt8(Alu::ror8(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRorUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400031D);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt8(Alu::ror8(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRorH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400030D);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt16(Alu::ror16(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRorUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000325);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt16(Alu::ror16(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRorW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000315);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::signExtendInt32(Alu::ror32(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRorUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400032D);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::zeroExtendInt32(Alu::ror32(_r[r2], _r[r3]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRorL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000335);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = Alu::ror64(_r[r2], _r[r3]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShliB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt8(Alu::shl8(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShliUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04000800);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt8(Alu::shl8(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShliH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04001000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt16(Alu::shl16(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShliUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04001800);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt16(Alu::shl16(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShliW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04002000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt32(Alu::shl32(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShliUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04002800);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt32(Alu::shl32(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShliL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04003000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::shl64(_r[r2], shiftCount);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShriB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04004000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt8(Alu::shr8(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShriUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04004800);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt8(Alu::shr8(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShriH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04005000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt16(Alu::shr16(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShriUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04005800);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt16(Alu::shr16(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShriW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04006000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt32(Alu::shr32(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShriUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04006800);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt32(Alu::shr32(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleShriL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04007000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::shr64(_r[r2], shiftCount);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsliB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04008000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt8(Alu::asl8(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsliUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04008800);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt8(Alu::asl8(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsliH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04009000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt16(Alu::asl16(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsliUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04009800);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt16(Alu::asl16(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsliW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400A000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt32(Alu::asl32(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsliUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400A800);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt32(Alu::asl32(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsliL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400B000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::asl64(_r[r2], shiftCount);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsriB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400C000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt8(Alu::asr8(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsriUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400C800);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt8(Alu::asr8(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsriH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400D000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt16(Alu::asr16(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsriUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400D800);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt16(Alu::asr16(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsriW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400E000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt32(Alu::asr32(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsriUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400E800);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt32(Alu::asr32(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAsriL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400F000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::asr64(_r[r2], shiftCount);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRoliB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04000040);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt8(Alu::rol8(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRoliUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04000840);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt8(Alu::rol8(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRoliH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04001040);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt16(Alu::rol16(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRoliUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04001840);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt16(Alu::rol16(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRoliW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04002040);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt32(Alu::rol32(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRoliUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04002840);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt32(Alu::rol32(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRoliL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04003040);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::rol64(_r[r2], shiftCount);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRoriB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04004040);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt8(Alu::ror8(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRoriUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04004840);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt8(Alu::ror8(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRoriH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04005040);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt16(Alu::ror16(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRoriUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04005840);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt16(Alu::ror16(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRoriW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04006040);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::signExtendInt32(Alu::ror32(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRoriUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04006840);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::zeroExtendInt32(Alu::ror32(_r[r2], shiftCount));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRoriL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04007040);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned shiftCount = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::ror64(_r[r2], shiftCount);
    //  Done
    return 1;
}

//////////
//  BASE (bit manipulation)
unsigned ProcessorCore::_handleClz(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000276);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _r[r1] = Alu::countLeadingZeroes(_r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCtz(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000277);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _r[r1] = Alu::countTailingZeroes(_r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleClo(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400027E);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _r[r1] = Alu::countLeadingOnes(_r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCto(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400027F);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _r[r1] = Alu::countTailingOnes(_r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBfeL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007C0) == 0x04000100 ||
            (instruction & 0xFC0007C0) == 0x04000140);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned bitCount = ((instruction >> 11) & 0x1F) | ((instruction >> 1) & 0x20);
    unsigned offset = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::extractSignedBitField(_r[r2], offset, bitCount);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBfeUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007C0) == 0x04000180 ||
            (instruction & 0xFC0007C0) == 0x040001C0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned bitCount = ((instruction >> 11) & 0x1F) | ((instruction >> 1) & 0x20);
    unsigned offset = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::extractUnsignedBitField(_r[r2], offset, bitCount);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBfiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007C0) == 0x04000080 ||
            (instruction & 0xFC0007C0) == 0x040000C0);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned bitCount = ((instruction >> 11) & 0x1F) | ((instruction >> 1) & 0x20);
    unsigned offset = (instruction & 0x3F);
    //  Execute
    _r[r1] = Alu::injectBitField(_r[r1], _r[r2], offset, bitCount);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBrevB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000268);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeBrevSigned<_Byte>(_r[r1], _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBrevUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000269);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeBrevUnsigned<_Byte>(_r[r1], _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBrevH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400026A);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeBrevSigned<_HalfWord>(_r[r1], _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBrevUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400026B);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeBrevUnsigned<_HalfWord>(_r[r1], _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBrevW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400026C);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeBrevSigned<_Word>(_r[r1], _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBrevUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400026D);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeBrevUnsigned<_Word>(_r[r1], _r[r2]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBrevL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400026E);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _executeBrevUnsigned<_LongWord>(_r[r1], _r[r2]);
    //  Done
    return 1;
}

//////////
//  BASE (conversions)
unsigned ProcessorCore::_handleCvtBL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000241);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    bool overflow = false;
    _r[r1] = _Byte::normalizeSigned(_r[r2], overflow);
    _handleIntegerOverflow(overflow);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtUBL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000242);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    bool overflow = false;
    _r[r1] = _Byte::normalizeUnsigned(_r[r2], overflow);
    _handleIntegerOverflow(overflow);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtHL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000243);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    bool overflow = false;
    _r[r1] = _HalfWord::normalizeSigned(_r[r2], overflow);
    _handleIntegerOverflow(overflow);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtUHL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000244);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    bool overflow = false;
    _r[r1] = _HalfWord::normalizeUnsigned(_r[r2], overflow);
    _handleIntegerOverflow(overflow);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtWL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000245);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    bool overflow = false;
    _r[r1] = _Word::normalizeSigned(_r[r2], overflow);
    _handleIntegerOverflow(overflow);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtUWL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000246);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    bool overflow = false;
    _r[r1] = _Word::normalizeUnsigned(_r[r2], overflow);
    _handleIntegerOverflow(overflow);
    //  Done
    return 1;
}

//////////
//  BASE (comparison)
unsigned ProcessorCore::_handleSeqL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000270);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = (Alu::compareSigned(_r[r2], _r[r3]) == 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSneL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000271);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = (Alu::compareSigned(_r[r2], _r[r3]) != 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSltL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000272);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = (Alu::compareSigned(_r[r2], _r[r3]) < 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSleL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000273);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = (Alu::compareSigned(_r[r2], _r[r3]) <= 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSgtL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000274);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = (Alu::compareSigned(_r[r2], _r[r3]) > 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSgeL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000275);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = (Alu::compareSigned(_r[r2], _r[r3]) >= 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSltUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400027A);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = (Alu::compareUnsigned(_r[r2], _r[r3]) < 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSleUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400027B);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = (Alu::compareUnsigned(_r[r2], _r[r3]) <= 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSgtUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400027C);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = (Alu::compareUnsigned(_r[r2], _r[r3]) > 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSgeUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400027D);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    _r[r1] = (Alu::compareUnsigned(_r[r2], _r[r3]) >= 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSeqiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x40000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _r[r1] = (Alu::compareSigned(_r[r2], imm) == 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSneiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x44000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _r[r1] = (Alu::compareSigned(_r[r2], imm) != 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSltiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x48000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _r[r1] = (Alu::compareSigned(_r[r2], imm) < 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSleiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x4C000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _r[r1] = (Alu::compareSigned(_r[r2], imm) <= 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSgtiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x50000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _r[r1] = (Alu::compareSigned(_r[r2], imm) > 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSgeiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x54000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _r[r1] = (Alu::compareSigned(_r[r2], imm) >= 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSltiUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x58000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt16(instruction);
    //  Execute
    _r[r1] = (Alu::compareUnsigned(_r[r2], imm) < 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSleiUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x5C000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt16(instruction);
    //  Execute
    _r[r1] = (Alu::compareUnsigned(_r[r2], imm) <= 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSgtiUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x78000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt16(instruction);
    //  Execute
    _r[r1] = (Alu::compareUnsigned(_r[r2], imm) > 0) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSgeiUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x7C000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt16(instruction);
    //  Execute
    _r[r1] = (Alu::compareUnsigned(_r[r2], imm) >= 0) ? 1u : 0u;
    //  Done
    return 1;
}

//////////
//  BASE (load/store)
unsigned ProcessorCore::_handleLB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x80000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _r[r1] = Alu::signExtendInt8(_loadByte(_r[r2] + imm));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleLUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x84000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _r[r1] = Alu::zeroExtendInt8(_loadByte(_r[r2] + imm));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleLH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x88000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _r[r1] = Alu::signExtendInt16(_loadHalfWord(_r[r2] + imm));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleLUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x8C000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _r[r1] = Alu::zeroExtendInt16(_loadHalfWord(_r[r2] + imm));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleLW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x90000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _r[r1] = Alu::signExtendInt32(_loadWord(_r[r2] + imm));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleLUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x94000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _r[r1] = Alu::zeroExtendInt32(_loadWord(_r[r2] + imm));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleLL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x98000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _r[r1] = _loadLongWord(_r[r2] + imm);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleXchg(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x9C000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute, locking the memory bus
    {
        QMutexLocker lock(_processor->_memoryBus);
        uint64_t value = _loadLongWord(_r[r2] + imm);
        _storeLongWord(_r[r2] + imm, _r[r1]);
        _r[r1] = value;
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xA0000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _storeByte(_r[r2] + imm, static_cast<uint8_t>(_r[r1]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xA4000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _storeHalfWord(_r[r2] + imm, static_cast<uint16_t>(_r[r1]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xA8000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _storeWord(_r[r2] + imm, static_cast<uint32_t>(_r[r1]));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xAC000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _storeLongWord(_r[r2] + imm, _r[r1]);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleLir(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x24000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    uint64_t imm = Alu::signExtendInt21(instruction);
    //  Execute
    _r[r1] = _fetchLongWord(_r[_IpRegister] + (imm << 2));
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleLdm(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x70000000);

    static const unsigned LoadMap[21] =
    {
        1, 2, 3, 4, //  $a0..$a3
        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, //  $s0..$s12
        26,     //  $gp
        28,     //  $fp
        29,     //  $dp
        30      //  $ra
    };

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;

    if (r1 == 31)
    {   //  OOPS! Can't use $ip as a base register
        throw ProgramInterrupt::OPERAND;
    }
    for (int i = 0; i < 21; i++)
    {   //  OOPS! Can't use loaded register as a base register
        if ((instruction & (UINT32_C(0x01) << i)) != 0 && LoadMap[i] == r1)
        {
            throw ProgramInterrupt::OPERAND;
        }
    }

    //  Execute
    unsigned numLoadedRegisters = 0;
    for (int i = 0; i < 21; i++)
    {   //  Load registers with lower numbers at lower addresses
        if ((instruction & (UINT32_C(0x00000001) << i)) != 0)
        {   //  Load this one
            uint64_t address = _r[r1] + (numLoadedRegisters << 3);
            numLoadedRegisters++;
            _r[LoadMap[i]] = _loadLongWord(address);
        }
    }
    _r[r1] += (numLoadedRegisters << 3);

    //  Done
    return 1 + numLoadedRegisters;
}

unsigned ProcessorCore::_handleStm(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x74000000);

    static const unsigned StoreMap[21] =
    {
        1, 2, 3, 4, //  $a0..$a3
        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, //  $s0..$s12
        26,     //  $gp
        28,     //  $fp
        29,     //  $dp
        30      //  $ra
    };

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;

    if (r1 == 31)
    {   //  OOPS! Can't use $ip as a base register
        throw ProgramInterrupt::OPERAND;
    }
    for (int i = 0; i < 21; i++)
    {   //  OOPS! Can't use stored register as a base register
        if ((instruction & (UINT32_C(0x01) << i)) != 0 && StoreMap[i] == r1)
        {
            throw ProgramInterrupt::OPERAND;
        }
    }

    //  Execute
    unsigned numStoredRegisters = 0;
    for (int i = 0; i < 21; i++)
    {   //  Store registers with higher numbers at higher addresses
        if ((instruction & (UINT32_C(0x00100000) >> i)) != 0)
        {   //  Store this one
            numStoredRegisters++;
            uint64_t address = _r[r1] - (numStoredRegisters << 3);
            _storeLongWord(address, _r[StoreMap[20 - i]]);
        }
    }
    _r[r1] -= (numStoredRegisters << 3);

    //  Done
    return 1 + numStoredRegisters;
}

//////////
//  BASE (flow control)
unsigned ProcessorCore::_handleJ(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x1C000000);

    //  Decode
    uint64_t target = Alu::signExtendInt26(instruction);
    //  Execute
    _r[_IpRegister] += (target << 2);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleJal(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x3C000000);

    //  Decode
    uint64_t target = Alu::signExtendInt26(instruction);
    //  Execute
    _r[_RaRegister] = _r[_IpRegister];
    _r[_IpRegister] += (target << 2);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleJr(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC1FFFFF) == 0x04000278);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    //  Execute
    _r[_IpRegister] = _r[r1];
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleJalr(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC1FFFFF) == 0x04000279);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    //  Execute
    _r[_RaRegister] = _r[_IpRegister];
    _r[_IpRegister] = _r[r1];
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBeqL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xC0000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t target = Alu::signExtendInt16(instruction);
    //  Execute
    if (Alu::compareSigned(_r[r1], _r[r2]) == 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBneL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xC4000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t target = Alu::signExtendInt16(instruction);
    //  Execute
    if (Alu::compareSigned(_r[r1], _r[r2]) != 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBltL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xC8000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t target = Alu::signExtendInt16(instruction);
    //  Execute
    if (Alu::compareSigned(_r[r1], _r[r2]) < 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBleL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xCC000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t target = Alu::signExtendInt16(instruction);
    //  Execute
    if (Alu::compareSigned(_r[r1], _r[r2]) <= 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBgtL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xD0000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t target = Alu::signExtendInt16(instruction);
    //  Execute
    if (Alu::compareSigned(_r[r1], _r[r2]) > 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBgeL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xD4000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t target = Alu::signExtendInt16(instruction);
    //  Execute
    if (Alu::compareSigned(_r[r1], _r[r2]) >= 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBltUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xD8000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t target = Alu::signExtendInt16(instruction);
    //  Execute
    if (Alu::compareUnsigned(_r[r1], _r[r2]) < 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBleUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xDC000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t target = Alu::signExtendInt16(instruction);
    //  Execute
    if (Alu::compareUnsigned(_r[r1], _r[r2]) <= 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBgtUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xF8000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t target = Alu::signExtendInt16(instruction);
    //  Execute
    if (Alu::compareUnsigned(_r[r1], _r[r2]) > 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBgeUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xFC000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t target = Alu::signExtendInt16(instruction);
    //  Execute
    if (Alu::compareUnsigned(_r[r1], _r[r2]) >= 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBeqiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04008040);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    uint64_t imm = Alu::signExtendInt5(instruction >> 16);
    uint64_t target = Alu::signExtendInt6(instruction);
    //  Execute
    if (Alu::compareSigned(_r[r1], imm) == 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBneiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04008840);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    uint64_t imm = Alu::signExtendInt5(instruction >> 16);
    uint64_t target = Alu::signExtendInt6(instruction);
    //  Execute
    if (Alu::compareSigned(_r[r1], imm) != 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBltiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04009040);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    uint64_t imm = Alu::signExtendInt5(instruction >> 16);
    uint64_t target = Alu::signExtendInt6(instruction);
    //  Execute
    if (Alu::compareSigned(_r[r1], imm) < 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBleiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x04009840);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    uint64_t imm = Alu::signExtendInt5(instruction >> 16);
    uint64_t target = Alu::signExtendInt6(instruction);
    //  Execute
    if (Alu::compareSigned(_r[r1], imm) <= 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBgtiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400A040);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    uint64_t imm = Alu::signExtendInt5(instruction >> 16);
    uint64_t target = Alu::signExtendInt6(instruction);
    //  Execute
    if (Alu::compareSigned(_r[r1], imm) > 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBgeiL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400A840);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    uint64_t imm = Alu::signExtendInt5(instruction >> 16);
    uint64_t target = Alu::signExtendInt6(instruction);
    //  Execute
    if (Alu::compareSigned(_r[r1], imm) >= 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBltiUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400D040);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt5(instruction >> 16);
    uint64_t target = Alu::signExtendInt6(instruction);
    //  Execute
    if (Alu::compareUnsigned(_r[r1], imm) < 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBleiUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400D840);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt5(instruction >> 16);
    uint64_t target = Alu::signExtendInt6(instruction);
    //  Execute
    if (Alu::compareUnsigned(_r[r1], imm) <= 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBgtiUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400E040);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt5(instruction >> 16);
    uint64_t target = Alu::signExtendInt6(instruction);
    //  Execute
    if (Alu::compareUnsigned(_r[r1], imm) > 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBgeiUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFC0) == 0x0400E840);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    uint64_t imm = Alu::zeroExtendInt5(instruction >> 16);
    uint64_t target = Alu::signExtendInt6(instruction);
    //  Execute
    if (Alu::compareUnsigned(_r[r1], imm) >= 0)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

//////////
//  BASE (I/O)
unsigned ProcessorCore::_handleInB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000228);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    if (!_state.isInKernelMode())
    {
        _raiseProgramInterrupt(ProgramInterrupt::PRIVILEGED);
    }
    try
    {
        uint8_t value = _processor->_ioBus->readByte(static_cast<uint16_t>(_r[r1]));
        _r[r2] = Alu::signExtendInt8(value);
    }
    catch (IoError ioError)
    {
        if (ioError == IoError::NotReady)
        {   //  Must retry on the next CPU cycle
            _r[_IpRegister] -= 4;
            return 1;
        }
        _translateAndThrowIO(ioError);
    }
    //  Done
    Q_ASSERT(_ioBusToProcessorClockRatio > 0);
    return _ioBusToProcessorClockRatio;
}

unsigned ProcessorCore::_handleInUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000230);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    if (!_state.isInKernelMode())
    {
        _raiseProgramInterrupt(ProgramInterrupt::PRIVILEGED);
    }
    try
    {
        uint8_t value = _processor->_ioBus->readByte(static_cast<uint16_t>(_r[r1]));
        _r[r2] = Alu::zeroExtendInt8(value);
    }
    catch (IoError ioError)
    {
        if (ioError == IoError::NotReady)
        {   //  Must retry on the next CPU cycle
            _r[_IpRegister] -= 4;
            return 1;
        }
        _translateAndThrowIO(ioError);
    }
    //  Done
    Q_ASSERT(_ioBusToProcessorClockRatio > 0);
    return _ioBusToProcessorClockRatio;
}

unsigned ProcessorCore::_handleInH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000229);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    if (!_state.isInKernelMode())
    {
        _raiseProgramInterrupt(ProgramInterrupt::PRIVILEGED);
    }
    try
    {
        uint16_t value = _processor->_ioBus->readHalfWord(static_cast<uint16_t>(_r[r1]));
        _r[r2] = Alu::signExtendInt16(value);
    }
    catch (IoError ioError)
    {
        if (ioError == IoError::NotReady)
        {   //  Must retry on the next CPU cycle
            _r[_IpRegister] -= 4;
            return 1;
        }
        _translateAndThrowIO(ioError);
    }
    //  Done
    Q_ASSERT(_ioBusToProcessorClockRatio > 0);
    return _ioBusToProcessorClockRatio;
}

unsigned ProcessorCore::_handleInUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000231);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    if (!_state.isInKernelMode())
    {
        _raiseProgramInterrupt(ProgramInterrupt::PRIVILEGED);
    }
    try
    {
        uint16_t value = _processor->_ioBus->readHalfWord(static_cast<uint16_t>(_r[r1]));
        _r[r2] = Alu::zeroExtendInt16(value);
    }
    catch (IoError ioError)
    {
        if (ioError == IoError::NotReady)
        {   //  Must retry on the next CPU cycle
            _r[_IpRegister] -= 4;
            return 1;
        }
        _translateAndThrowIO(ioError);
    }
    //  Done
    Q_ASSERT(_ioBusToProcessorClockRatio > 0);
    return _ioBusToProcessorClockRatio;
}

unsigned ProcessorCore::_handleInW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400022A);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    if (!_state.isInKernelMode())
    {
        _raiseProgramInterrupt(ProgramInterrupt::PRIVILEGED);
    }
    try
    {
        uint32_t value = _processor->_ioBus->readWord(static_cast<uint16_t>(_r[r1]));
        _r[r2] = Alu::signExtendInt32(value);
    }
    catch (IoError ioError)
    {
        if (ioError == IoError::NotReady)
        {   //  Must retry on the next CPU cycle
            _r[_IpRegister] -= 4;
            return 1;
        }
        _translateAndThrowIO(ioError);
    }
    //  Done
    Q_ASSERT(_ioBusToProcessorClockRatio > 0);
    return _ioBusToProcessorClockRatio;
}

unsigned ProcessorCore::_handleInUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000232);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    if (!_state.isInKernelMode())
    {
        _raiseProgramInterrupt(ProgramInterrupt::PRIVILEGED);
    }
    try
    {
        uint32_t value = _processor->_ioBus->readWord(static_cast<uint16_t>(_r[r1]));
        _r[r2] = Alu::zeroExtendInt32(value);
    }
    catch (IoError ioError)
    {
        if (ioError == IoError::NotReady)
        {   //  Must retry on the next CPU cycle
            _r[_IpRegister] -= 4;
            return 1;
        }
        _translateAndThrowIO(ioError);
    }
    //  Done
    Q_ASSERT(_ioBusToProcessorClockRatio > 0);
    return _ioBusToProcessorClockRatio;
}

unsigned ProcessorCore::_handleInL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400022B);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    if (!_state.isInKernelMode())
    {
        _raiseProgramInterrupt(ProgramInterrupt::PRIVILEGED);
    }
    try
    {
        _r[r2] = _processor->_ioBus->readLongWord(static_cast<uint16_t>(_r[r1]));
    }
    catch (IoError ioError)
    {
        if (ioError == IoError::NotReady)
        {   //  Must retry on the next CPU cycle
            _r[_IpRegister] -= 4;
            return 1;
        }
        _translateAndThrowIO(ioError);
    }
    //  Done
    Q_ASSERT(_ioBusToProcessorClockRatio > 0);
    return _ioBusToProcessorClockRatio;
}

unsigned ProcessorCore::_handleOutB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400022C);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    if (!_state.isInKernelMode())
    {
        _raiseProgramInterrupt(ProgramInterrupt::PRIVILEGED);
    }
    try
    {
        _processor->_ioBus->writeByte(static_cast<uint16_t>(_r[r1]), static_cast<uint8_t>(_r[r2]));
    }
    catch (IoError ioError)
    {
        if (ioError == IoError::NotReady)
        {   //  Must retry on the next CPU cycle
            _r[_IpRegister] -= 4;
            return 1;
        }
        _translateAndThrowIO(ioError);
    }
    //  Done
    Q_ASSERT(_ioBusToProcessorClockRatio > 0);
    return _ioBusToProcessorClockRatio;
}

unsigned ProcessorCore::_handleOutH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400022D);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    if (!_state.isInKernelMode())
    {
        _raiseProgramInterrupt(ProgramInterrupt::PRIVILEGED);
    }
    try
    {
        _processor->_ioBus->writeHalfWord(static_cast<uint16_t>(_r[r1]), static_cast<uint16_t>(_r[r2]));
    }
    catch (IoError ioError)
    {
        if (ioError == IoError::NotReady)
        {   //  Must retry on the next CPU cycle
            _r[_IpRegister] -= 4;
            return 1;
        }
        _translateAndThrowIO(ioError);
    }
    //  Done
    Q_ASSERT(_ioBusToProcessorClockRatio > 0);
    return _ioBusToProcessorClockRatio;
}

unsigned ProcessorCore::_handleOutW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400022E);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    if (!_state.isInKernelMode())
    {
        _raiseProgramInterrupt(ProgramInterrupt::PRIVILEGED);
    }
    try
    {
        _processor->_ioBus->writeWord(static_cast<uint16_t>(_r[r1]), static_cast<uint32_t>(_r[r2]));
    }
    catch (IoError ioError)
    {
        if (ioError == IoError::NotReady)
        {   //  Must retry on the next CPU cycle
            _r[_IpRegister] -= 4;
            return 1;
        }
        _translateAndThrowIO(ioError);
    }
    //  Done
    Q_ASSERT(_ioBusToProcessorClockRatio > 0);
    return _ioBusToProcessorClockRatio;
}

unsigned ProcessorCore::_handleOutL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400022F);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    if (!_state.isInKernelMode())
    {
        _raiseProgramInterrupt(ProgramInterrupt::PRIVILEGED);
    }
    try
    {
        _processor->_ioBus->writeLongWord(static_cast<uint16_t>(_r[r1]), _r[r2]);
    }
    catch (IoError ioError)
    {
        if (ioError == IoError::NotReady)
        {   //  Must retry on the next CPU cycle
            _r[_IpRegister] -= 4;
            return 1;
        }
        _translateAndThrowIO(ioError);
    }
    //  Done
    Q_ASSERT(_ioBusToProcessorClockRatio > 0);
    return _ioBusToProcessorClockRatio;
}

unsigned ProcessorCore::_handleTstp(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000220);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    if (!_state.isInKernelMode())
    {
        _raiseProgramInterrupt(ProgramInterrupt::PRIVILEGED);
    }
    try
    {
        _r[r2] = _processor->_ioBus->testPortStatus(static_cast<uint16_t>(_r[r1]));
    }
    catch (IoError ioError)
    {
        _translateAndThrowIO(ioError);
    }
    //  Done
    Q_ASSERT(_ioBusToProcessorClockRatio > 0);
    return _ioBusToProcessorClockRatio;
}

unsigned ProcessorCore::_handleSetp(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000221);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    if (!_state.isInKernelMode())
    {
        _raiseProgramInterrupt(ProgramInterrupt::PRIVILEGED);
    }
    try
    {
        _processor->_ioBus->setPortStatus(static_cast<uint16_t>(_r[r1]), _r[r2]);
    }
    catch (IoError ioError)
    {
        _translateAndThrowIO(ioError);
    }
    //  Done
    Q_ASSERT(_ioBusToProcessorClockRatio > 0);
    return _ioBusToProcessorClockRatio;
}

//////////
//  BASE (Misc)
unsigned ProcessorCore::_handleHalt(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT(instruction == 0x04000219);

    //  Decode
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    if (!_state.isInKernelMode())
    {
        _raiseProgramInterrupt(ProgramInterrupt::PRIVILEGED);
    }
    _state.setIdleMode();
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleIret(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC1FFFFF) == 0x04000218);

    //  Decode
    unsigned inum = (instruction >> 21) & 0x1F;
    if ((instruction & 0x001FF800) != 0)
    {   //  Bits 11..20 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    if (inum > 5)
    {   //  Bits 11..20 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPERAND);
    }
    //  Execute
    if (!_state.isInKernelMode())
    {
        _raiseProgramInterrupt(ProgramInterrupt::PRIVILEGED);
    }
    //  TODO make sure state change is valid (e.g. byte order change, etc.)
    switch (inum)
    {
        case 0: //  TIMER
            _r[_IpRegister] = _isaveipTm;
            _state = _isavestateTm;
            break;
        case 1: //  IO
            _r[_IpRegister] = _isaveipIo;
            _state = _isavestateIo;
            break;
        case 2: //  SVC
            _r[_IpRegister] = _isaveipSvc;
            _state = _isavestateSvc;
            break;
        case 3: //  PROGRAM
            _r[_IpRegister] = _isaveipPrg;
            _state = _isavestatePrg;
            break;
        case 4: //  EXTERNAL
            _r[_IpRegister] = _isaveipExt;
            _state = _isavestateExt;
            break;
        case 5: //  HARDWARE
            _r[_IpRegister] = _isaveipHw;
            _state = _isavestateHw;
            break;
        default:
            failure();
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleGetfl(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC1FFFFF) == 0x04000338);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    if ((instruction & 0x001FF800) != 0)
    {   //  Bits 11..20 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _r[r1] = _flags;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSetfl(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC1FFFFF) == 0x04000339);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    if ((instruction & 0x001FF800) != 0)
    {   //  Bits 11..20 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _flags = _r[r1];
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleRstfl(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT(instruction == 0x0400033A);

    //  Decode
    if ((instruction & 0x03FFF800) != 0)
    {   //  Bits 11..25 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _flags = 0;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSvc(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT(instruction == 0x0400033D);

    /*
    CARL_OUT << "$ip = "
             << carl::toString(_r[_IpRegister], "%016X")
             << ", $sp = "
             << carl::toString(_r[27], "%016X")
             << "\n";
    */
    //  Decode
    if ((instruction & 0x03FFF800) != 0)
    {   //  Bits 11..25 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    if (_state.isSvcInterruptsEnabled())
    {
        _handleSvcInterrupt();
    }
    else
    {
        _raiseProgramInterrupt(ProgramInterrupt::MASKED);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBrk(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC1FFFFF) == 0x0400033E);

    throw hadesvm::core::VirtualApplianceException("Not yet implemented");
}

unsigned ProcessorCore::_handleCpuid(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC1FFFFF) == 0x0400021A);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    if ((instruction & 0x001FF800) != 0)
    {   //  Bits 11..20 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    _r[r1] =
        (static_cast<uint64_t>(_processor->id()) << 56) |
        (static_cast<uint64_t>(_id) << 48) |
        (_processor->isPrimaryProcessor() ? UINT64_C(0x0000800000000000) : 0) |
        (_isPrimaryCore ? UINT64_C(0x0000400000000000) : 0) |
        UINT64_C(0x0000010000000000) |
        (_features.has(Feature::PerformanceMonitoring) ? UINT64_C(0x0000000000000040) : 0) |
        (_features.has(Feature::VirtualMemory) ? UINT64_C(0x0000000000000020) : 0) |
        (_features.has(Feature::ProtectedMemory) ? UINT64_C(0x0000000000000010) : 0) |
        (_features.has(Feature::UnalignedOperand) ? UINT64_C(0x0000000000000008) : 0) |
        (_features.has(Feature::Debug) ? UINT64_C(0x0000000000000004) : 0) |
        (_features.has(Feature::FloatingPoint) ? UINT64_C(0x0000000000000002) : 0) |
        (_features.has(Feature::Base) ? UINT64_C(0x0000000000000001) : 0);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSigp(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400021B);

    throw hadesvm::core::VirtualApplianceException("Not yet implemented");
}

unsigned ProcessorCore::_handleNop(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT(instruction == 0x04000247);

    //  Decode
    if ((instruction & 0x03FFF800) != 0)
    {   //  Bits 11..25 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Done
    return 1;
}

//  End of hadesvm-cereon/ProcessorCore.Base.cpp
