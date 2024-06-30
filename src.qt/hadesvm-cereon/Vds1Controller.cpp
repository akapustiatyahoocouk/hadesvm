//
//  hadesvm-cereon/Vds1Controller.cpp
//
//  hadesvm::cereon::Vds1Controller class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Constants
const uint16_t Vds1Controller::DefaultStatePortAddress = 0x0200;
const uint16_t Vds1Controller::DefaultCommandPortAddress = 0x0201;
const uint16_t Vds1Controller::DefaultDataPortAddress = 0x0202;
const unsigned Vds1Controller::DefaultNumberOfCompartments = 1;
const hadesvm::core::ClockFrequency Vds1Controller::DefaultClockFrequency = hadesvm::core::ClockFrequency::megahertz(2);

//////////
//  Construction/destruction
Vds1Controller::Vds1Controller()
    :   //  Configuration
        _statePortAddress(DefaultStatePortAddress),
        _commandPortAddress(DefaultCommandPortAddress),
        _dataPortAddress(DefaultDataPortAddress),
        _numberOfCompartments(DefaultNumberOfCompartments),
        _clockFrequency(DefaultClockFrequency),
        //  Runtime state
        _runtimeStateGuard(),
        _ioPorts(),
        _interruptMask(0),
        _allCompartments(),             //  initialized by initialize()
        _currentCompartment(nullptr),    //  initialized by initialize()
        _commandBytes(),
        _resultBytes(),
        //  I/O ports
        _statePort(this),
        _commandPort(this),
        _dataPort(this)
{
    _ioPorts.append(&_statePort);
    _ioPorts.append(&_commandPort);
    _ioPorts.append(&_dataPort);
}

Vds1Controller::~Vds1Controller() noexcept
{
}

//////////
//  hadesvm::core::Component
QString Vds1Controller::displayName() const
{
    return hadesvm::util::toString(_clockFrequency) +
           " " +
           Type::instance()->displayName() +
           " @ " +
           hadesvm::util::toString(_statePortAddress, "%04X");
}

void Vds1Controller::serialiseConfiguration(QDomElement componentElement) const
{
    componentElement.setAttribute("StatePortAddress", hadesvm::util::toString(_statePortAddress, "%04X"));
    componentElement.setAttribute("CommandPortAddress", hadesvm::util::toString(_commandPortAddress, "%04X"));
    componentElement.setAttribute("DataPortAddress", hadesvm::util::toString(_dataPortAddress, "%04X"));
    componentElement.setAttribute("NumberOfCompartments", hadesvm::util::toString(_numberOfCompartments));
    componentElement.setAttribute("ClockFrequency", hadesvm::util::toString(_clockFrequency));
}

void Vds1Controller::deserialiseConfiguration(QDomElement componentElement)
{
    uint16_t statePortAddress = 0;
    if (hadesvm::util::fromString(componentElement.attribute("StatePortAddress"), "%X", statePortAddress))
    {
        _statePortAddress = statePortAddress;
    }

    uint16_t commandPortAddress = 0;
    if (hadesvm::util::fromString(componentElement.attribute("CommandPortAddress"), "%X", commandPortAddress))
    {
        _commandPortAddress = commandPortAddress;
    }

    uint16_t dataPortAddress = 0;
    if (hadesvm::util::fromString(componentElement.attribute("DataPortAddress"), "%X", dataPortAddress))
    {
        _dataPortAddress = dataPortAddress;
    }

    unsigned numberOfCompartments = 0;
    if (hadesvm::util::fromString(componentElement.attribute("NumberOfCompartments"), numberOfCompartments) &&
        numberOfCompartments >= 1 && numberOfCompartments <= 256)
    {
        _numberOfCompartments = numberOfCompartments;
    }

    hadesvm::core::ClockFrequency clockFrequency;
    if (hadesvm::util::fromString(componentElement.attribute("ClockFrequency"), clockFrequency))
    {   //  TODO and is >0
        _clockFrequency = clockFrequency;
    }
}

hadesvm::core::ComponentEditor * Vds1Controller::createEditor(QWidget * parent)
{
    return new Vds1ControllerEditor(parent, this);
}

Vds1Controller::Ui * Vds1Controller::createUi()
{
    return nullptr;
}

//////////
//  Operations (component configuration)
void Vds1Controller::setStatePortAddress(uint16_t statePortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    //  TODO validate "statePortAddress"
    _statePortAddress = statePortAddress;
}

void Vds1Controller::setCommandPortAddress(uint16_t commandPortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    //  TODO validate "commandPortAddress"
    _commandPortAddress = commandPortAddress;
}

void Vds1Controller::setDataPortAddress(uint16_t dataPortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    //  TODO validate "dataPortAddress"
    _dataPortAddress = dataPortAddress;
}

void Vds1Controller::setNumberOfCompartments(unsigned numberOfCompartments)
{
    Q_ASSERT(_state == State::Constructed);

    if (numberOfCompartments >= 1 && numberOfCompartments <= 256)
    {
        _numberOfCompartments = numberOfCompartments;
    }
}

void Vds1Controller::setClockFrequency(const hadesvm::core::ClockFrequency & clockFrequency)
{
    Q_ASSERT(_state == State::Constructed);

    if (clockFrequency.toHz() > 0)  //  TODO and upper limit too ?
    {
        _clockFrequency = clockFrequency;
    }
}

//////////
//  hadesvm::core::IClockedComponent
void Vds1Controller::onClockTick() noexcept
{
    QMutexLocker lock(&_runtimeStateGuard);

    if (_operationalState == _OperationalState::_ExecutingCommand && _executeDelay > 0)
    {
        if (--_executeDelay == 0)
        {   //  Done executing a long command
            _operationalState = _resultBytes.isEmpty() ?
                                    _OperationalState::_Ready :
                                    _OperationalState::_ProvidingResult;
        }
    }
}

//////////
//  IIoController
IoPortList Vds1Controller::ioPorts()
{
    return _ioPorts;
}

//////////
//  hadesvm::core::Component (state management)
Vds1Controller::State Vds1Controller::state() const noexcept
{
    return _state;
}

void Vds1Controller::connect() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Constructed)
    {   //  OOPS! Can't
        return;
    }

    //  Create compartments
    Q_ASSERT(_allCompartments.isEmpty());
    for (unsigned i = 0; i < _numberOfCompartments; i++)
    {
        _allCompartments.append(new _Compartment(static_cast<uint8_t>(i)));
    }
    _currentCompartment = _allCompartments[0];

    //  Locate all "Vds1Display" components & connect to this controller
    try
    {
        for (Vds1Display * vds1Display : virtualAppliance()->componentsImplementing<Vds1Display>())
        {   //  We have found a "Vds1Display"...
            if (vds1Display->controllerStatePortAddress() == _statePortAddress)
            {   //  ...which should be attached to this controller
                if (vds1Display->controllerCompartmentNumber() >= _allCompartments.count())
                {   //  OOPS!
                    throw hadesvm::core::VirtualApplianceException("TODO proper error message");
                }
                _Compartment * compartment = _allCompartments[vds1Display->controllerCompartmentNumber()];
                if (compartment->attachedDisplay() != nullptr)
                {   //  OOPS!
                    throw hadesvm::core::VirtualApplianceException("TODO proper error message");
                }
                compartment->attachDisplay(vds1Display);
            }
        }
    }
    catch (...)
    {   //  OOPS! Cleanup & re-throw
        for (auto compartment : _allCompartments)
        {
            compartment->detachDisplay();
            delete compartment;
        }
        _allCompartments.clear();
        throw;
    }

    _state = State::Connected;
}

void Vds1Controller::initialize() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Initialized;
}

void Vds1Controller::start() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Running;
}

void Vds1Controller::stop() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Running)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Initialized;
}

void Vds1Controller::deinitialize() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Connected;
}

void Vds1Controller::disconnect() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    _currentCompartment = nullptr;
    for (_Compartment * compartment : _allCompartments)
    {
        compartment->detachDisplay();
        delete compartment;
    }
    _allCompartments.clear();

    _state = State::Constructed;
}

//////////
//  hadesvm::cereon::Vds1Controller::Type
HADESVM_IMPLEMENT_SINGLETON(Vds1Controller::Type)
Vds1Controller::Type::Type() {}
Vds1Controller::Type::~Type() {}

QString Vds1Controller::Type::mnemonic() const
{
    return "CereonVds1Controller";
}

QString Vds1Controller::Type::displayName() const
{
    return "Cereon VDS1 controller";
}

hadesvm::core::ComponentCategory * Vds1Controller::Type::category() const
{
    return hadesvm::core::StandardComponentCategories::IoControllers;
}

bool Vds1Controller::Type::suspendable() const
{
    return true;
}

bool Vds1Controller::Type::isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const
{
    return architecture == CereonWorkstationArchitecture::instance();
}

bool Vds1Controller::Type::isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const
{
    return type == hadesvm::core::VirtualMachineType::instance();
}

Vds1Controller * Vds1Controller::Type::createComponent()
{
    return new Vds1Controller();
}

//////////
//  Implementation helpers
unsigned Vds1Controller::_getCommandLength(uint8_t commandByte)
{
    switch (commandByte)
    {
        case _ResetCommand:
            return 1;   //  <command>
        case _ResetCompartmentCommand:
            return 1;   //  <command>
        case _GetCapacityCommand:
            return 1;   //  <command>
        case _GetInterruptMaskCommand:
            return 1;   //  <command>
        case _SetInterruptMaskCommand:
            return 2;   //  <command> <controllers interrupt mask>
        case _SenseDisplayCommand:
            return 2;   //  <command> <compartment number>
        case _GetCurrentCompartmentCommand:
            return 1;   //  <command>
        case _SetCurrentCompartmentCommand:
            return 2;   //  <command> <new current compartment>
        case _GetVideoModeCommand:
            return 1;   //  <command>
        case _SetVideoModeCommand:
            return 2;   //  <command> <new video mode for the current compartment>
        case _GetVideoPageCommand:
            return 1;   //  <command>
        case _SetVideoPageCommand:
            return 2;   //  <command> <new video page for the current compartment>
        case _GetSourceAddressCommand:
            return 1;   //  <command>
        case _SetSourceAddressCommand:
            return 3;   //  <command> <high byte of the source address> <low byte of the source address>
        case _GetDestinationAddressCommand:
            return 1;   //  <command>
        case _SetDestinationAddressCommand:
            return 3;   //  <command> <high byte of the destination address> <low byte of the destination address>
        case _GetLengthCommand:
            return 1;   //  <command>
        case _SetLengthCommand:
            return 3;   //  <command> <new high byte of the length> <new low byte of the length>
        case _ReadCommand:
            return 1;   //  <command>
        case _ReadAndAdvanceCommand:
            return 1;   //  <command>
        case _WriteCommand:
            return 2;   //  <command> <byte to write to the video memory>
        case _WriteAndAdvanceCommand:
            return 2;   //  <command> <byte to write to the video memory>
        case _WriteWithMaskCommand:
            return 3;   //  <command> <mask> <byte to write to the video memory>
        case _WriteWithMaskAndAdvanceCommand:
            return 3;   //  <command> <mask> <byte to write to the video memory>
        case _FillCommand:
            return 2;   //  <command> <byte to fill the video memory with>
        case _FillWithMaskCommand:
            return 3;   //  <command> <mask> <byte to fill the video memory with>
        case _MoveCommand:
            return 1;   //  <command>
        case _MoveBackwardCommand:
            return 1;   //  <command>
        case _MoveWithMaskCommand:
            return 2;   //  <command> <mask>
        case _MoveBackwardWithMaskCommand:
            return 2;   //  <command> <mask>
        default:
            return 0;
    }
}

void Vds1Controller::_executeCommand()
{
    Q_ASSERT(!_commandBytes.isEmpty());
    Q_ASSERT(_commandBytes.size() == _getCommandLength(_commandBytes[0]));
    Q_ASSERT(_operationalState == _OperationalState::_AcceptingCommand ||
             _operationalState == _OperationalState::_Ready);

    _operationalState = _OperationalState::_ExecutingCommand;
    _executeDelay = 0;
    switch (_commandBytes[0])
    {
        case _ResetCommand:
            _executeResetCommand();
            break;
        case _ResetCompartmentCommand:
            _executeResetCompartmentCommand();
            break;
        case _GetCapacityCommand:
            _executeGetCapacityCommand();
            break;
        case _GetInterruptMaskCommand:
            _executeGetInterruptMaskCommand();
            break;
        case _SetInterruptMaskCommand:
            _executeSetInterruptMaskCommand();
            break;
        case _SenseDisplayCommand:
            _executeSenseDisplayCommand();
            break;
        case _GetCurrentCompartmentCommand:
            _executeGetCurrentCompartmentCommand();
            break;
        case _SetCurrentCompartmentCommand:
            _executeSetCurrentCompartmentCommand();
            break;
        case _GetVideoModeCommand:
            _executeGetVideoModeCommand();
            break;
        case _SetVideoModeCommand:
            _executeSetVideoModeCommand();
            break;
        case _GetVideoPageCommand:
            _executeGetVideoPageCommand();
            break;
        case _SetVideoPageCommand:
            _executeSetVideoPageCommand();
            break;
        case _GetSourceAddressCommand:
            _executeGetSourceAddressCommand();
            break;
        case _SetSourceAddressCommand:
            _executeSetSourceAddressCommand();
            break;
        case _GetDestinationAddressCommand:
            _executeGetDestinationAddressCommand();
            break;
        case _SetDestinationAddressCommand:
            _executeSetDestinationAddressCommand();
            break;
        case _GetLengthCommand:
            _executeGetLengthCommand();
            break;
        case _SetLengthCommand:
            _executeSetLengthCommand();
            break;
        case _ReadCommand:
            _executeReadCommand();
            break;
        case _ReadAndAdvanceCommand:
            _executeReadAndAdvanceCommand();
            break;
        case _WriteCommand:
            _executeWriteCommand();
            break;
        case _WriteAndAdvanceCommand:
            _executeWriteAndAdvanceCommand();
            break;
        case _WriteWithMaskCommand:
            _executeWriteWithMaskCommand();
            break;
        case _WriteWithMaskAndAdvanceCommand:
            _executeWriteWithMaskAndAdvanceCommand();
            break;
        case _FillCommand:
            _executeFillCommand();
            break;
        case _FillWithMaskCommand:
            _executeFillWithMaskCommand();
            break;
        case _MoveCommand:
            _executeMoveCommand();
            break;
        case _MoveBackwardCommand:
            _executeMoveBackwardCommand();
            break;
        case _MoveWithMaskCommand:
            _executeMoveWithMaskCommand();
            break;
        case _MoveBackwardWithMaskCommand:
            _executeMoveBackwardWithMaskCommand();
            break;
        default:
            failure();
    }
}

void Vds1Controller::_executeResetCommand()
{
    //  TODO uncomment reset();
    //  Simulate command execution delay
    _executeDelay = static_cast<unsigned>(65536 * _allCompartments.size());    //  1 clock cycle per 1 byte of video memory
}

void Vds1Controller::_executeResetCompartmentCommand()
{
    _currentCompartment->reset();
    //  Simulate command execution delay
    _executeDelay = 65536;  //  1 clock cycle per 1 byte of video memory
}

void Vds1Controller::_executeGetCapacityCommand()
{
    _finishCommandExecution(static_cast<uint8_t>(_allCompartments.size()));
}

void Vds1Controller::_executeGetInterruptMaskCommand()
{
    _finishCommandExecution(_interruptMask);
}

void Vds1Controller::_executeSetInterruptMaskCommand()
{
    Q_ASSERT(_commandBytes[1] == 0); //  interrupts not yet implemented
    _finishCommandExecution();
}

void Vds1Controller::_executeSenseDisplayCommand()
{
    uint8_t result = 0;

    Vds1Display * display = _currentCompartment->attachedDisplay();
    if (display != nullptr)
    {
        result |= 0x01; //  display is connected
        //  TODO if...
        result |= 0x02; //  display is powered on
    }
    _finishCommandExecution(result);
}

void Vds1Controller::_executeGetCurrentCompartmentCommand()
{
    _finishCommandExecution(_currentCompartment->number());
}

void Vds1Controller::_executeSetCurrentCompartmentCommand()
{
    if (_commandBytes[1] < _allCompartments.size())
    {
        _currentCompartment = _allCompartments[_commandBytes[1]];
    }
    _finishCommandExecution();
}

void Vds1Controller::_executeGetVideoModeCommand()
{
    _finishCommandExecution(_currentCompartment->videoMode());
}

void Vds1Controller::_executeSetVideoModeCommand()
{
    _currentCompartment->setVideoMode(_commandBytes[1]);
    _finishCommandExecution();
}

void Vds1Controller::_executeGetVideoPageCommand()
{
    failure_with_message("Not yet implemented");
}

void Vds1Controller::_executeSetVideoPageCommand()
{
    failure_with_message("Not yet implemented");
}

void Vds1Controller::_executeGetSourceAddressCommand()
{
    uint16_t source = _currentCompartment->source();
    _finishCommandExecution(static_cast<uint8_t>(source >> 8), static_cast<uint8_t>(source));
}

void Vds1Controller::_executeSetSourceAddressCommand()
{
    _currentCompartment->setSource(static_cast<uint16_t>((_commandBytes[1] << 8) | _commandBytes[2]));
    _finishCommandExecution();
}

void Vds1Controller::_executeGetDestinationAddressCommand()
{
    uint16_t destination = _currentCompartment->destination();
    _finishCommandExecution(static_cast<uint8_t>(destination >> 8), static_cast<uint8_t>(destination));
}

void Vds1Controller::_executeSetDestinationAddressCommand()
{
    _currentCompartment->setDestination(static_cast<uint16_t>((_commandBytes[1] << 8) | _commandBytes[2]));
    _finishCommandExecution();
}

void Vds1Controller::_executeGetLengthCommand()
{
    failure_with_message("Not yet implemented");
}

void Vds1Controller::_executeSetLengthCommand()
{
    _currentCompartment->setLength(static_cast<uint16_t>((_commandBytes[1] << 8) | _commandBytes[2]));
    _finishCommandExecution();
}

void Vds1Controller::_executeReadCommand()
{
    _finishCommandExecution(_currentCompartment->read());
}

void Vds1Controller::_executeReadAndAdvanceCommand()
{
    failure_with_message("Not yet implemented");
}

void Vds1Controller::_executeWriteCommand()
{
    _currentCompartment->write(_commandBytes[1]);
    _finishCommandExecution();
}

void Vds1Controller::_executeWriteAndAdvanceCommand()
{
    _currentCompartment->writeAndAdvance(_commandBytes[1]);
    _finishCommandExecution();
}

void Vds1Controller::_executeWriteWithMaskCommand()
{
    failure_with_message("Not yet implemented");
}

void Vds1Controller::_executeWriteWithMaskAndAdvanceCommand()
{
    failure_with_message("Not yet implemented");
}

void Vds1Controller::_executeFillCommand()
{
    failure_with_message("Not yet implemented");
}

void Vds1Controller::_executeFillWithMaskCommand()
{
    failure_with_message("Not yet implemented");
}

void Vds1Controller::_executeMoveCommand()
{
    _currentCompartment->move();
    //  Simulate command execution delay
    _executeDelay = static_cast<unsigned>(_currentCompartment->length());
}

void Vds1Controller::_executeMoveBackwardCommand()
{
    failure_with_message("Not yet implemented");
}

void Vds1Controller::_executeMoveWithMaskCommand()
{
    failure_with_message("Not yet implemented");
}

void Vds1Controller::_executeMoveBackwardWithMaskCommand()
{
    failure_with_message("Not yet implemented");
}

void Vds1Controller::_finishCommandExecution()
{
    _resultBytes.clear();
    _operationalState = _OperationalState::_Ready;
}

void Vds1Controller::_finishCommandExecution(uint8_t resultByte1)
{
    _resultBytes.clear();
    _resultBytes.append(resultByte1);
    _operationalState = _OperationalState::_ProvidingResult;
}

void Vds1Controller::_finishCommandExecution(uint8_t resultByte1, uint8_t resultByte2)
{
    _resultBytes.clear();
    _resultBytes.append(resultByte1);
    _resultBytes.append(resultByte2);
    _operationalState = _OperationalState::_ProvidingResult;
}

//////////
//  Vds1Controller::_StatePort
bool Vds1Controller::_StatePort::readByte(uint8_t & value)
{
    QMutexLocker lock(&_vds1Controller->_runtimeStateGuard);

    value = 0x01;
    if (_vds1Controller->_operationalState == _OperationalState::_Ready)
    {
        value |= 0x02; //  ready for COMMAND port write
    }
    if (_vds1Controller->_operationalState == _OperationalState::_AcceptingCommand)
    {   //  Waiting for command parameters OR command pre-emption
        value |= 0x0A;
    }
    if (_vds1Controller->_operationalState == _OperationalState::_ProvidingResult)
    {   //  Result byte(s) ready to be read OR next command issued
        value |= 0x06;
    }
    if (_vds1Controller->_operationalState == _OperationalState::_ExecutingCommand)
    {
        value |= 0x10; //  command in progress
    }
    return true;
}

bool Vds1Controller::_StatePort::writeByte(uint8_t /*value*/)
{   //  Writes to STATE port are ignored
    return true;
}

//////////
//  Vds1Controller::_CommandPort
bool Vds1Controller::_CommandPort::readByte(uint8_t & value)
{
    value = 0;   //  COMMAND is a write-only port
    return true;
}

bool Vds1Controller::_CommandPort::writeByte(uint8_t value)
{
    QMutexLocker lock(&_vds1Controller->_runtimeStateGuard);
    unsigned commandLength;

    switch (_vds1Controller->_operationalState)
    {
        case _OperationalState::_AcceptingCommand: //  preempting the previous command
        case _OperationalState::_ProvidingResult:  //  dropping the result from previous command & starting a new one
        case _OperationalState::_Ready:    //  waiting for COMMAND byte
            commandLength = _getCommandLength(value);
            if (commandLength == 0)
            {   //  Invalid COMMAND byte - no effect
                return true;
            }
            //  Store the 1st byte of the command
            _vds1Controller->_commandBytes.clear();
            _vds1Controller->_commandBytes.append(value);
            //  More needed ?
            if (commandLength > 1)
            {   //  Yes - await command parameters
                _vds1Controller->_operationalState = _OperationalState::_AcceptingCommand;
                return true;
            }
            //  No parameters needed - execute the command
            _vds1Controller->_executeCommand();  //  ...this switches _state
            break;
        case _OperationalState::_ExecutingCommand:  //  Suppress write
            break;
        default:
            failure();
    }
    return true;
}

//////////
//  Vds1Controller::_DataPort
bool Vds1Controller::_DataPort::readByte(uint8_t & value)
{
    QMutexLocker lock(&_vds1Controller->_runtimeStateGuard);

    if (_vds1Controller->_operationalState == _OperationalState::_ProvidingResult)
    {   //  CPU reads command results
        value = _vds1Controller->_resultBytes[0];
        _vds1Controller->_resultBytes.removeAt(0);
        if (_vds1Controller->_resultBytes.isEmpty())
        {   //  No more results
            _vds1Controller->_operationalState = _OperationalState::_Ready;
        }
        return true;
    }
    //  else no result bytes to send to CPU
    value = 0;
    return true;
}

bool Vds1Controller::_DataPort::writeByte(uint8_t value)
{
    QMutexLocker lock(&_vds1Controller->_runtimeStateGuard);

    if (_vds1Controller->_operationalState == _OperationalState::_AcceptingCommand)
    {   //  we've got 1 more byte of parameter for the current command
        _vds1Controller->_commandBytes.append(value);
        if (_vds1Controller->_commandBytes.size() == _getCommandLength(_vds1Controller->_commandBytes[0]))
        {   //  Got the entire command - execute it
            _vds1Controller->_executeCommand();  //  ...this switches _state
        }
    }
    //  else not accepting parameters - ignore the write
    return true;
}

//  End of hadesvm-cereon/Vds1Controller.cpp
