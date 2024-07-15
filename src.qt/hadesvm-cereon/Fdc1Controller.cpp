//
//  hadesvm-cereon/Fdc1Controller.cpp
//
//  hadesvm::cereon::Fdc1Controller class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Constants
const hadesvm::core::ClockFrequency Fdc1Controller::DefaultClockFrequency = hadesvm::core::ClockFrequency::megahertz(1);

//////////
//  Construction/destruction
Fdc1Controller::Fdc1Controller()
    :   //  Configuration
        _stateAndControlPortAddress(DefaultStateAndControlPortAddress),
        _dataPortAddress(DefaultDataPortAddress),
        _interruptMaskPortAddress(DefaultInterruptMaskPortAddress),
        _clockFrequency(DefaultClockFrequency),
        //  Runtime state
        _runtimeStateGuard(),
        _ioPorts(),
        //  I/O ports
        _stateAndControlPort(this),
        _dataPort(this),
        _interruptMaskPort(this),
        //  Command completion handlers
        _getDriveStatusCompletionHandler(this),
        _readCompletionHandler(this),
        _writeCompletionHandler(this),
        _seekCompletionHandler(this)
{
    _ioPorts.append(&_stateAndControlPort);
    _ioPorts.append(&_dataPort);
    _ioPorts.append(&_interruptMaskPort);

    _floppyDrives[0] = _floppyDrives[1] = _floppyDrives[2] = _floppyDrives[3] = nullptr;
}

Fdc1Controller::~Fdc1Controller() noexcept
{
}

//////////
//  hadesvm::core::Component
QString Fdc1Controller::displayName() const
{
    return hadesvm::util::toString(_clockFrequency) +
           " " +
           Type::instance()->displayName() +
           " @ " +
           hadesvm::util::toString(_stateAndControlPortAddress, "%04X");
}

void Fdc1Controller::serialiseConfiguration(QDomElement componentElement) const
{
    componentElement.setAttribute("StateAndControlPortAddress", hadesvm::util::toString(_stateAndControlPortAddress, "%04X"));
    componentElement.setAttribute("DataPortAddress", hadesvm::util::toString(_dataPortAddress, "%04X"));
    componentElement.setAttribute("InterruptMaskPortAddress", hadesvm::util::toString(_interruptMaskPortAddress, "%04X"));
    componentElement.setAttribute("ClockFrequency", hadesvm::util::toString(_clockFrequency));
}

void Fdc1Controller::deserialiseConfiguration(QDomElement componentElement)
{
    uint16_t stateAndControlPortAddress = 0;
    if (hadesvm::util::fromString(componentElement.attribute("StateAndControlPortAddress"), "%X", stateAndControlPortAddress))
    {
        _stateAndControlPortAddress = stateAndControlPortAddress;
    }

    uint16_t dataPortAddress = 0;
    if (hadesvm::util::fromString(componentElement.attribute("DataPortAddress"), "%X", dataPortAddress))
    {
        _dataPortAddress = dataPortAddress;
    }

    uint16_t interruptMaskPortAddress = 0;
    if (hadesvm::util::fromString(componentElement.attribute("InterruptMaskPortAddress"), "%X", interruptMaskPortAddress))
    {
        _interruptMaskPortAddress = interruptMaskPortAddress;
    }

    hadesvm::core::ClockFrequency clockFrequency;
    if (hadesvm::util::fromString(componentElement.attribute("ClockFrequency"), clockFrequency))
    {   //  TODO and is >0
        _clockFrequency = clockFrequency;
    }
}

hadesvm::core::ComponentEditor * Fdc1Controller::createEditor()
{
    return new Fdc1ControllerEditor(this);
}

Fdc1Controller::Ui * Fdc1Controller::createUi()
{
    return nullptr;
}

//////////
//  hadesvm::core::Component (state management)
Fdc1Controller::State Fdc1Controller::state() const noexcept
{
    return _state;
}

void Fdc1Controller::connect() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Constructed)
    {   //  OOPS! Can't
        return;
    }

    //  Detect relevant floppy drives
    for (Fdc1FloppyDrive * floppyDrive : this->virtualAppliance()->componentsImplementing<Fdc1FloppyDrive>())
    {   //  TODO check the drive is destined for THIS controller
        if (/*floppyDrive->channel() < 0 ||*/ floppyDrive->channel() > 3 ||
            _floppyDrives[floppyDrive->channel()] != nullptr)
        {   //  OOPS! Cleanup & throw
            _floppyDrives[0] = _floppyDrives[1] = _floppyDrives[2] = _floppyDrives[3] = nullptr;
            throw hadesvm::core::VirtualApplianceException("Misconfigured floppy drive on channel " +
                                                           hadesvm::util::toString(floppyDrive->channel()));
        }
        _floppyDrives[floppyDrive->channel()] = floppyDrive;
    }

    _state = State::Connected;
}

void Fdc1Controller::initialize() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Initialized;
}

void Fdc1Controller::start() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Running;
}

void Fdc1Controller::stop() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Running)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Initialized;
}

void Fdc1Controller::deinitialize() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Connected;
}

void Fdc1Controller::disconnect() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    _floppyDrives[0] = _floppyDrives[1] = _floppyDrives[2] = _floppyDrives[3] = nullptr;

    _state = State::Constructed;
}

void Fdc1Controller::reset() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    _performReset();
}

//////////
//  hadesvm::core::IClockedComponent
void Fdc1Controller::onClockTick() noexcept
{
    QMutexLocker lock(&_runtimeStateGuard);

    //  Propagate I/O interrupts from controller to I/O port
    if (_pendingInterruptConditions != 0)
    {   //  There are pending interrupt conditions.
        if (IoInterrupt * pendingIoInterrupt = _stateAndControlPort.releasePendingIoInterrupt())
        {   //  ...which must be combined with already-pending ones
            _stateAndControlPort.setPendingIoInterrupt(static_cast<uint16_t>(_pendingInterruptConditions | pendingIoInterrupt->interruptStatusCode()));
            delete pendingIoInterrupt;
        }
        else
        {   //  ...which must result in a new I/O interrupt
            _stateAndControlPort.setPendingIoInterrupt(_pendingInterruptConditions);
        }
        _pendingInterruptConditions = 0;
    }

    //  Did the last issued FDD command just finish ?
    _AsyncResult * asyncResult = _asyncResult.exchange(nullptr);
    if (asyncResult != nullptr)
    {
        if (_GetDriveStatusAsyncResult * getDriveStatusAsyncResult =
            dynamic_cast<_GetDriveStatusAsyncResult*>(asyncResult))
        {
            _completeGetDriveStatusCommand(getDriveStatusAsyncResult->commandStatusByte,
                                           getDriveStatusAsyncResult->statusByte1);
        }
        else if (_ReadAsyncResult * readAsyncResult =
                 dynamic_cast<_ReadAsyncResult*>(asyncResult))
        {
            _completeReadCommand(readAsyncResult->commandStatusByte,
                                 readAsyncResult->dataBytes,
                                 readAsyncResult->numDataBytes);
        }
        else if (_WriteAsyncResult * writeAsyncResult =
                 dynamic_cast<_WriteAsyncResult *>(asyncResult))
        {
            _completeWriteCommand(writeAsyncResult->commandStatusByte);
        }
        else if (_SeekAsyncResult * seekAsyncResult =
                 dynamic_cast<_SeekAsyncResult*>(asyncResult))
        {
            _completeSeekCommand(seekAsyncResult->commandStatusByte);
        }
        else
        {   //  OOPS! Can't be!
            failure();
        }
        delete asyncResult;
        //  ...simulate BUSY flag going from 1 to 0 and
        //  INPUT_READY/OUTPUT_READY flags going from 0 to 1...
        _raiseBusyOffInterrupt();
        _raiseInputReadyOnInterrupt();
        _raiseOutputReadyOnInterrupt();
        //  ...and enter the "result" stage
        _operationalState = _OperationalState::_Result;
    }
}


//////////
//  IIoController
IoPortList Fdc1Controller::ioPorts()
{
    return _ioPorts;
}

//////////
//  Operations (configuration)
void Fdc1Controller::setStateAndControlPortAddress(uint16_t stateAndControlPortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    _stateAndControlPortAddress = stateAndControlPortAddress;
}

void Fdc1Controller::setDataPortAddress(uint16_t dataPortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    _dataPortAddress = dataPortAddress;
}

void Fdc1Controller::setInterruptMaskPortAddress(uint16_t interruptMaskPortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    _interruptMaskPortAddress = interruptMaskPortAddress;
}

void Fdc1Controller::setClockFrequency(const hadesvm::core::ClockFrequency & clockFrequency)
{
    Q_ASSERT(_state == State::Constructed);

    if (clockFrequency.toHz() > 0)
    {   //  TODO min/max
        _clockFrequency = clockFrequency;
    }
}

//////////
//  hadesvm::cereon::Fdc1Controller::Type
HADESVM_IMPLEMENT_SINGLETON(Fdc1Controller::Type)
Fdc1Controller::Type::Type() {}
Fdc1Controller::Type::~Type() {}

QString Fdc1Controller::Type::mnemonic() const
{
    return "CereonFdc1Controller";
}

QString Fdc1Controller::Type::displayName() const
{
    return "Cereon FDC1 controller";
}

hadesvm::core::ComponentCategory * Fdc1Controller::Type::category() const
{
    return hadesvm::core::StandardComponentCategories::IoControllers;
}

bool Fdc1Controller::Type::isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const
{
    return architecture == CereonWorkstationArchitecture::instance();
}

bool Fdc1Controller::Type::isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const
{
    return type == hadesvm::core::VirtualMachineType::instance();
}

Fdc1Controller * Fdc1Controller::Type::createComponent()
{
    return new Fdc1Controller();
}

//////////
//  Implementation helpers
void Fdc1Controller::_performReset()
{
    _interruptMask = 0;
    _currentDeviceIndex = 0;
    _operationalState = _OperationalState::_Idle;
    _numCommandBytes = 0;
    _numResultBytes = 0;
    _nextResultByte = 0;
    _pendingInterruptConditions = 0;

    _asyncResult = nullptr;

    delete _stateAndControlPort.releasePendingIoInterrupt();
}

unsigned Fdc1Controller::_getCommandLength(uint8_t commandByte)
{
    if ((commandByte & 0xFE) == 0x3E)
    {   //  Read track
        return 1;
    }
    else if ((commandByte & 0xC0) == 0x00)
    {   //  Read sector
        return 1;
    }
    else if ((commandByte & 0xFE) == 0x7E)
    {   //  Write track
        return 1 + 512 * 18;
    }
    else if ((commandByte & 0xC0) == 0x40)
    {   //  Write sector
        return 1 + 512;
    }
    else if ((commandByte & 0xE0) == 0xE0)
    {   //  A control command
        return 1;   //  ...currently all control command have no parameters
    }
    else if ((commandByte & 0x80) == 0x80)
    {   //  Seek
        return 1;
    }
    else
    {   //  Invalid
        return 1;
    }
}

void Fdc1Controller::_raiseBusyOffInterrupt()
{
    if ((_interruptMask & 0x01) != 0)
    {   //  The BUSY_OFF interrupt is enabled
        _pendingInterruptConditions |= 0x01;
    }
}

void Fdc1Controller::_raiseInputReadyOnInterrupt()
{
    if ((_interruptMask & 0x02) != 0)
    {   //  The INPUT_READY_ON interrupt is enabled
        _pendingInterruptConditions |= 0x02;
    }
}

void Fdc1Controller::_raiseOutputReadyOnInterrupt()
{
    if ((_interruptMask & 0x04) != 0)
    {   //  The OUTPUT_READY_ON interrupt is enabled
        _pendingInterruptConditions |= 0x04;
    }
}

void Fdc1Controller::_executeCommand()
{
    Q_ASSERT(_operationalState == _OperationalState::_ExecutingCommand);
    Q_ASSERT(_numCommandBytes > 0);
    Q_ASSERT(_getCommandLength(_commandBytes[0]) == _numCommandBytes);

    _numResultBytes = 0;
    _nextResultByte = 0;
    if (_commandBytes[0] == 0xE4)
    {   //  Get Drive Status
        _executeGetDriveStatusCommand();
    }
    else if ((_commandBytes[0] & 0xFE) == 0x3E)
    {   //  Read Track
        _executeReadTrackCommand();
    }
    else if ((_commandBytes[0] & 0xC0) == 0x00)
    {   //  Read Sector
        _executeReadSectorCommand();
    }
    else if ((_commandBytes[0] & 0xC0) == 0x40)
    {   //  Write Sector
        _executeWriteSectorCommand();
    }
    else if ((_commandBytes[0] & 0x80) == 0x80)
    {   //  Seek
        _executeSeekCommand();
    }
    //  TODO more commands
    else
    {   //  OOPS! Invalid command - prepare command status byte for "result" stage...
        _resultBytes[_numResultBytes++] = Status::InvalidCommand;
        //  ...simulate BUSY flag going from 1 to 0 and
        //  INPUT_READY/OUTPUT_READY flags going from 0 to 1...
        _raiseBusyOffInterrupt();
        _raiseInputReadyOnInterrupt();
        _raiseOutputReadyOnInterrupt();
        //  ...and enter the "result" stage
        _operationalState = _OperationalState::_Result;
    }
}

void Fdc1Controller::_executeGetDriveStatusCommand()
{
    if (_floppyDrives[_currentDeviceIndex] == nullptr)
    {   //  OOPS! No "current" drive!
        _resultBytes[_numResultBytes++] = Status::NotReady;
        //  ...simulate BUSY flag going from 1 to 0 and
        //  INPUT_READY/OUTPUT_READY flags going from 0 to 1...
        _raiseBusyOffInterrupt();
        _raiseInputReadyOnInterrupt();
        _raiseOutputReadyOnInterrupt();
        //  ...and enter the "result" stage
        _operationalState = _OperationalState::_Result;
        return;
    }

    //  Go!
    _floppyDrives[_currentDeviceIndex]->beginGetDriveStatus(&_getDriveStatusCompletionHandler);
}

void Fdc1Controller::_executeReadSectorCommand()
{
    if (_floppyDrives[_currentDeviceIndex] == nullptr)
    {   //  OOPS! No "current" drive!
        _resultBytes[_numResultBytes++] = Status::NotReady;
        //  ...simulate BUSY flag going from 1 to 0 and
        //  INPUT_READY/OUTPUT_READY flags going from 0 to 1...
        _raiseBusyOffInterrupt();
        _raiseInputReadyOnInterrupt();
        _raiseOutputReadyOnInterrupt();
        //  ...and enter the "result" stage
        _operationalState = _OperationalState::_Result;
        return;
    }

    //  Go!
    _floppyDrives[_currentDeviceIndex]->beginRead(static_cast<unsigned>(_commandBytes[0] & 0x01),
                                                  static_cast<unsigned>((_commandBytes[0] >> 1) & 0x1F),
                                                  1,
                                                  &_readCompletionHandler);
}

void Fdc1Controller::_executeReadTrackCommand()
{
    if (_floppyDrives[_currentDeviceIndex] == nullptr)
    {   //  OOPS! No "current" drive!
        _resultBytes[_numResultBytes++] = Status::NotReady;
        //  ...simulate BUSY flag going from 1 to 0 and
        //  INPUT_READY/OUTPUT_READY flags going from 0 to 1...
        _raiseBusyOffInterrupt();
        _raiseInputReadyOnInterrupt();
        _raiseOutputReadyOnInterrupt();
        //  ...and enter the "result" stage
        _operationalState = _OperationalState::_Result;
        return;
    }

    //  Go!
    _floppyDrives[_currentDeviceIndex]->beginRead(static_cast<unsigned>(_commandBytes[0] & 0x01),
                                                  0,
                                                  18,
                                                  &_readCompletionHandler);
}

void Fdc1Controller::_executeWriteSectorCommand()
{
    if (_floppyDrives[_currentDeviceIndex] == nullptr)
    {   //  OOPS! No "current" drive!
        _resultBytes[_numResultBytes++] = Status::NotReady;
        //  ...simulate BUSY flag going from 1 to 0 and
        //  INPUT_READY/OUTPUT_READY flags going from 0 to 1...
        _raiseBusyOffInterrupt();
        _raiseInputReadyOnInterrupt();
        _raiseOutputReadyOnInterrupt();
        //  ...and enter the "result" stage
        _operationalState = _OperationalState::_Result;
        return;
    }

    //  Go!
    _floppyDrives[_currentDeviceIndex]->beginWrite(static_cast<unsigned>(_commandBytes[0] & 0x01),
                                                   static_cast<unsigned>((_commandBytes[0] >> 1) & 0x1F),
                                                   1,
                                                   _commandBytes + 1,
                                                   &_writeCompletionHandler);
}

void Fdc1Controller::_executeSeekCommand()
{
    if (_floppyDrives[_currentDeviceIndex] == nullptr)
    {   //  OOPS! No "current" drive!
        _resultBytes[_numResultBytes++] = Status::NotReady;
        //  ...simulate BUSY flag going from 1 to 0 and
        //  INPUT_READY/OUTPUT_READY flags going from 0 to 1...
        _raiseBusyOffInterrupt();
        _raiseInputReadyOnInterrupt();
        _raiseOutputReadyOnInterrupt();
        //  ...and enter the "result" stage
        _operationalState = _OperationalState::_Result;
        return;
    }

    //  Go!
    _floppyDrives[_currentDeviceIndex]->beginSeek(static_cast<unsigned>(_commandBytes[0] & 0x7F),
                                                  &_seekCompletionHandler);
}

void Fdc1Controller::_completeGetDriveStatusCommand(uint8_t commandStatusByte, uint8_t statusByte1)
{
    Q_ASSERT(_numResultBytes == 0);
    Q_ASSERT(_nextResultByte == 0);

    _resultBytes[_numResultBytes++] = commandStatusByte;
    _resultBytes[_numResultBytes++] = statusByte1;
}

void Fdc1Controller::_completeReadCommand(uint8_t commandStatusByte, const uint8_t * dataBytes, unsigned numDataBytes)
{
    Q_ASSERT(_numResultBytes == 0);
    Q_ASSERT(_nextResultByte == 0);

    _resultBytes[_numResultBytes++] = commandStatusByte;
    for (unsigned i = 0; i < numDataBytes; i++)
    {
        _resultBytes[_numResultBytes++] = dataBytes[i];
    }
}

void Fdc1Controller::_completeWriteCommand(uint8_t commandStatusByte)
{
    Q_ASSERT(_numResultBytes == 0);
    Q_ASSERT(_nextResultByte == 0);

    _resultBytes[_numResultBytes++] = commandStatusByte;
}

void Fdc1Controller::_completeSeekCommand(uint8_t commandStatusByte)
{
    Q_ASSERT(_numResultBytes == 0);
    Q_ASSERT(_nextResultByte == 0);

    _resultBytes[_numResultBytes++] = commandStatusByte;
}

//////////
//  Fdc1Controller::_StateAndControlPort
uint8_t Fdc1Controller::_StateAndControlPort::readByte()
{
    QMutexLocker lock(&_fdc1Controller->_runtimeStateGuard);

    //  Reads STATE
    uint8_t result = 0x80;   //  controller is present
    switch (_fdc1Controller->_operationalState)
    {
        case _OperationalState::_Idle:
            result |= 0x20;  //  INPUT_READY
            break;
        case _OperationalState::_AcceptingCommand:
            result |= 0x20;  //  INPUT_READY
            break;
        case _OperationalState::_ExecutingCommand:
            result |= 0x40;  //  BUSY
            break;
        case _OperationalState::_Result:
            result |= 0x30;  //  INPUT_READY + OUTPUT_READY
            break;
        default:
            failure();
    }
    for (int i = 0; i < 4; i++)
    {
        if (_fdc1Controller->_floppyDrives[i] != nullptr &&
            _fdc1Controller->_floppyDrives[i]->getMotorStatus() == Fdc1FloppyDrive::MotorStatus::Spinning)
        {
            result |= (0x01 << i);   //  MOTn
        }
    }
    return result;
}

void Fdc1Controller::_StateAndControlPort::writeByte(uint8_t value)
{
    QMutexLocker lock(&_fdc1Controller->_runtimeStateGuard);
    //  Writes CONTROL

    //  RESET ?
    if ((value & 0x80) != 0)
    {   //  Yes, all other bits are ignored
        _fdc1Controller->_performReset();
        return;
    }

    //  DR1,DR0
    _fdc1Controller->_currentDeviceIndex = static_cast<unsigned>((value >> 4) & 0x03);

    //  MOTn
    for (int i = 0; i < 4; i++)
    {
        if (_fdc1Controller->_floppyDrives[i] != nullptr)
        {
            Fdc1FloppyDrive::MotorStatus motorStatus = _fdc1Controller->_floppyDrives[i]->getMotorStatus();
            if ((value & (0x01 << i)) != 0)
            {   //  Turn motor on TODO properly
                if (motorStatus != Fdc1FloppyDrive::MotorStatus::Spinning &&
                    motorStatus != Fdc1FloppyDrive::MotorStatus::Starting)
                {
                    _fdc1Controller->_floppyDrives[i]->beginStartMotor(nullptr);
                }
            }
            else
            {   //  Turn motor off TODO properly
                if (motorStatus != Fdc1FloppyDrive::MotorStatus::Stopped &&
                    motorStatus != Fdc1FloppyDrive::MotorStatus::Stopping)
                {
                    _fdc1Controller->_floppyDrives[i]->beginStopMotor(nullptr);
                }
            }
        }
    }
}

//////////
//  Fdc1Controller::_DataPort
uint8_t Fdc1Controller::_DataPort::readByte()
{
    QMutexLocker lock(&_fdc1Controller->_runtimeStateGuard);

    if (_fdc1Controller->_operationalState != _OperationalState::_Result)
    {   //  We're not in "result" stage
        return 0;
    }

    //  Provide the next result byte
    Q_ASSERT(_fdc1Controller->_nextResultByte < _fdc1Controller->_numResultBytes);
    uint8_t result = _fdc1Controller->_resultBytes[_fdc1Controller->_nextResultByte++];

    //  Effect state change
    if (_fdc1Controller->_nextResultByte == _fdc1Controller->_numResultBytes)
    {   //  That was the last result byte - switch to "idle" state.
        //  Note that INPUT_READY was 1 during the "result" stage, so it
        //  remains "1" for the "idle" state
        _fdc1Controller->_operationalState = _OperationalState::_Idle;
    }
    else
    {   //  There will be more result bytes - simulate the OUTPUT_READY
        //  bit changing from 1 to 0 and then back to 1
        _fdc1Controller->_raiseOutputReadyOnInterrupt();
    }
    return result;
}

void Fdc1Controller::_DataPort::writeByte(uint8_t value)
{
    QMutexLocker lock(&_fdc1Controller->_runtimeStateGuard);

    if (_fdc1Controller->_operationalState == _OperationalState::_Idle)
    {   //  Accept the 1st byte of the command
        _fdc1Controller->_numCommandBytes = 0;
        _fdc1Controller->_commandBytes[_fdc1Controller->_numCommandBytes++] = value;
    }
    else if (_fdc1Controller->_operationalState == _OperationalState::_AcceptingCommand)
    {   //  Accept the next byte of the command
        _fdc1Controller->_commandBytes[_fdc1Controller->_numCommandBytes++] = value;
    }
    else if (_fdc1Controller->_operationalState == _OperationalState::_Result)
    {   //  Abort the result phase and start accepting a new command
        _fdc1Controller->_numCommandBytes = 0;
        _fdc1Controller->_commandBytes[_fdc1Controller->_numCommandBytes++] = value;
    }
    else
    {   //  Ignore the write
        return;
    }

    //  Do we have enough command bytes to start executing the command ?
    Q_ASSERT(_fdc1Controller->_numCommandBytes > 0 &&
             _fdc1Controller->_numCommandBytes <= _getCommandLength(_fdc1Controller->_commandBytes[0]));
    if (_fdc1Controller->_numCommandBytes == _getCommandLength(_fdc1Controller->_commandBytes[0]))
    {   //  Yes. Switch to "executing command" stage...
        _fdc1Controller->_operationalState = _OperationalState::_ExecutingCommand;
        //  ...and execute
        _fdc1Controller->_executeCommand();
    }
    else
    {   //  No - need more parameter bytes. Simulate the INPUT_READY
        //  bit changing from 1 to 0 and then back to 1...
        _fdc1Controller->_raiseInputReadyOnInterrupt();
        //  ...and continue in "waiting for more input" stage
        _fdc1Controller->_operationalState = _OperationalState::_AcceptingCommand;
    }

}

//////////
//  Fdc1Controller::_InterruptMaskPort
uint8_t Fdc1Controller::_InterruptMaskPort::readByte()
{
    QMutexLocker lock(&_fdc1Controller->_runtimeStateGuard);

    return _fdc1Controller->_interruptMask;
}

void Fdc1Controller::_InterruptMaskPort::writeByte(uint8_t value)
{
    QMutexLocker lock(&_fdc1Controller->_runtimeStateGuard);

    _fdc1Controller->_interruptMask = value;
}

//////////
//  Fdc1Controller::_GetDriveStatusCompletionHandler
void Fdc1Controller::_GetDriveStatusCompletionHandler::onOperationCompleted(
    Fdc1FloppyDrive * /*floppyDrive*/, uint8_t commandStatusByte,
    uint8_t statusByte1)
{
    _fdc1Controller->_asyncResult = new _GetDriveStatusAsyncResult(commandStatusByte, statusByte1);
}

//////////
//  Fdc1Controller::_ReadCompletionHandler
void Fdc1Controller::_ReadCompletionHandler::onOperationCompleted(
    Fdc1FloppyDrive * /*floppyDrive*/, uint8_t commandStatusByte,
    const uint8_t * dataBytes, unsigned numDataBytes)
{
    _fdc1Controller->_asyncResult = new _ReadAsyncResult(commandStatusByte, dataBytes, numDataBytes);
}

//////////
//  Fdc1Controller::_WriteCompletionHandler
void Fdc1Controller::_WriteCompletionHandler::onOperationCompleted(
    Fdc1FloppyDrive * /*floppyDrive*/, uint8_t commandStatusByte)
{
    _fdc1Controller->_asyncResult = new _WriteAsyncResult(commandStatusByte);
}

//////////
//  Fdc1Controller::_SeekCompletionHandler
void Fdc1Controller::_SeekCompletionHandler::onOperationCompleted(
    Fdc1FloppyDrive * /*floppyDrive*/, uint8_t commandStatusByte)
{
    _fdc1Controller->_asyncResult = new _SeekAsyncResult(commandStatusByte);
}

//  End of hadesvm-cereon/Fdc1Controller.cpp
