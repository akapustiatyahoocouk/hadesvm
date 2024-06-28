//
//  hadesvm-cereon/ProcessorCore.FloatingPoint.cpp
//
//  hadesvm::cereon::ProcessorCore (FLOATING POINT feature instructions)
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  FLOATING POINT (data movement)
unsigned ProcessorCore::_handleMovD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000400);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
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

unsigned ProcessorCore::_handleLiD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0x20000000);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    hadesvm::ieee754::Real21 imm = hadesvm::ieee754::Real21::fromBitPattern(instruction & 0x001FFFFF);
    //  Execute
    _r[r1] = imm.toReal64().bitPattern;
    //  Done
    return 1;
}

//////////
//  FLOATING POINT (arithmetics)
unsigned ProcessorCore::_handleAddF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000410);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real32 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]).toReal32(fpEnvironment);
    hadesvm::ieee754::Real32 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r3]).toReal32(fpEnvironment);
    _r[r1] = x.add(y, fpEnvironment).toReal64().bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAddD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000408);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    hadesvm::ieee754::Real64 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r3]);
    _r[r1] = x.add(y, fpEnvironment).bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000411);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real32 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]).toReal32(fpEnvironment);
    hadesvm::ieee754::Real32 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r3]).toReal32(fpEnvironment);
    _r[r1] = x.subtract(y, fpEnvironment).toReal64().bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSubD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000409);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    hadesvm::ieee754::Real64 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r3]);
    _r[r1] = x.subtract(y, fpEnvironment).bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleMulF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000412);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real32 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]).toReal32(fpEnvironment);
    hadesvm::ieee754::Real32 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r3]).toReal32(fpEnvironment);
    _r[r1] = x.multiply(y, fpEnvironment).toReal64().bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleMulD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400040A);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    hadesvm::ieee754::Real64 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r3]);
    _r[r1] = x.multiply(y, fpEnvironment).bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleDivF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x04000413);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real32 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]).toReal32(fpEnvironment);
    hadesvm::ieee754::Real32 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r3]).toReal32(fpEnvironment);
    _r[r1] = x.divide(y, fpEnvironment).toReal64().bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleDivD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC0007FF) == 0x0400040B);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    hadesvm::ieee754::Real64 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r3]);
    _r[r1] = x.divide(y, fpEnvironment).bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAbsF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000414);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real32 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]).toReal32(fpEnvironment);
    _r[r1] = (x.compare(hadesvm::ieee754::Real32::fromInt8(0)) >= 0) ?
                x.toReal64().bitPattern :
                x.negate(fpEnvironment).toReal64().bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleAbsD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400040C);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    _r[r1] = (x.compare(hadesvm::ieee754::Real64::fromInt8(0)) >= 0) ?
        x.bitPattern :
        x.negate(fpEnvironment).bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleNegF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000415);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real32 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]).toReal32(fpEnvironment);
    _r[r1] = x.negate(fpEnvironment).toReal64().bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleNegD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400040D);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    _r[r1] = x.negate(fpEnvironment).bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSqrtF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000416);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real32 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]).toReal32(fpEnvironment);
    hadesvm::ieee754::Real32 result;
    switch (x.classify())
    {
        case hadesvm::ieee754::ValueClass::Zero:
            //  sqrt(0.0) = 0.0
            result = x;
            break;
        case hadesvm::ieee754::ValueClass::PositiveDenormalized:
        case hadesvm::ieee754::ValueClass::NegativeDenormalized:
        case hadesvm::ieee754::ValueClass::PositiveNormalized:
        case hadesvm::ieee754::ValueClass::NegativeNormalized:
            //  Calculate, making sure operand is > 0
            if (x.compare(hadesvm::ieee754::Real32::fromInt8(0)) < 0)
            {   //  OOPS! Operand is negative!
                _r[r1] = 0;
                _flags.setRealOperandFlag();
                _raiseProgramInterrupt(ProgramInterrupt::FOPERAND);
            }
            result = x.sqrt(fpEnvironment);
            break;
        case hadesvm::ieee754::ValueClass::PositiveInfinity:
        case hadesvm::ieee754::ValueClass::NegativeInfinity:
            //  sqrt(+/-inf) = +/-inf
            result = x;
            break;
        case hadesvm::ieee754::ValueClass::SignallingNan:
            //  sqrt(NaN) = NaN, but SNaN may cause an exception
            _flags.setRealOperandFlag();
            if (_state.isRealOperandExceptionAllowed())
            {
                _r[r1] = x.toReal64().bitPattern;
                _raiseProgramInterrupt(ProgramInterrupt::FOPERAND);
            }
            //  Fall-through
        case hadesvm::ieee754::ValueClass::QuietNan:
            result = x;
            break;
        default:
            failure();
    }
    _r[r1] = result.toReal64().bitPattern;
    fpEnvironment.exception &= hadesvm::ieee754::Exception::Inexact;    //  ...we don't care about other ones
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSqrtD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400040E);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    hadesvm::ieee754::Real64 result;
    switch (x.classify())
    {
        case hadesvm::ieee754::ValueClass::Zero:
            //  sqrt(0.0) = 0.0
            result = x;
            break;
        case hadesvm::ieee754::ValueClass::PositiveDenormalized:
        case hadesvm::ieee754::ValueClass::NegativeDenormalized:
        case hadesvm::ieee754::ValueClass::PositiveNormalized:
        case hadesvm::ieee754::ValueClass::NegativeNormalized:
            //  Calculate, making sure operand is > 0
            if (x.compare(hadesvm::ieee754::Real64::fromInt8(0)) < 0)
            {   //  OOPS! Operand is negative!
                _r[r1] = 0;
                _flags.setRealOperandFlag();
                _raiseProgramInterrupt(ProgramInterrupt::FOPERAND);
            }
            result = x.sqrt(fpEnvironment);
            break;
        case hadesvm::ieee754::ValueClass::PositiveInfinity:
        case hadesvm::ieee754::ValueClass::NegativeInfinity:
            //  sqrt(+/-inf) = +/-inf
            result = x;
            break;
        case hadesvm::ieee754::ValueClass::SignallingNan:
            //  sqrt(NaN) = NaN, but SNaN may cause an exception
            _flags.setRealOperandFlag();
            if (_state.isRealOperandExceptionAllowed())
            {
                _r[r1] = x.bitPattern;
                _raiseProgramInterrupt(ProgramInterrupt::FOPERAND);
            }
            //  Fall-through
        case hadesvm::ieee754::ValueClass::QuietNan:
            result = x;
            break;
        default:
            failure();
    }
    _r[r1] = result.bitPattern;
    fpEnvironment.exception &= hadesvm::ieee754::Exception::Inexact;    //  ...we don't care about other ones
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

//////////
//  FLOATING POINT (conversions)
unsigned ProcessorCore::_handleCvtFB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000420);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real32 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]).toReal32(fpEnvironment);
    _r[r1] = Alu::signExtendInt8(static_cast<uint8_t>(x.toInt8(fpEnvironment)));
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtFUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000424);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real32 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]).toReal32(fpEnvironment);
    _r[r1] = Alu::zeroExtendInt8(x.toUInt8(fpEnvironment));
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtFH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000421);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real32 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]).toReal32(fpEnvironment);
    _r[r1] = Alu::signExtendInt16(static_cast<uint16_t>(x.toInt16(fpEnvironment)));
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtFUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000425);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real32 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]).toReal32(fpEnvironment);
    _r[r1] = Alu::zeroExtendInt16(x.toUInt16(fpEnvironment));
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtFW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000422);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real32 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]).toReal32(fpEnvironment);
    _r[r1] = Alu::signExtendInt32(static_cast<uint32_t>(x.toInt32(fpEnvironment)));
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtFUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000426);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real32 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]).toReal32(fpEnvironment);
    _r[r1] = Alu::zeroExtendInt32(x.toUInt32(fpEnvironment));
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtFL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000423);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real32 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]).toReal32(fpEnvironment);
    _r[r1] = static_cast<uint64_t>(x.toInt64(fpEnvironment));
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtFUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000427);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real32 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]).toReal32(fpEnvironment);
    _r[r1] = x.toUInt64(fpEnvironment);
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtDB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000430);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    _r[r1] = Alu::signExtendInt8(static_cast<uint8_t>(x.toInt8(fpEnvironment)));
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtDUB(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000434);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    _r[r1] = Alu::zeroExtendInt8(x.toUInt8(fpEnvironment));
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtDH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000431);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    _r[r1] = Alu::signExtendInt16(static_cast<uint16_t>(x.toInt16(fpEnvironment)));
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtDUH(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000435);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    _r[r1] = Alu::zeroExtendInt16(x.toUInt16(fpEnvironment));
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtDW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000432);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    _r[r1] = Alu::signExtendInt32(static_cast<uint32_t>(x.toInt32(fpEnvironment)));
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtDUW(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000436);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    _r[r1] = Alu::zeroExtendInt32(x.toUInt32(fpEnvironment));
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtDL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000433);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    _r[r1] = static_cast<uint64_t>(x.toInt64(fpEnvironment));
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtDUL(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000437);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    _r[r1] = x.toUInt64(fpEnvironment);
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtBF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000428);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    _r[r1] = hadesvm::ieee754::Real32::fromInt8(static_cast<int8_t>(_r[r2])).toReal64().bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtUBF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400042C);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    _r[r1] = hadesvm::ieee754::Real32::fromUInt8(static_cast<uint8_t>(_r[r2])).toReal64().bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtHF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000429);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    _r[r1] = hadesvm::ieee754::Real32::fromInt16(static_cast<int16_t>(_r[r2])).toReal64().bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtUHF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400042D);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    _r[r1] = hadesvm::ieee754::Real32::fromUInt16(static_cast<uint16_t>(_r[r2])).toReal64().bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtWF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400042A);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    _r[r1] = hadesvm::ieee754::Real32::fromInt32(static_cast<int32_t>(_r[r2])).toReal64().bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtUWF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400042E);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    _r[r1] = hadesvm::ieee754::Real32::fromUInt32(static_cast<uint32_t>(_r[r2])).toReal64().bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtLF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400042B);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    _r[r1] = hadesvm::ieee754::Real32::fromInt64(static_cast<int64_t>(_r[r2])).toReal64().bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtULF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400042F);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    _r[r1] = hadesvm::ieee754::Real32::fromUInt64(_r[r2]).toReal64().bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtBD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000438);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    _r[r1] = hadesvm::ieee754::Real64::fromInt8(static_cast<int8_t>(_r[r2])).bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtUBD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400043C);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    _r[r1] = hadesvm::ieee754::Real64::fromUInt8(static_cast<uint8_t>(_r[r2])).bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtHD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000439);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    _r[r1] = hadesvm::ieee754::Real64::fromInt16(static_cast<int16_t>(_r[r2])).bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtUHD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400043D);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    _r[r1] = hadesvm::ieee754::Real64::fromUInt16(static_cast<uint16_t>(_r[r2])).bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtWD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400043A);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    _r[r1] = hadesvm::ieee754::Real64::fromInt32(static_cast<int32_t>(_r[r2])).bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtUWD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400043E);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    _r[r1] = hadesvm::ieee754::Real64::fromUInt32(static_cast<uint32_t>(_r[r2])).bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtLD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400043B);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    _r[r1] = hadesvm::ieee754::Real64::fromInt64(static_cast<int64_t>(_r[r2])).bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtULD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400043F);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    _r[r1] = hadesvm::ieee754::Real64::fromUInt64(_r[r2]).bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleCvtDF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000401);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    if ((instruction & 0x0000F800) != 0)
    {   //  Bits 11..15 are unused and must all be 0
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Execute
    hadesvm::ieee754::Environment fpEnvironment;
    _r[r1] = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]).toReal32(fpEnvironment).toReal64(fpEnvironment).bitPattern;
    _handleFloatingPointConditions(fpEnvironment);
    //  Done
    return 1;
}

//////////
//  FLOATING POINT (comparison)
unsigned ProcessorCore::_handleSeqD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000418);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    hadesvm::ieee754::Real64 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r3]);
    _r[r1] = (x == y) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSneD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x04000419);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    hadesvm::ieee754::Real64 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r3]);
    _r[r1] = (x != y) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSltD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400041A);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    hadesvm::ieee754::Real64 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r3]);
    _r[r1] = (x < y) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSleD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400041B);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    hadesvm::ieee754::Real64 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r3]);
    _r[r1] = (x <= y) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSgtD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400041C);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    hadesvm::ieee754::Real64 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r3]);
    _r[r1] = (x > y) ? 1u : 0u;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSgeD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC00FFFF) == 0x0400041D);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    unsigned r3 = (instruction >> 11) & 0x1F;
    //  Execute
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    hadesvm::ieee754::Real64 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r3]);
    _r[r1] = (x >= y) ? 1u : 0u;
    //  Done
    return 1;
}

//////////
//  FLOATING POINT (load/store)
unsigned ProcessorCore::_handleLF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xB0000000);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _r[r1] = hadesvm::ieee754::Real32::fromBitPattern(_loadWord(_r[r2] + imm)).toReal64().bitPattern;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleLD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xB4000000);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _r[r1] = hadesvm::ieee754::Real64::fromBitPattern(_loadLongWord(_r[r2] + imm)).bitPattern;
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSF(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xB8000000);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _storeWord(_r[r2] + imm, hadesvm::ieee754::Real64::fromBitPattern(_r[r1]).toReal32().bitPattern);
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleSD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xBC000000);

    //  Check availability
    if (!_features.has(Feature::FloatingPoint))
    {   //  OOPS! FLOATING POINT feature not available
        _raiseProgramInterrupt(ProgramInterrupt::OPCODE);
    }
    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t imm = Alu::signExtendInt16(instruction);
    //  Execute
    _storeLongWord(_r[r2] + imm, hadesvm::ieee754::Real64::fromBitPattern(_r[r1]).bitPattern);
    //  Done
    return 1;
}

//////////
//  FLOATING POINT (flow control)
unsigned ProcessorCore::_handleBeqD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xE0000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t target = Alu::signExtendInt16(instruction);
    //  Execute
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r1]);
    hadesvm::ieee754::Real64 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    if (x == y)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBneD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xE4000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t target = Alu::signExtendInt16(instruction);
    //  Execute
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r1]);
    hadesvm::ieee754::Real64 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    if (x != y)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBltD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xE8000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t target = Alu::signExtendInt16(instruction);
    //  Execute
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r1]);
    hadesvm::ieee754::Real64 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    if (x < y)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBleD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xEC000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t target = Alu::signExtendInt16(instruction);
    //  Execute
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r1]);
    hadesvm::ieee754::Real64 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    if (x <= y)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBgtD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xF0000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t target = Alu::signExtendInt16(instruction);
    //  Execute
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r1]);
    hadesvm::ieee754::Real64 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    if (x > y)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

unsigned ProcessorCore::_handleBgeD(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    Q_ASSERT((instruction & 0xFC000000) == 0xF4000000);

    //  Decode
    unsigned r1 = (instruction >> 21) & 0x1F;
    unsigned r2 = (instruction >> 16) & 0x1F;
    uint64_t target = Alu::signExtendInt16(instruction);
    //  Execute
    hadesvm::ieee754::Real64 x = hadesvm::ieee754::Real64::fromBitPattern(_r[r1]);
    hadesvm::ieee754::Real64 y = hadesvm::ieee754::Real64::fromBitPattern(_r[r2]);
    if (x >= y)
    {
        _r[_IpRegister] += (target << 2);
    }
    //  Done
    return 1;
}

//  End of hadesvm-cereon/ProcessorCore.FloatingPoint.cpp
