//
//  hadesvm-cereon/ProcessorCore.cpp
//
//  hadesvm::cereon::ProcessorCore class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Construction/destruction
ProcessorCore::ProcessorCore(Processor * processor, uint8_t id, const Features & features,
                             ByteOrder initialByteOrder, bool canChangeByteOrder,
                             Mmu * mmu)
    :   _processor(processor),
        _id(id),
        _features(features),
        _initialByteOrder(initialByteOrder),
        _canChangeByteOrder(canChangeByteOrder),
        _isPrimaryCore(_processor->_cores.isEmpty()),   //  1st core created becomes "primary"
        _mmu(mmu),
        //  Registers & state
        _cPtr{&_state.value(),
              &_pth,
              &_itc,
              &_cc,
              &_isaveipTm,
              &_isavestateTm,
              &_ihstateTm,
              &_ihaTm,
              &_isaveipIo,
              &_isavestateIo,
              &_ihstateIo,
              &_ihaIo,
              &_iscIo,
              &_isaveipSvc,
              &_isavestateSvc,
              &_ihstateSvc,
              &_ihaSvc,
              &_isaveipPrg,
              &_isavestatePrg,
              &_ihstatePrg,
              &_ihaPrg,
              &_iscPrg,
              &_isaveipExt,
              &_isavestateExt,
              &_ihstateExt,
              &_ihaExt,
              &_iscExt,
              &_isaveipHw,
              &_isavestateHw,
              &_ihstateHw,
              &_ihaHw,
              &_iscHw},
        _flags(),
        //  Timing characteristics
        _memoryBusToProcessorClockRatio(1),
        _ioBusToProcessorClockRatio(1),
        //  Misc
        _cyclesToStall(0)
{
    Q_ASSERT(_processor != nullptr);
    Q_ASSERT(_mmu != nullptr);

    //  Make sure no two cores have the same ID
    for (ProcessorCore * core : _processor->_cores)
    {
        if (core->_id == _id)
        {   //  OOPS!
            throw hadesvm::core::VirtualApplianceException("Processor 0x" +
                                                           hadesvm::util::toString(_processor->id(), "%02X") +
                                                           " has more than one core with ID = 0x" +
                                                           hadesvm::util::toString(_id, "%02X"));
        }
    }

    //  Create link from processor to this core
    _processor->_cores.append(this);
}

ProcessorCore::~ProcessorCore() noexcept
{
    //  Destroy link from processor to this core
    _processor->_cores.removeOne(this);
}

//////////
//  Operations
void ProcessorCore::reset()
{
    //  Adjust timing characteristics (important for 1st reset on startup)
    _memoryBusToProcessorClockRatio = qMax(1u, _processor->clockFrequency().toHz() / _processor->_memoryBus->clockFrequency().toHz());
    _ioBusToProcessorClockRatio = qMax(1u, _processor->clockFrequency().toHz() / _processor->_ioBus->clockFrequency().toHz());

    //  1.  The W flag of each DMA channel's $state register is set to 0.
    //      (not applicable for processor cores)
    //  TODO implement

    //  2.  All registers of all processors are set to 0, with the exception
    //      of registers explicitly specified below as being set to something else.
    for (int i = 0; i < 32; i++)
    {
        _r[i] = *_cPtr[i] = _d[i] = _m[i] = 0;
    }
    _flags = 0;

    //  3.  In every Cereon system, there is exactly one processor set up as a
    //      primary processor. If the system has more than one processor, all
    //      remaining processors are set up as secondary processors. This processor
    //      type setup is hardwired and cannot be changed. If the primary processor
    //      has more than one core, one of these cores is hardwired as a primary core.

    //  4.  For each processor, whether primary or secondary, the special bootstrap
    //      IP value is hardwired. This value is copied to $ip register.
    _r[_IpRegister] = _processor->_restartAddress;

    //  5.  For each processor, the K flag of $state if set to 1.
    _state.setKernelMode();

    //  6.  For each processor, the B flag of $state is set to reflect the default
    //      byte ordering. Whether this flag can be changed later or not depends on
    //      the processor model.
    _state.setByteOrder(_processor->_byteOrder);

    //  7.  For the primary core of the primary processor, bit 31 of $state is
    //      set to 1, thus allowing HARDWARE interrupts.
    if (_processor->_isPrimaryProcessor && _isPrimaryCore)
    {
        _state.enableHardwareInterrupts();
    }

    //  8.  For all secondary processor cores, bits 30 and 31 of $state is set
    //      to 1, thus allowing EXTERNAL and HARDWARE interrupts.
    if (!_processor->_isPrimaryProcessor && !_isPrimaryCore)
    {
        _state.enableHardwareInterrupts();
        _state.enableExternalInterrupts();
    }

    //  9.  For all secondary processor cores, $ip is copied to $iha.ext and
    //      $state is copied to $ihstate.ext. After the copy, the W flag of $ihstate.ext is set to 1.
    if (!_processor->_isPrimaryProcessor && !_isPrimaryCore)
    {
        _ihaExt = _r[_IpRegister];
        _ihstateExt = _state;
        _ihstateExt |= _StateRegister::WorkingModeMask;
    }

    //  10. For the primary processor, the W flag of $state is set to 1.
    //      This effectively starts the primary processor.
    if (_processor->_isPrimaryProcessor && _isPrimaryCore)
    {
        _state.setWorkingMode();
    }

    //  Finish resetting
    _cyclesToStall = 0;
}

void ProcessorCore::onClockTick()
{
    //  Update $itc
    if (_itc > 1)
    {   //  Just decrement
        _itc--;
    }
    else if (_itc == 1)
    {   //  Decrementing will make it 0. Can a TIMER interrupt occur NOW ?
        if (_state.isTimerInterruptsEnabled() && _cyclesToStall == 0)
        {   //  Yes - decrement $itc to 0 and do it
            _itc = 0;
            _handleTimerInterrupt();
        }
        //  else TIMER interrupt must be postponed, so don't decrement $itc
    }

    //  Are we stalling or Idle ?
    if (_cyclesToStall > 0)
    {
        _cyclesToStall--;
        return;
    }
    else if (_state.isInIdleMode())
    {
        return;
    }

    //  We're Working - increment $cc and handle traps
    _cc++;
    //if (_cc % 100'000'000 == 0)
    //{   //  TODO kill off this "if" - it's a debug code
    //    QDateTime now = QDateTime::currentDateTimeUtc();
    //    QString nowAsString = now.toString(Qt::DateFormat::ISODateWithMs);
    //    qDebug() << "cc = " << _cc << " @ " << nowAsString;
    //}

    if (_state.isInTrapMode())
    {   //  TRAP, unless PROGRAM interrupts are disabled
        if (_state.isProgramInterruptsEnabled())
        {   //  PROGRAM interrupt
            _handleProgramInterrupt(ProgramInterrupt::TRAP);
        }
        //  else suppress PROGRAM interrupt, but keep TRAP flag set
    }
    else if (_state.isInPendingTrapMode())
    {   //  PT -> T, 0 -> PT
        _state.setTrapMode();
        _state.clearPendingTrapMode();
    }

    //  Go!
    try
    {
        try
        {
            unsigned cyclesTaken = _fetchAndExecuteInstruction();
            Q_ASSERT(cyclesTaken > 0 && cyclesTaken <= 1024);
            //  1 cycle has just executed - stall the rest of the way...
            _cyclesToStall = (cyclesTaken == 0) ? 1 : (cyclesTaken - 1);    //  ...but be defensive in release mode
        }
        catch (ProgramInterrupt ex)
        {   //  Raise; halt if masked
            _raiseProgramInterrupt(ex);
            return;
        }
        catch (HardwareInterrupt ex)
        {   //  Raise; halt if masked
            _raiseHardwareInterrupt(ex);
            return;
        }
    }
    catch (const ForceHalt &)
    {
        _state.setIdleMode();
        return;
    }
}

//////////
//  Implementation helpers (memory access)
uint32_t ProcessorCore::_fetchInstruction(uint64_t address) throws(ProgramInterrupt, HardwareInterrupt)
{
    if ((address & 0x03) != 0)
    {   //  OOPS! (even if unaligned operands feature is present)
        throw ProgramInterrupt::IALIGN;
    }

    uint64_t physicalAddress =
        _state.isInVirtualMode() ?
            _mmu->translateFetchAddress(address) :
            address;

    try
    {
        return _processor->_memoryBus->loadWord(physicalAddress, _state.getByteOrder());
    }
    catch (MemoryAccessError err)
    {   //  OOPS! Translate & re-throw
        _translateAndThrowI(err);
    }
}

uint64_t ProcessorCore::_fetchLongWord(uint64_t address) throws(ProgramInterrupt, HardwareInterrupt)
{
    if ((address & 0x07) != 0)
    {   //  OOPS! Instructions must be naturally aligned (even if unaligned operands feature is present)
        throw ProgramInterrupt::IALIGN;
    }

    uint64_t physicalAddress =
        _state.isInVirtualMode() ?
            _mmu->translateFetchAddress(address) :
            address;

    try
    {
        return _processor->_memoryBus->loadLongWord(physicalAddress, _state.getByteOrder());
    }
    catch (MemoryAccessError err)
    {   //  OOPS! Translate & re-throw
        _translateAndThrowD(err);
    }
}

uint8_t ProcessorCore::_loadByte(uint64_t address)
{
    uint64_t physicalAddress =
        _state.isInVirtualMode() ?
            _mmu->translateLoadAddress(address) :
            address;

    try
    {
        return _processor->_memoryBus->loadByte(physicalAddress);
    }
    catch (MemoryAccessError err)
    {   //  OOPS! Translate & re-throw
        _translateAndThrowD(err);
    }
}

uint16_t ProcessorCore::_loadHalfWord(uint64_t address)
{
    if ((address & 0x01) == 0)
    {   //  Naturally aligned
        uint64_t physicalAddress =
            _state.isInVirtualMode() ?
                _mmu->translateLoadAddress(address) :
                address;
        try
        {
            return _processor->_memoryBus->loadHalfWord(physicalAddress, _state.getByteOrder());
        }
        catch (MemoryAccessError err)
        {   //  OOPS! Translate & re-throw
            _translateAndThrowD(err);
        }
    }
    else if (_features.has(Feature::UnalignedOperand))
    {   //  Not naturally aligned - simulate by series of byte loads
        uint16_t result = 0;
        switch (_state.getByteOrder())
        {
            case hadesvm::util::ByteOrder::BigEndian:
                result |= _loadByte(address++);
                result |= _loadByte(address++);
                break;
            case hadesvm::util::ByteOrder::LittleEndian:
                address += 2;
                result |= _loadByte(--address);
                result |= _loadByte(--address);
                break;
            case hadesvm::util::ByteOrder::Unknown:
            default:
                Q_ASSERT(false);
        }
        return result;
    }
    else
    {   //  OOPS! Alignment constraint violated
        throw ProgramInterrupt::DALIGN;
    }
}

uint32_t ProcessorCore::_loadWord(uint64_t address)
{
    if ((address & 0x03) == 0)
    {   //  Naturally aligned
        uint64_t physicalAddress =
            _state.isInVirtualMode() ?
                _mmu->translateLoadAddress(address) :
                address;
        try
        {
            return _processor->_memoryBus->loadWord(physicalAddress, _state.getByteOrder());
        }
        catch (MemoryAccessError err)
        {   //  OOPS! Translate & re-throw
            _translateAndThrowD(err);
        }
    }
    else if (_features.has(Feature::UnalignedOperand))
    {   //  Not naturally aligned - simulate by series of byte loads
        uint32_t result = 0;
        switch (_state.getByteOrder())
        {
            case hadesvm::util::ByteOrder::BigEndian:
                result |= _loadByte(address++);
                result |= _loadByte(address++);
                result |= _loadByte(address++);
                result |= _loadByte(address++);
                break;
            case hadesvm::util::ByteOrder::LittleEndian:
                address += 4;
                result |= _loadByte(--address);
                result |= _loadByte(--address);
                result |= _loadByte(--address);
                result |= _loadByte(--address);
                break;
            case hadesvm::util::ByteOrder::Unknown:
            default:
                Q_ASSERT(false);
        }
        return result;
    }
    else
    {   //  OOPS! Alignment constraint violated
        throw ProgramInterrupt::DALIGN;
    }
}

uint64_t ProcessorCore::_loadLongWord(uint64_t address)
{
    if ((address & 0x07) == 0)
    {   //  Naturally aligned
        uint64_t physicalAddress =
            _state.isInVirtualMode() ?
                _mmu->translateLoadAddress(address) :
                address;
        try
        {
            return _processor->_memoryBus->loadLongWord(physicalAddress, _state.getByteOrder());
        }
        catch (MemoryAccessError err)
        {   //  OOPS! Translate & re-throw
            _translateAndThrowD(err);
        }
    }
    else if (_features.has(Feature::UnalignedOperand))
    {   //  Not naturally aligned - simulate by series of byte loads
        uint64_t result = 0;
        switch (_state.getByteOrder())
        {
            case hadesvm::util::ByteOrder::BigEndian:
                result |= _loadByte(address++);
                result |= _loadByte(address++);
                result |= _loadByte(address++);
                result |= _loadByte(address++);
                result |= _loadByte(address++);
                result |= _loadByte(address++);
                result |= _loadByte(address++);
                result |= _loadByte(address++);
                break;
            case hadesvm::util::ByteOrder::LittleEndian:
                address += 8;
                result |= _loadByte(--address);
                result |= _loadByte(--address);
                result |= _loadByte(--address);
                result |= _loadByte(--address);
                result |= _loadByte(--address);
                result |= _loadByte(--address);
                result |= _loadByte(--address);
                result |= _loadByte(--address);
                break;
            case hadesvm::util::ByteOrder::Unknown:
            default:
                Q_ASSERT(false);
        }
        return result;
    }
    else
    {   //  OOPS! Alignment constraint violated
        throw ProgramInterrupt::DALIGN;
    }
}

void ProcessorCore::_storeByte(uint64_t address, uint8_t value)
{
    uint64_t physicalAddress =
        _state.isInVirtualMode() ?
            _mmu->translateStoreAddress(address) :
            address;

    try
    {
        _processor->_memoryBus->storeByte(physicalAddress, value);
    }
    catch (MemoryAccessError err)
    {   //  OOPS! Translate & re-throw
        _translateAndThrowD(err);
    }
}

void ProcessorCore::_storeHalfWord(uint64_t address, uint16_t value)
{
    if ((address & 0x01) == 0)
    {   //  Naturally aligned
        uint64_t physicalAddress =
            _state.isInVirtualMode() ?
                _mmu->translateStoreAddress(address) :
                address;
        try
        {
            _processor->_memoryBus->storeHalfWord(physicalAddress, value, _state.getByteOrder());
        }
        catch (MemoryAccessError err)
        {   //  OOPS! Translate & re-throw
            _translateAndThrowD(err);
        }
    }
    else if (_features.has(Feature::UnalignedOperand))
    {   //  Not naturally aligned - simulate by series of byte stores
        switch (_state.getByteOrder())
        {
            case hadesvm::util::ByteOrder::BigEndian:
                _storeByte(address++, static_cast<uint8_t>(value >> 8));
                _storeByte(address++, static_cast<uint8_t>(value >> 0));
                break;
            case hadesvm::util::ByteOrder::LittleEndian:
                address += 2;
                _storeByte(--address, static_cast<uint8_t>(value >> 8));
                _storeByte(--address, static_cast<uint8_t>(value >> 0));
                break;
            case hadesvm::util::ByteOrder::Unknown:
            default:
                Q_ASSERT(false);
        }
    }
    else
    {   //  OOPS! Alignment constraint violated
        throw ProgramInterrupt::DALIGN;
    }
}

void ProcessorCore::_storeWord(uint64_t address, uint32_t value)
{
    if ((address & 0x03) == 0)
    {   //  Naturally aligned
        uint64_t physicalAddress =
            _state.isInVirtualMode() ?
                _mmu->translateStoreAddress(address) :
                address;
        try
        {
            _processor->_memoryBus->storeWord(physicalAddress, value, _state.getByteOrder());
        }
        catch (MemoryAccessError err)
        {   //  OOPS! Translate & re-throw
            _translateAndThrowD(err);
        }
    }
    else if (_features.has(Feature::UnalignedOperand))
    {   //  Not naturally aligned - simulate by series of byte stores
        switch (_state.getByteOrder())
        {
            case hadesvm::util::ByteOrder::BigEndian:
                _storeByte(address++, static_cast<uint8_t>(value >> 24));
                _storeByte(address++, static_cast<uint8_t>(value >> 16));
                _storeByte(address++, static_cast<uint8_t>(value >> 8));
                _storeByte(address++, static_cast<uint8_t>(value >> 0));
                break;
            case hadesvm::util::ByteOrder::LittleEndian:
                address += 4;
                _storeByte(--address, static_cast<uint8_t>(value >> 24));
                _storeByte(--address, static_cast<uint8_t>(value >> 16));
                _storeByte(--address, static_cast<uint8_t>(value >> 8));
                _storeByte(--address, static_cast<uint8_t>(value >> 0));
                break;
            case hadesvm::util::ByteOrder::Unknown:
            default:
                Q_ASSERT(false);
        }
    }
    else
    {   //  OOPS! Alignment constraint violated
        throw ProgramInterrupt::DALIGN;
    }
}

void ProcessorCore::_storeLongWord(uint64_t address , uint64_t value)
{
    if ((address & 0x07) == 0)
    {   //  Naturally aligned
        uint64_t physicalAddress =
            _state.isInVirtualMode() ?
                _mmu->translateStoreAddress(address) :
                address;
        try
        {
            _processor->_memoryBus->storeLongWord(physicalAddress, value, _state.getByteOrder());
        }
        catch (MemoryAccessError err)
        {   //  OOPS! Translate & re-throw
            _translateAndThrowD(err);
        }
    }
    else if (_features.has(Feature::UnalignedOperand))
    {   //  Not naturally aligned - simulate by series of byte stores
        switch (_state.getByteOrder())
        {
            case hadesvm::util::ByteOrder::BigEndian:
                _storeByte(address++, static_cast<uint8_t>(value >> 56));
                _storeByte(address++, static_cast<uint8_t>(value >> 48));
                _storeByte(address++, static_cast<uint8_t>(value >> 40));
                _storeByte(address++, static_cast<uint8_t>(value >> 32));
                _storeByte(address++, static_cast<uint8_t>(value >> 24));
                _storeByte(address++, static_cast<uint8_t>(value >> 16));
                _storeByte(address++, static_cast<uint8_t>(value >> 8));
                _storeByte(address++, static_cast<uint8_t>(value >> 0));
                break;
            case hadesvm::util::ByteOrder::LittleEndian:
                address += 8;
                _storeByte(--address, static_cast<uint8_t>(value >> 56));
                _storeByte(--address, static_cast<uint8_t>(value >> 48));
                _storeByte(--address, static_cast<uint8_t>(value >> 40));
                _storeByte(--address, static_cast<uint8_t>(value >> 32));
                _storeByte(--address, static_cast<uint8_t>(value >> 24));
                _storeByte(--address, static_cast<uint8_t>(value >> 16));
                _storeByte(--address, static_cast<uint8_t>(value >> 8));
                _storeByte(--address, static_cast<uint8_t>(value >> 0));
                break;
            case hadesvm::util::ByteOrder::Unknown:
            default:
                Q_ASSERT(false);
        }
    }
    else
    {   //  OOPS! Alignment constraint violated
        throw ProgramInterrupt::DALIGN;
    }
}

//////////
//  Implementation helpers (interrupt handling)
Q_NORETURN void ProcessorCore::_translateAndThrowI(MemoryAccessError memoryAccessError) throws(ProgramInterrupt, HardwareInterrupt)
{
    switch (memoryAccessError)
    {
        case MemoryAccessError::InvalidAddress:
            throw ProgramInterrupt::IADDRESS;
        case MemoryAccessError::InvalidAlignment:
            throw ProgramInterrupt::IALIGN;
        case MemoryAccessError::AccessDenied:
            throw ProgramInterrupt::IACCESS;
        case MemoryAccessError::HardwareFault:
        default:
            throw HardwareInterrupt::MEMORY;
    }
}

Q_NORETURN void ProcessorCore::_translateAndThrowD(MemoryAccessError memoryAccessError) throws(ProgramInterrupt, HardwareInterrupt)
{
    switch (memoryAccessError)
    {
        case MemoryAccessError::InvalidAddress:
            throw ProgramInterrupt::DADDRESS;
        case MemoryAccessError::InvalidAlignment:
            throw ProgramInterrupt::DALIGN;
        case MemoryAccessError::AccessDenied:
            throw ProgramInterrupt::DACCESS;
        case MemoryAccessError::HardwareFault:
        default:
            throw HardwareInterrupt::MEMORY;
    }
}

Q_NORETURN void ProcessorCore::_translateAndThrowIO(IoError ioError) throws(ProgramInterrupt, HardwareInterrupt)
{
    switch (ioError)
    {
        case IoError::NotReady: //  tstp/setp should NEVER be "not ready"
        case IoError::HardwareFault:
        default:
            throw HardwareInterrupt::IO;
    }
}

void ProcessorCore::_handleTimerInterrupt()
{
    Q_ASSERT(_state.isTimerInterruptsEnabled());

    //  TODO make sure state change is valid (e.g. byte order change, etc.)
    _isaveipTm = _r[_IpRegister];
    _isavestateTm = _state;
    _state = _ihstateTm;
    _r[_IpRegister] = _ihaTm;
}

void ProcessorCore::_handleIoInterrupt(uint64_t interruptStatusCode)
{
    Q_ASSERT(_state.isIoInterruptsEnabled());

    //  TODO make sure state change is valid (e.g. byte order change, etc.)
    _isaveipIo = _r[_IpRegister];
    _isavestateIo = _state;
    _iscIo = interruptStatusCode;
    _state = _ihstateIo;
    _r[_IpRegister] = _ihaIo;
}

void ProcessorCore::_handleSvcInterrupt()
{
    Q_ASSERT(_state.isSvcInterruptsEnabled());

    //  TODO make sure state change is valid (e.g. byte order change, etc.)
    _isaveipSvc = _r[_IpRegister];
    _isavestateSvc = _state;
    _state = _ihstateSvc;
    _r[_IpRegister] = _ihaSvc;
}

void ProcessorCore::_handleProgramInterrupt(ProgramInterrupt interruptStatusCode)
{
    Q_ASSERT(_state.isProgramInterruptsEnabled());

    //  TODO make sure state change is valid (e.g. byte order change, etc.)
    _isaveipPrg = _r[_IpRegister];
    _isavestatePrg = _state;
    _iscPrg = static_cast<uint64_t>(interruptStatusCode);
    _state = _ihstatePrg;
    _r[_IpRegister] = _ihaPrg;
}

void ProcessorCore::_handleExternalInterrupt(uint64_t interruptStatusCode)
{
    Q_ASSERT(_state.isExternalInterruptsEnabled());

    //  TODO make sure state change is valid (e.g. byte order change, etc.)
    _isaveipExt = _r[_IpRegister];
    _isavestateExt = _state;
    _iscExt = interruptStatusCode;
    _state = _ihstateExt;
    _r[_IpRegister] = _ihaExt;
}

void ProcessorCore::_handleHardwareInterrupt(HardwareInterrupt interruptStatusCode)
{
    Q_ASSERT(_state.isHardwareInterruptsEnabled());

    //  TODO make sure state change is valid (e.g. byte order change, etc.)
    _isaveipHw = _r[_IpRegister];
    _isavestateHw = _state;
    _iscHw = static_cast<uint64_t>(interruptStatusCode);
    _state = _ihstateHw;
    _r[_IpRegister] = _ihaHw;
}

void ProcessorCore::_raiseProgramInterrupt(ProgramInterrupt interruptStatusCode)
{
    if (_state.isProgramInterruptsEnabled())
    {   //  Can handle
        _handleProgramInterrupt(interruptStatusCode);
    }
    else
    {   //  Masked - HALT
        throw ForceHalt();
    }
}

void ProcessorCore::_raiseHardwareInterrupt(HardwareInterrupt interruptStatusCode)
{
    if (_state.isHardwareInterruptsEnabled())
    {   //  Can handle
        _handleHardwareInterrupt(interruptStatusCode);
    }
    else
    {   //  Masked - HALT
        throw ForceHalt();
    }
}

//////////
//  Implementation helpers (instruction execution)
unsigned ProcessorCore::_fetchAndExecuteInstruction() throws(ProgramInterrupt, HardwareInterrupt)
{
    //  Translate instruction address and fetch the instruction
    uint64_t instructionAddress = _r[_IpRegister];
    _r[_IpRegister] += 4;
    uint32_t instruction = _fetchInstruction(instructionAddress);

    //  Dispatch to handler
    static const _InstructionHandler DispatchTable[64] =
    {
        //  000...
        &ProcessorCore::_handleLiL,                 //  ...000
        &ProcessorCore::_handleCop1,                //  ...001
        &ProcessorCore::_handleAddiL,               //  ...010
        &ProcessorCore::_handleSubiL,               //  ...011
        &ProcessorCore::_handleMuliL,               //  ...100
        &ProcessorCore::_handleDiviL,               //  ...101
        &ProcessorCore::_handleModiL,               //  ...110
        &ProcessorCore::_handleJ,                   //  ...111
        //  001...
        &ProcessorCore::_handleLiD,                 //  ...000
        &ProcessorCore::_handleLir,                 //  ...001
        &ProcessorCore::_handleAddiUL,              //  ...010
        &ProcessorCore::_handleSubiUL,              //  ...011
        &ProcessorCore::_handleMuliUL,              //  ...100
        &ProcessorCore::_handleDiviUL,              //  ...101
        &ProcessorCore::_handleModiUL,              //  ...110
        &ProcessorCore::_handleJal,                 //  ...111
        //  010...
        &ProcessorCore::_handleSeqiL,               //  ...000
        &ProcessorCore::_handleSneiL,               //  ...001
        &ProcessorCore::_handleSltiL,               //  ...010
        &ProcessorCore::_handleSleiL,               //  ...011
        &ProcessorCore::_handleSgtiL,               //  ...100
        &ProcessorCore::_handleSgeiL,               //  ...101
        &ProcessorCore::_handleSltiUL,              //  ...110
        &ProcessorCore::_handleSleiUL,              //  ...111
        //  011...
        &ProcessorCore::_handleAndiL,               //  ...000
        &ProcessorCore::_handleOriL,                //  ...001
        &ProcessorCore::_handleXoriL,               //  ...010
        &ProcessorCore::_handleImpliL,              //  ...011
        &ProcessorCore::_handleLdm,                 //  ...100
        &ProcessorCore::_handleStm,                 //  ...101
        &ProcessorCore::_handleSgtiUL,              //  ...110
        &ProcessorCore::_handleSgeiUL,              //  ...111
        //  100...
        &ProcessorCore::_handleLB,                  //  ...000
        &ProcessorCore::_handleLUB,                 //  ...001
        &ProcessorCore::_handleLH,                  //  ...010
        &ProcessorCore::_handleLUH,                 //  ...011
        &ProcessorCore::_handleLW,                  //  ...100
        &ProcessorCore::_handleLUW,                 //  ...101
        &ProcessorCore::_handleLL,                  //  ...110
        &ProcessorCore::_handleXchg,                //  ...111
        //  101...
        &ProcessorCore::_handleSB,                  //  ...000
        &ProcessorCore::_handleSH,                  //  ...001
        &ProcessorCore::_handleSW,                  //  ...010
        &ProcessorCore::_handleSL,                  //  ...011
        &ProcessorCore::_handleLF,                  //  ...100
        &ProcessorCore::_handleLD,                  //  ...101
        &ProcessorCore::_handleSF,                  //  ...110
        &ProcessorCore::_handleSD,                  //  ...111
        //  110...
        &ProcessorCore::_handleBeqL,                //  ...000
        &ProcessorCore::_handleBneL,                //  ...001
        &ProcessorCore::_handleBltL,                //  ...010
        &ProcessorCore::_handleBleL,                //  ...011
        &ProcessorCore::_handleBgtL,                //  ...100
        &ProcessorCore::_handleBgeL,                //  ...101
        &ProcessorCore::_handleBltUL,               //  ...110
        &ProcessorCore::_handleBleUL,               //  ...111
        //  111...
        &ProcessorCore::_handleBeqD,                //  ...000
        &ProcessorCore::_handleBneD,                //  ...001
        &ProcessorCore::_handleBltD,                //  ...010
        &ProcessorCore::_handleBleD,                //  ...011
        &ProcessorCore::_handleBgtD,                //  ...100
        &ProcessorCore::_handleBgeD,                //  ...101
        &ProcessorCore::_handleBgtUL,               //  ...110
        &ProcessorCore::_handleBgeUL                //  ...111
    };

    _InstructionHandler handler = DispatchTable[instruction >> 26];
    return (this->*handler)(instruction);
}

unsigned ProcessorCore::_handleInvalidInstruction(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    //  TODO kill off the printout
    qDebug() << "Unsupported instruction "
             << hadesvm::util::toString(instruction, "%08X")
             << " at "
             << hadesvm::util::toString(_r[_IpRegister] - 4, "%016X")
             << "\n";
    throw ProgramInterrupt::OPCODE;
}

unsigned ProcessorCore::_handleCop1(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    static const _InstructionHandler DispatchTable[32] =
    {
        //  00...
        &ProcessorCore::_handleShift1,              //  ..000
        &ProcessorCore::_handleShift2,              //  ..001
        &ProcessorCore::_handleBfiL,                //  ..010
        &ProcessorCore::_handleBfiL,                //  ..011
        &ProcessorCore::_handleBfeL,                //  ..100
        &ProcessorCore::_handleBfeL,                //  ..101
        &ProcessorCore::_handleBfeUL,               //  ..110
        &ProcessorCore::_handleBfeUL,               //  ..111
        //  01...
        &ProcessorCore::_handleBase1,               //  ..000
        &ProcessorCore::_handleBase2,               //  ..001
        &ProcessorCore::_handleBase3,               //  ..010
        &ProcessorCore::_handleBase4,               //  ..011
        &ProcessorCore::_handleBase5,               //  ..100
        &ProcessorCore::_handleInvalidInstruction,  //  ..101
        &ProcessorCore::_handleInvalidInstruction,  //  ..110
        &ProcessorCore::_handleInvalidInstruction,  //  ..111
        //  10...
        &ProcessorCore::_handleFp1,                 //  ..000
        &ProcessorCore::_handleInvalidInstruction,  //  ..001
        &ProcessorCore::_handleInvalidInstruction,  //  ..010
        &ProcessorCore::_handleInvalidInstruction,  //  ..011
        &ProcessorCore::_handleInvalidInstruction,  //  ..100
        &ProcessorCore::_handleInvalidInstruction,  //  ..101
        &ProcessorCore::_handleInvalidInstruction,  //  ..110
        &ProcessorCore::_handleInvalidInstruction,  //  ..111
        //  11...
        &ProcessorCore::_handleInvalidInstruction,  //  ..000
        &ProcessorCore::_handleInvalidInstruction,  //  ..001
        &ProcessorCore::_handleInvalidInstruction,  //  ..010
        &ProcessorCore::_handleInvalidInstruction,  //  ..011
        &ProcessorCore::_handleInvalidInstruction,  //  ..100
        &ProcessorCore::_handleInvalidInstruction,  //  ..101
        &ProcessorCore::_handleInvalidInstruction,  //  ..110
        &ProcessorCore::_handleInvalidInstruction,  //  ..111
    };

    _InstructionHandler handler = DispatchTable[(instruction >> 6) & 0x1F];
    return (this->*handler)(instruction);
}

unsigned ProcessorCore::_handleShift1(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    static const _InstructionHandler DispatchTable[32] =
    {
        //  00...
        &ProcessorCore::_handleShliB,               //  ..000
        &ProcessorCore::_handleShliUB,              //  ..001
        &ProcessorCore::_handleShliH,               //  ..010
        &ProcessorCore::_handleShliUH,              //  ..011
        &ProcessorCore::_handleShliW,               //  ..100
        &ProcessorCore::_handleShliUW,              //  ..101
        &ProcessorCore::_handleShliL,               //  ..110
        &ProcessorCore::_handleInvalidInstruction,  //  ..111
        //  01...
        &ProcessorCore::_handleShriB,               //  ..000
        &ProcessorCore::_handleShriUB,              //  ..001
        &ProcessorCore::_handleShriH,               //  ..010
        &ProcessorCore::_handleShriUH,              //  ..011
        &ProcessorCore::_handleShriW,               //  ..100
        &ProcessorCore::_handleShriUW,              //  ..101
        &ProcessorCore::_handleShriL,               //  ..110
        &ProcessorCore::_handleInvalidInstruction,  //  ..111
        //  10...
        &ProcessorCore::_handleAsliB,               //  ..000
        &ProcessorCore::_handleAsliUB,              //  ..001
        &ProcessorCore::_handleAsliH,               //  ..010
        &ProcessorCore::_handleAsliUH,              //  ..011
        &ProcessorCore::_handleAsliW,               //  ..100
        &ProcessorCore::_handleAsliUW,              //  ..101
        &ProcessorCore::_handleAsliL,               //  ..110
        &ProcessorCore::_handleInvalidInstruction,  //  ..111
        //  11...
        &ProcessorCore::_handleAsriB,               //  ..000
        &ProcessorCore::_handleAsriUB,              //  ..001
        &ProcessorCore::_handleAsriH,               //  ..010
        &ProcessorCore::_handleAsriUH,              //  ..011
        &ProcessorCore::_handleAsriW,               //  ..100
        &ProcessorCore::_handleAsriUW,              //  ..101
        &ProcessorCore::_handleAsriL,               //  ..110
        &ProcessorCore::_handleInvalidInstruction,  //  ..111
    };

    _InstructionHandler handler = DispatchTable[(instruction >> 11) & 0x1F];
    return (this->*handler)(instruction);
}

unsigned ProcessorCore::_handleShift2(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    static const _InstructionHandler DispatchTable[32] =
    {
        //  00...
        &ProcessorCore::_handleRoliB,               //  ..000
        &ProcessorCore::_handleRoliUB,              //  ..001
        &ProcessorCore::_handleRoliH,               //  ..010
        &ProcessorCore::_handleRoliUH,              //  ..011
        &ProcessorCore::_handleRoliW,               //  ..100
        &ProcessorCore::_handleRoliUW,              //  ..101
        &ProcessorCore::_handleRoliL,               //  ..110
        &ProcessorCore::_handleInvalidInstruction,  //  ..111
        //  01...
        &ProcessorCore::_handleRoriB,               //  ..000
        &ProcessorCore::_handleRoriUB,              //  ..001
        &ProcessorCore::_handleRoriH,               //  ..010
        &ProcessorCore::_handleRoriUH,              //  ..011
        &ProcessorCore::_handleRoriW,               //  ..100
        &ProcessorCore::_handleRoriUW,              //  ..101
        &ProcessorCore::_handleRoriL,               //  ..110
        &ProcessorCore::_handleInvalidInstruction,  //  ..111
        //  10...
        &ProcessorCore::_handleBeqiL,               //  ..000
        &ProcessorCore::_handleBneiL,               //  ..001
        &ProcessorCore::_handleBltiL,               //  ..010
        &ProcessorCore::_handleBleiL,               //  ..011
        &ProcessorCore::_handleBgtiL,               //  ..100
        &ProcessorCore::_handleBgeiL,               //  ..101
        &ProcessorCore::_handleInvalidInstruction,  //  ..110
        &ProcessorCore::_handleInvalidInstruction,  //  ..111
        //  11...
        &ProcessorCore::_handleInvalidInstruction,  //  ..000
        &ProcessorCore::_handleInvalidInstruction,  //  ..001
        &ProcessorCore::_handleBltiUL,              //  ..010
        &ProcessorCore::_handleBleiUL,              //  ..011
        &ProcessorCore::_handleBgtiUL,              //  ..100
        &ProcessorCore::_handleBgeiUL,              //  ..101
        &ProcessorCore::_handleInvalidInstruction,  //  ..110
        &ProcessorCore::_handleInvalidInstruction,  //  ..111
    };

    _InstructionHandler handler = DispatchTable[(instruction >> 11) & 0x1F];
    return (this->*handler)(instruction);
}

unsigned ProcessorCore::_handleBase1(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    static const _InstructionHandler DispatchTable[64] =
    {
        //  000...
        &ProcessorCore::_handleMovCR,               //  ...000
        &ProcessorCore::_handleMovRC,               //  ...001
        &ProcessorCore::_handleInvalidInstruction,  //  ...010
        &ProcessorCore::_handleInvalidInstruction,  //  ...011
        &ProcessorCore::_handleInvalidInstruction,  //  ...100
        &ProcessorCore::_handleInvalidInstruction,  //  ...101
        &ProcessorCore::_handleInvalidInstruction,  //  ...110
        &ProcessorCore::_handleInvalidInstruction,  //  ...111
        //  001...
        &ProcessorCore::_handleInvalidInstruction,  //  ...000
        &ProcessorCore::_handleInvalidInstruction,  //  ...001
        &ProcessorCore::_handleInvalidInstruction,  //  ...010
        &ProcessorCore::_handleInvalidInstruction,  //  ...011
        &ProcessorCore::_handleInvalidInstruction,  //  ...100
        &ProcessorCore::_handleInvalidInstruction,  //  ...101
        &ProcessorCore::_handleInvalidInstruction,  //  ...110
        &ProcessorCore::_handleInvalidInstruction,  //  ...111
        //  010...
        &ProcessorCore::_handleInvalidInstruction,  //  ...000
        &ProcessorCore::_handleInvalidInstruction,  //  ...001
        &ProcessorCore::_handleInvalidInstruction,  //  ...010
        &ProcessorCore::_handleInvalidInstruction,  //  ...011
        &ProcessorCore::_handleInvalidInstruction,  //  ...100
        &ProcessorCore::_handleInvalidInstruction,  //  ...101
        &ProcessorCore::_handleInvalidInstruction,  //  ...110
        &ProcessorCore::_handleInvalidInstruction,  //  ...111
        //  011...
        &ProcessorCore::_handleIret,                //  ...000
        &ProcessorCore::_handleHalt,                //  ...001
        &ProcessorCore::_handleCpuid,               //  ...010
        &ProcessorCore::_handleSigp,                //  ...011
        &ProcessorCore::_handleInvalidInstruction,  //  ...100
        &ProcessorCore::_handleInvalidInstruction,  //  ...101
        &ProcessorCore::_handleInvalidInstruction,  //  ...110
        &ProcessorCore::_handleInvalidInstruction,  //  ...111
        //  100...
        &ProcessorCore::_handleTstp,                //  ...000
        &ProcessorCore::_handleSetp,                //  ...001
        &ProcessorCore::_handleInvalidInstruction,  //  ...010
        &ProcessorCore::_handleInvalidInstruction,  //  ...011
        &ProcessorCore::_handleInvalidInstruction,  //  ...100
        &ProcessorCore::_handleInvalidInstruction,  //  ...101
        &ProcessorCore::_handleInvalidInstruction,  //  ...110
        &ProcessorCore::_handleInvalidInstruction,  //  ...111
        //  101...
        &ProcessorCore::_handleInB,                 //  ...000
        &ProcessorCore::_handleInH,                 //  ...001
        &ProcessorCore::_handleInW,                 //  ...010
        &ProcessorCore::_handleInL,                 //  ...011
        &ProcessorCore::_handleOutB,                //  ...100
        &ProcessorCore::_handleOutH,                //  ...101
        &ProcessorCore::_handleOutB,                //  ...110
        &ProcessorCore::_handleOutL,                //  ...111
        //  110...
        &ProcessorCore::_handleInUB,                //  ...000
        &ProcessorCore::_handleInUH,                //  ...001
        &ProcessorCore::_handleInUW,                //  ...010
        &ProcessorCore::_handleInvalidInstruction,  //  ...011
        &ProcessorCore::_handleInvalidInstruction,  //  ...100
        &ProcessorCore::_handleInvalidInstruction,  //  ...101
        &ProcessorCore::_handleInvalidInstruction,  //  ...110
        &ProcessorCore::_handleInvalidInstruction,  //  ...111
        //  111...
        &ProcessorCore::_handleInvalidInstruction,  //  ...000
        &ProcessorCore::_handleInvalidInstruction,  //  ...001
        &ProcessorCore::_handleInvalidInstruction,  //  ...010
        &ProcessorCore::_handleInvalidInstruction,  //  ...011
        &ProcessorCore::_handleInvalidInstruction,  //  ...100
        &ProcessorCore::_handleInvalidInstruction,  //  ...101
        &ProcessorCore::_handleInvalidInstruction,  //  ...110
        &ProcessorCore::_handleInvalidInstruction   //  ...111
    };

    _InstructionHandler handler = DispatchTable[instruction & 0x3F];
    return (this->*handler)(instruction);
}

unsigned ProcessorCore::_handleBase2(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    static const _InstructionHandler DispatchTable[64] =
    {
        //  000...
        &ProcessorCore::_handleMovL,                //  ...000
        &ProcessorCore::_handleCvtBL,               //  ...001
        &ProcessorCore::_handleCvtUBL,              //  ...010
        &ProcessorCore::_handleCvtHL,               //  ...011
        &ProcessorCore::_handleCvtUHL,              //  ...100
        &ProcessorCore::_handleCvtWL,               //  ...101
        &ProcessorCore::_handleCvtUWL,              //  ...110
        &ProcessorCore::_handleNop,                 //  ...111
        //  001...
        &ProcessorCore::_handleAndB,                //  ...000
        &ProcessorCore::_handleAndUB,               //  ...001
        &ProcessorCore::_handleAndH,                //  ...010
        &ProcessorCore::_handleAndUH,               //  ...011
        &ProcessorCore::_handleAndW,                //  ...100
        &ProcessorCore::_handleAndUW,               //  ...101
        &ProcessorCore::_handleAndL,                //  ...110
        &ProcessorCore::_handleSwapH,               //  ...111
        //  010...
        &ProcessorCore::_handleOrB,                 //  ...000
        &ProcessorCore::_handleOrUB,                //  ...001
        &ProcessorCore::_handleOrH,                 //  ...010
        &ProcessorCore::_handleOrUH,                //  ...011
        &ProcessorCore::_handleOrW,                 //  ...100
        &ProcessorCore::_handleOrUW,                //  ...101
        &ProcessorCore::_handleOrL,                 //  ...110
        &ProcessorCore::_handleSwapUH,              //  ...111
        //  011...
        &ProcessorCore::_handleXorB,                //  ...000
        &ProcessorCore::_handleXorUB,               //  ...001
        &ProcessorCore::_handleXorH,                //  ...010
        &ProcessorCore::_handleXorUH,               //  ...011
        &ProcessorCore::_handleXorW,                //  ...100
        &ProcessorCore::_handleXorUW,               //  ...101
        &ProcessorCore::_handleXorL,                //  ...110
        &ProcessorCore::_handleSwapW,               //  ...111
        //  100...
        &ProcessorCore::_handleNotB,                //  ...000
        &ProcessorCore::_handleNotUB,               //  ...001
        &ProcessorCore::_handleNotH,                //  ...010
        &ProcessorCore::_handleNotUH,               //  ...011
        &ProcessorCore::_handleNotW,                //  ...100
        &ProcessorCore::_handleNotUW,               //  ...101
        &ProcessorCore::_handleNotL,                //  ...110
        &ProcessorCore::_handleSwapUW,              //  ...111
        //  101...
        &ProcessorCore::_handleBrevB,               //  ...000
        &ProcessorCore::_handleBrevUB,              //  ...001
        &ProcessorCore::_handleBrevH,               //  ...010
        &ProcessorCore::_handleBrevUH,              //  ...011
        &ProcessorCore::_handleBrevW,               //  ...100
        &ProcessorCore::_handleBrevUW,              //  ...101
        &ProcessorCore::_handleBrevL,               //  ...110
        &ProcessorCore::_handleSwapL,               //  ...111
        //  110...
        &ProcessorCore::_handleSeqL,                //  ...000
        &ProcessorCore::_handleSneL,                //  ...001
        &ProcessorCore::_handleSltL,                //  ...010
        &ProcessorCore::_handleSleL,                //  ...011
        &ProcessorCore::_handleSgtL,                //  ...100
        &ProcessorCore::_handleSgeL,                //  ...101
        &ProcessorCore::_handleClz,                 //  ...110
        &ProcessorCore::_handleCtz,                 //  ...111
        //  111...
        &ProcessorCore::_handleJr,                  //  ...000
        &ProcessorCore::_handleJalr,                //  ...001
        &ProcessorCore::_handleSltUL,               //  ...010
        &ProcessorCore::_handleSleUL,               //  ...011
        &ProcessorCore::_handleSgtUL,               //  ...100
        &ProcessorCore::_handleSgeUL,               //  ...101
        &ProcessorCore::_handleClo,                 //  ...110
        &ProcessorCore::_handleCto                  //  ...111
    };

    _InstructionHandler handler = DispatchTable[instruction & 0x3F];
    return (this->*handler)(instruction);
}

unsigned ProcessorCore::_handleBase3(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    static const _InstructionHandler DispatchTable[64] =
    {
        //  000...
        &ProcessorCore::_handleAddB,                //  ...000
        &ProcessorCore::_handleSubB,                //  ...001
        &ProcessorCore::_handleMulB,                //  ...010
        &ProcessorCore::_handleDivB,                //  ...011
        &ProcessorCore::_handleModB,                //  ...100
        &ProcessorCore::_handleAbsB,                //  ...101
        &ProcessorCore::_handleNegB,                //  ...110
        &ProcessorCore::_handleImplB,               //  ...111
        //  001...
        &ProcessorCore::_handleAddUB,               //  ...000
        &ProcessorCore::_handleSubUB,               //  ...001
        &ProcessorCore::_handleMulUB,               //  ...010
        &ProcessorCore::_handleDivUB,               //  ...011
        &ProcessorCore::_handleModUB,               //  ...100
        &ProcessorCore::_handleInvalidInstruction,  //  ...101
        &ProcessorCore::_handleCpl2UB,              //  ...110
        &ProcessorCore::_handleImplUB,              //  ...111
        //  010...
        &ProcessorCore::_handleAddH,                //  ...000
        &ProcessorCore::_handleSubH,                //  ...001
        &ProcessorCore::_handleMulH,                //  ...010
        &ProcessorCore::_handleDivH,                //  ...011
        &ProcessorCore::_handleModH,                //  ...100
        &ProcessorCore::_handleAbsH,                //  ...101
        &ProcessorCore::_handleNegH,                //  ...110
        &ProcessorCore::_handleImplH,               //  ...111
        //  011...
        &ProcessorCore::_handleAddUH,               //  ...000
        &ProcessorCore::_handleSubUH,               //  ...001
        &ProcessorCore::_handleMulUH,               //  ...010
        &ProcessorCore::_handleDivUH,               //  ...011
        &ProcessorCore::_handleModUH,               //  ...100
        &ProcessorCore::_handleInvalidInstruction,  //  ...101
        &ProcessorCore::_handleCpl2UH,              //  ...110
        &ProcessorCore::_handleImplUH,              //  ...111
        //  100...
        &ProcessorCore::_handleAddW,                //  ...000
        &ProcessorCore::_handleSubW,                //  ...001
        &ProcessorCore::_handleMulW,                //  ...010
        &ProcessorCore::_handleDivW,                //  ...011
        &ProcessorCore::_handleModW,                //  ...100
        &ProcessorCore::_handleAbsW,                //  ...101
        &ProcessorCore::_handleNegW,                //  ...110
        &ProcessorCore::_handleImplW,               //  ...111
        //  101...
        &ProcessorCore::_handleAddUW,               //  ...000
        &ProcessorCore::_handleSubUW,               //  ...001
        &ProcessorCore::_handleMulUW,               //  ...010
        &ProcessorCore::_handleDivUW,               //  ...011
        &ProcessorCore::_handleModUW,               //  ...100
        &ProcessorCore::_handleInvalidInstruction,  //  ...101
        &ProcessorCore::_handleCpl2UW,              //  ...110
        &ProcessorCore::_handleImplUW,              //  ...111
        //  110...
        &ProcessorCore::_handleAddL,                //  ...000
        &ProcessorCore::_handleSubL,                //  ...001
        &ProcessorCore::_handleMulL,                //  ...010
        &ProcessorCore::_handleDivL,                //  ...011
        &ProcessorCore::_handleModL,                //  ...100
        &ProcessorCore::_handleAbsL,                //  ...101
        &ProcessorCore::_handleNegL,                //  ...110
        &ProcessorCore::_handleImplL,               //  ...111
        //  111...
        &ProcessorCore::_handleAddUL,               //  ...000
        &ProcessorCore::_handleSubUL,               //  ...001
        &ProcessorCore::_handleMulUL,               //  ...010
        &ProcessorCore::_handleDivUL,               //  ...011
        &ProcessorCore::_handleModUL,               //  ...100
        &ProcessorCore::_handleInvalidInstruction,  //  ...101
        &ProcessorCore::_handleCpl2UL,              //  ...110
        &ProcessorCore::_handleInvalidInstruction   //  ...111
    };

    _InstructionHandler handler = DispatchTable[instruction & 0x3F];
    return (this->*handler)(instruction);
}

unsigned ProcessorCore::_handleBase4(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    static const _InstructionHandler DispatchTable[32] =
    {
        //  00...
        &ProcessorCore::_handleAddiB,               //  ..000
        &ProcessorCore::_handleAddiUB,              //  ..001
        &ProcessorCore::_handleAddiH,               //  ..010
        &ProcessorCore::_handleAddiUH,              //  ..011
        &ProcessorCore::_handleAddiW,               //  ..100
        &ProcessorCore::_handleAddiUW,              //  ..101
        &ProcessorCore::_handleModiB,               //  ..110
        &ProcessorCore::_handleModiUB,              //  ..111
        //  01...
        &ProcessorCore::_handleSubiB,               //  ..000
        &ProcessorCore::_handleSubiUB,              //  ..001
        &ProcessorCore::_handleSubiH,               //  ..010
        &ProcessorCore::_handleSubiUH,              //  ..011
        &ProcessorCore::_handleSubiW,               //  ..100
        &ProcessorCore::_handleSubiUW,              //  ..101
        &ProcessorCore::_handleModiH,               //  ..110
        &ProcessorCore::_handleModiUH,              //  ..111
        //  10...
        &ProcessorCore::_handleMuliB,               //  ..000
        &ProcessorCore::_handleMuliUB,              //  ..001
        &ProcessorCore::_handleMuliH,               //  ..010
        &ProcessorCore::_handleMuliUH,              //  ..011
        &ProcessorCore::_handleMuliW,               //  ..100
        &ProcessorCore::_handleMuliUW,              //  ..101
        &ProcessorCore::_handleModiW,               //  ..110
        &ProcessorCore::_handleModiUW,              //  ..111
        //  11...
        &ProcessorCore::_handleDiviB,               //  ..000
        &ProcessorCore::_handleDiviUB,              //  ..001
        &ProcessorCore::_handleDiviH,               //  ..010
        &ProcessorCore::_handleDiviUH,              //  ..011
        &ProcessorCore::_handleDiviW,               //  ..100
        &ProcessorCore::_handleDiviUW,              //  ..101
        &ProcessorCore::_handleInvalidInstruction,  //  ..110
        &ProcessorCore::_handleInvalidInstruction,  //  ..111
    };

    _InstructionHandler handler = DispatchTable[(instruction >> 11) & 0x1F];
    return (this->*handler)(instruction);
}

unsigned ProcessorCore::_handleBase5(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    static const _InstructionHandler DispatchTable[64] =
    {
        //  000...
        &ProcessorCore::_handleShlB,                //  ...000
        &ProcessorCore::_handleShrB,                //  ...001
        &ProcessorCore::_handleAslB,                //  ...010
        &ProcessorCore::_handleAsrB,                //  ...011
        &ProcessorCore::_handleRolB,                //  ...100
        &ProcessorCore::_handleRorB,                //  ...101
        &ProcessorCore::_handleInvalidInstruction,  //  ...110
        &ProcessorCore::_handleInvalidInstruction,  //  ...111
        //  001...
        &ProcessorCore::_handleShlH,                //  ...000
        &ProcessorCore::_handleShrH,                //  ...001
        &ProcessorCore::_handleAslH,                //  ...010
        &ProcessorCore::_handleAsrH,                //  ...011
        &ProcessorCore::_handleRolH,                //  ...100
        &ProcessorCore::_handleRorH,                //  ...101
        &ProcessorCore::_handleInvalidInstruction,  //  ...110
        &ProcessorCore::_handleInvalidInstruction,  //  ...111
        //  010...
        &ProcessorCore::_handleShlW,                //  ...000
        &ProcessorCore::_handleShrW,                //  ...001
        &ProcessorCore::_handleAslW,                //  ...010
        &ProcessorCore::_handleAsrW,                //  ...011
        &ProcessorCore::_handleRolW,                //  ...100
        &ProcessorCore::_handleRorW,                //  ...101
        &ProcessorCore::_handleInvalidInstruction,  //  ...110
        &ProcessorCore::_handleInvalidInstruction,  //  ...111
        //  011...
        &ProcessorCore::_handleShlUB,               //  ...000
        &ProcessorCore::_handleShrUB,               //  ...001
        &ProcessorCore::_handleAslUB,               //  ...010
        &ProcessorCore::_handleAsrUB,               //  ...011
        &ProcessorCore::_handleRolUB,               //  ...100
        &ProcessorCore::_handleRorUB,               //  ...101
        &ProcessorCore::_handleInvalidInstruction,  //  ...110
        &ProcessorCore::_handleInvalidInstruction,  //  ...111
        //  100...
        &ProcessorCore::_handleShlUH,               //  ...000
        &ProcessorCore::_handleShrUH,               //  ...001
        &ProcessorCore::_handleAslUH,               //  ...010
        &ProcessorCore::_handleAsrUH,               //  ...011
        &ProcessorCore::_handleRolUH,               //  ...100
        &ProcessorCore::_handleRorUH,               //  ...101
        &ProcessorCore::_handleInvalidInstruction,  //  ...110
        &ProcessorCore::_handleInvalidInstruction,  //  ...111
        //  101...
        &ProcessorCore::_handleShlUW,               //  ...000
        &ProcessorCore::_handleShrUW,               //  ...001
        &ProcessorCore::_handleAslUW,               //  ...010
        &ProcessorCore::_handleAsrUW,               //  ...011
        &ProcessorCore::_handleRolUW,               //  ...100
        &ProcessorCore::_handleRorUW,               //  ...101
        &ProcessorCore::_handleInvalidInstruction,  //  ...110
        &ProcessorCore::_handleInvalidInstruction,  //  ...111
        //  110...
        &ProcessorCore::_handleShlL,                //  ...000
        &ProcessorCore::_handleShrL,                //  ...001
        &ProcessorCore::_handleAslL,                //  ...010
        &ProcessorCore::_handleAsrL,                //  ...011
        &ProcessorCore::_handleRolL,                //  ...100
        &ProcessorCore::_handleRorL,                //  ...101
        &ProcessorCore::_handleInvalidInstruction,  //  ...110
        &ProcessorCore::_handleInvalidInstruction,  //  ...111
        //  111...
        &ProcessorCore::_handleGetfl,               //  ...000
        &ProcessorCore::_handleSetfl,               //  ...001
        &ProcessorCore::_handleRstfl,               //  ...010
        &ProcessorCore::_handleInvalidInstruction,  //  ...011
        &ProcessorCore::_handleInvalidInstruction,  //  ...100
        &ProcessorCore::_handleSvc,                 //  ...101
        &ProcessorCore::_handleBrk,                 //  ...110
        &ProcessorCore::_handleInvalidInstruction   //  ...111
    };

    _InstructionHandler handler = DispatchTable[instruction & 0x3F];
    return (this->*handler)(instruction);
}

unsigned ProcessorCore::_handleFp1(uint32_t instruction) throws(ProgramInterrupt, HardwareInterrupt)
{
    static const _InstructionHandler DispatchTable[64] =
    {
        //  000...
        &ProcessorCore::_handleMovD,                //  ...000
        &ProcessorCore::_handleCvtDF,               //  ...001
        &ProcessorCore::_handleInvalidInstruction,  //  ...010
        &ProcessorCore::_handleInvalidInstruction,  //  ...011
        &ProcessorCore::_handleInvalidInstruction,  //  ...100
        &ProcessorCore::_handleInvalidInstruction,  //  ...101
        &ProcessorCore::_handleInvalidInstruction,  //  ...110
        &ProcessorCore::_handleInvalidInstruction,  //  ...111
        //  001...
        &ProcessorCore::_handleAddD,                //  ...000
        &ProcessorCore::_handleSubD,                //  ...001
        &ProcessorCore::_handleMulD,                //  ...010
        &ProcessorCore::_handleDivD,                //  ...011
        &ProcessorCore::_handleAbsD,                //  ...100
        &ProcessorCore::_handleNegD,                //  ...101
        &ProcessorCore::_handleSqrtD,               //  ...110
        &ProcessorCore::_handleInvalidInstruction,  //  ...111
        //  010...
        & ProcessorCore::_handleAddF,               //  ...000
        & ProcessorCore::_handleSubF,               //  ...001
        & ProcessorCore::_handleMulF,               //  ...010
        & ProcessorCore::_handleDivF,               //  ...011
        & ProcessorCore::_handleAbsF,               //  ...100
        & ProcessorCore::_handleNegF,               //  ...101
        & ProcessorCore::_handleSqrtF,              //  ...110
        &ProcessorCore::_handleInvalidInstruction,  //  ...111
        //  011...
        &ProcessorCore::_handleSeqD,                //  ...000
        &ProcessorCore::_handleSneD,                //  ...001
        &ProcessorCore::_handleSltD,                //  ...010
        &ProcessorCore::_handleSleD,                //  ...011
        &ProcessorCore::_handleSgtD,                //  ...100
        &ProcessorCore::_handleSgeD,                //  ...101
        &ProcessorCore::_handleInvalidInstruction,  //  ...110
        &ProcessorCore::_handleInvalidInstruction,  //  ...111
        //  100...
        &ProcessorCore::_handleCvtFB,               //  ...000
        &ProcessorCore::_handleCvtFH,               //  ...001
        &ProcessorCore::_handleCvtFW,               //  ...010
        &ProcessorCore::_handleCvtFL,               //  ...011
        &ProcessorCore::_handleCvtFUB,              //  ...100
        &ProcessorCore::_handleCvtFUH,              //  ...101
        &ProcessorCore::_handleCvtFUW,              //  ...110
        &ProcessorCore::_handleCvtFUL,              //  ...111
        //  101...
        &ProcessorCore::_handleCvtBF,               //  ...000
        &ProcessorCore::_handleCvtHF,               //  ...001
        &ProcessorCore::_handleCvtWF,               //  ...010
        &ProcessorCore::_handleCvtLF,               //  ...011
        &ProcessorCore::_handleCvtUBF,              //  ...100
        &ProcessorCore::_handleCvtUHF,              //  ...101
        &ProcessorCore::_handleCvtUWF,              //  ...110
        &ProcessorCore::_handleCvtULF,              //  ...111
        //  110...
        & ProcessorCore::_handleCvtDB,               //  ...000
        & ProcessorCore::_handleCvtDH,               //  ...001
        & ProcessorCore::_handleCvtDW,               //  ...010
        & ProcessorCore::_handleCvtDL,               //  ...011
        & ProcessorCore::_handleCvtDUB,              //  ...100
        & ProcessorCore::_handleCvtDUH,              //  ...101
        & ProcessorCore::_handleCvtDUW,              //  ...110
        & ProcessorCore::_handleCvtDUL,              //  ...111
        //  111...
        & ProcessorCore::_handleCvtBD,               //  ...000
        & ProcessorCore::_handleCvtHD,               //  ...001
        & ProcessorCore::_handleCvtWD,               //  ...010
        & ProcessorCore::_handleCvtLD,               //  ...011
        & ProcessorCore::_handleCvtUBD,              //  ...100
        & ProcessorCore::_handleCvtUHD,              //  ...101
        & ProcessorCore::_handleCvtUWD,              //  ...110
        & ProcessorCore::_handleCvtULD               //  ...111
    };

    _InstructionHandler handler = DispatchTable[instruction & 0x3F];
    return (this->*handler)(instruction);
}

//  End of hadesvm-cereon/ProcessorCore.cpp
