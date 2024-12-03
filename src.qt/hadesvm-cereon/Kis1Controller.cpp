//
//  hadesvm-cereon/Kis1Controller.cpp
//
//  hadesvm::cereon::Kis1Controller class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Constants
const hadesvm::core::ClockFrequency  Kis1Controller::DefaultClockFrequency = hadesvm::core::ClockFrequency::kilohertz(10);

//////////
//  Construction/destruction
Kis1Controller::Kis1Controller()
    :   //  Configuration
        _statePortAddress(DefaultStatePortAddress),
        _currentDevicePortAddress(DefaultCurrentDevicePortAddress),
        _interruptMaskPortAddress(DefaultInterruptMaskPortAddress),
        _inputSourcePortAddress(DefaultInputSourcePortAddress),
        _deviceStatePortAddress(DefaultDeviceStatePortAddress),
        _dataInPortAddress(DefaultDataInPortAddress),
        _layoutPortAddress(DefaultLayoutPortAddress),
        _numberOfCompartments(DefaultNumberOfCompartments),
        _clockFrequency(DefaultClockFrequency),
        //  Runtime state
        _runtimeStateGuard(),
        _ioPorts(),
        _keyboards(),
        //  Ports
        _statePort(this),
        _currentDevicePort(this),
        _interruptMaskPort(this),
        _inputSourcePort(this),
        _deviceStatePort(this),
        _dataInPort(this),
        _layoutPort(this)
{
    for (int i = 0; i < 256; i++)
    {
        _keyboardsMap[i] = nullptr;
    }

    _ioPorts.append(&_statePort);
    _ioPorts.append(&_currentDevicePort);
    _ioPorts.append(&_interruptMaskPort);
    _ioPorts.append(&_inputSourcePort);
    _ioPorts.append(&_deviceStatePort);
    _ioPorts.append(&_dataInPort);
    _ioPorts.append(&_layoutPort);
}

Kis1Controller::~Kis1Controller() noexcept
{
}

//////////
//  hadesvm::core::Component
QString Kis1Controller::displayName() const
{
    return hadesvm::util::toString(_clockFrequency) +
           " " +
           Type::instance()->displayName() +
           " @ " +
           hadesvm::util::toString(_statePortAddress, "%04X");
}

void Kis1Controller::serialiseConfiguration(QDomElement componentElement) const
{
    componentElement.setAttribute("StatePortAddress", hadesvm::util::toString(_statePortAddress, "%04X"));
    componentElement.setAttribute("CurrentDevicePortAddress", hadesvm::util::toString(_currentDevicePortAddress, "%04X"));
    componentElement.setAttribute("InterruptMaskPortAddress", hadesvm::util::toString(_interruptMaskPortAddress, "%04X"));
    componentElement.setAttribute("InputSourcePortAddress", hadesvm::util::toString(_inputSourcePortAddress, "%04X"));
    componentElement.setAttribute("DeviceStatePortAddress", hadesvm::util::toString(_deviceStatePortAddress, "%04X"));
    componentElement.setAttribute("DataInPortAddress", hadesvm::util::toString(_dataInPortAddress, "%04X"));
    componentElement.setAttribute("LayoutPortAddress", hadesvm::util::toString(_layoutPortAddress, "%04X"));
    componentElement.setAttribute("NumberOfCompartments", hadesvm::util::toString(_numberOfCompartments));
    componentElement.setAttribute("ClockFrequency", hadesvm::util::toString(_clockFrequency));
}

void Kis1Controller::deserialiseConfiguration(QDomElement componentElement)
{
    uint16_t statePortAddress = 0;
    if (hadesvm::util::fromString(componentElement.attribute("StatePortAddress"), "%X", statePortAddress))
    {
        _statePortAddress = statePortAddress;
    }

    uint16_t currentDevicePortAddress = 0;
    if (hadesvm::util::fromString(componentElement.attribute("CurrentDevicePortAddress"), "%X", currentDevicePortAddress))
    {
        _currentDevicePortAddress = currentDevicePortAddress;
    }

    uint16_t interruptMaskPortAddress = 0;
    if (hadesvm::util::fromString(componentElement.attribute("InterruptMaskPortAddress"), "%X", interruptMaskPortAddress))
    {
        _interruptMaskPortAddress = interruptMaskPortAddress;
    }

    uint16_t inputSourcePortAddress = 0;
    if (hadesvm::util::fromString(componentElement.attribute("InputSourcePortAddress"), "%X", inputSourcePortAddress))
    {
        _inputSourcePortAddress = inputSourcePortAddress;
    }

    uint16_t deviceStatePortAddress = 0;
    if (hadesvm::util::fromString(componentElement.attribute("DeviceStatePortAddress"), "%X", deviceStatePortAddress))
    {
        _deviceStatePortAddress = deviceStatePortAddress;
    }

    uint16_t dataInPortAddress = 0;
    if (hadesvm::util::fromString(componentElement.attribute("DataInPortAddress"), "%X", dataInPortAddress))
    {
        _dataInPortAddress = dataInPortAddress;
    }

    uint16_t layoutPortAddress = 0;
    if (hadesvm::util::fromString(componentElement.attribute("LayoutPortAddress"), "%X", layoutPortAddress))
    {
        _layoutPortAddress = layoutPortAddress;
    }

    unsigned numberOfCompartments = 0;
    if (hadesvm::util::fromString(componentElement.attribute("NumberOfCompartments"), numberOfCompartments) &&
        numberOfCompartments >= 1 && numberOfCompartments <= 256)
    {
        _numberOfCompartments = numberOfCompartments;
    }

    hadesvm::core::ClockFrequency clockFrequency;
    if (hadesvm::util::fromString(componentElement.attribute("ClockFrequency"), clockFrequency) &&
        clockFrequency.toHz() > 0)
    {   //  TODO min/max clock frequency as class constants, in addition to DefaultClockFrequency
        _clockFrequency = clockFrequency;
    }
}

hadesvm::core::ComponentEditor * Kis1Controller::createEditor()
{
    return new Kis1ControllerEditor(this);
}

Kis1Controller::Ui * Kis1Controller::createUi()
{
    return nullptr;
}

//////////
//  hadesvm::core::Component (state management)
Kis1Controller::State Kis1Controller::state() const noexcept
{
    return _state;
}

void Kis1Controller::connect() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Constructed)
    {   //  OOPS! Can't
        return;
    }

    //  Locate all "Kis1Keyboard" components and attach them
    for (Kis1Keyboard * kis1Keyboard : virtualAppliance()->componentsImplementing<Kis1Keyboard>())
    {
        if (kis1Keyboard->controllerStatePortAddress() == this->statePortAddress())
        {
            if (_keyboardsMap[kis1Keyboard->controllerCompartmentNumber()] != nullptr)
            {   //  OOPS! Cleanup and report
                _keyboards.clear();
                for (int i = 0; i < 256; i++)
                {
                    _keyboardsMap[i] = nullptr;
                }
                throw hadesvm::core::VirtualApplianceException(
                    "KIS1 compartment 0x" +
                    hadesvm::util::toString(kis1Keyboard->controllerCompartmentNumber(), "%02X") +
                    " is already in use");
            }
            _keyboards.append(kis1Keyboard);
            _keyboardsMap[kis1Keyboard->controllerCompartmentNumber()] = kis1Keyboard;
        }
    }

    _state = State::Connected;
}

void Kis1Controller::initialize() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Initialized;
}

void Kis1Controller::start() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Running;
}

void Kis1Controller::stop() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Running)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Initialized;
}

void Kis1Controller::deinitialize() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    //  Detach all keyboards
    _keyboards.clear();
    for (int i = 0; i < 256; i++)
    {
        _keyboardsMap[i] = nullptr;
    }

    _state = State::Connected;
}

void Kis1Controller::disconnect() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Constructed;
}

void Kis1Controller::reset() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    _operationalState = _OperationalState::_Ready;
    _timeout = 0;
    _currentDevice = 0;
    _interruptMask = 0;
    _inputSource = 0;
    _busyOffInterrputPending = false;
    _inputReadyOnInterrputPending = false;

    delete _statePort.releasePendingIoInterrupt(); //  in case one exists
}

//////////
//  hadesvm::core::IClockedComponent
void Kis1Controller::onClockTick() noexcept
{
    QMutexLocker lock(&_runtimeStateGuard);

    switch (_operationalState)
    {
        case _OperationalState::_ChangingCurrentDevice:
        case _OperationalState::_ChangingInterruptMask:
        case _OperationalState::_ChangingDeviceState:
            if (_timeout > 0)
            {   //  Still busy
                _timeout--;
                return;
            }
            //  Finished the "write to controller register" operation
            _raiseBusyOffInterrupt();
            _operationalState = _OperationalState::_Ready;
            [[fallthrough]];    //  may need  to become InputReady if input is available

        case _OperationalState::_Ready:
            //  Is there available input in any keyboard ?
            for (Kis1Keyboard * keyboard : _keyboards)
            {
                if (keyboard->isInputReady())
                {   //  Yes!
                    _operationalState = _OperationalState::_InputReady;
                    _inputSource = keyboard->controllerCompartmentNumber();
                    _raiseInputReadyOnInterrupt();
                    break;
                }
            }
            return;

        case _OperationalState::_InputReady:
            return; //  nothing to do

        default:
            failure();
    }
}

//////////
//  IIoController
IoPortList Kis1Controller::ioPorts()
{
    return _ioPorts;
}

//////////
//  Operations (configuration)
void Kis1Controller::setStatePortAddress(uint16_t statePortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    _statePortAddress = statePortAddress;
}

void Kis1Controller::setCurrentDevicePortAddress(uint16_t currentDevicePortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    _currentDevicePortAddress = currentDevicePortAddress;
}

void Kis1Controller::setInterruptMaskPortAddress(uint16_t interruptMaskPortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    _interruptMaskPortAddress = interruptMaskPortAddress;
}

void Kis1Controller::setInputSourcePortAddress(uint16_t inputSourcePortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    _inputSourcePortAddress = inputSourcePortAddress;
}

void Kis1Controller::setDeviceStatePortAddress(uint16_t deviceStatePortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    _deviceStatePortAddress = deviceStatePortAddress;
}

void Kis1Controller::setDataInPortAddress(uint16_t dataInPortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    _dataInPortAddress = dataInPortAddress;
}

void Kis1Controller::setLayoutPortAddress(uint16_t  layoutPortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    _layoutPortAddress = layoutPortAddress;
}

void Kis1Controller::setNumberOfCompartments(unsigned numberOfCompartments)
{
    Q_ASSERT(_state == State::Constructed);

    if (numberOfCompartments >= 1 && numberOfCompartments <= 256)
    {
        _numberOfCompartments = numberOfCompartments;
    }
}

void Kis1Controller::setClockFrequency(const hadesvm::core::ClockFrequency & clockFrequency)
{
    Q_ASSERT(_state == State::Constructed);

    if (clockFrequency.toHz() > 0)
    {   //  TODO min/max
        _clockFrequency = clockFrequency;
    }
}

//////////
//  hadesvm::cereon::Kis1Controller::Type
HADESVM_IMPLEMENT_SINGLETON(Kis1Controller::Type)
Kis1Controller::Type::Type() {}
Kis1Controller::Type::~Type() {}

QString Kis1Controller::Type::mnemonic() const
{
    return "CereonKis1Controller";
}

QString Kis1Controller::Type::displayName() const
{
    return "Cereon KIS1 controller";
}

hadesvm::core::ComponentCategory * Kis1Controller::Type::category() const
{
    return hadesvm::core::StandardComponentCategories::IoControllers;
}

bool Kis1Controller::Type::isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const
{
    return architecture == CereonWorkstationArchitecture::instance();
}

bool Kis1Controller::Type::isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const
{
    return type == hadesvm::core::VirtualMachineType::instance();
}

Kis1Controller * Kis1Controller::Type::createComponent()
{
    return new Kis1Controller();
}

//////////
//  Implementation helpers
void Kis1Controller::_raiseBusyOffInterrupt()
{
    if ((_interruptMask & _BusyOffInterrputMask) != 0)
    {   //  BUSY_OFF interrupt is permitted...
        if (!_busyOffInterrputPending)
        {   //  ...and not yet pending - mark it as "pending"...
            _busyOffInterrputPending = true;
            //  ...and update the IoInterrupt in the STATE port
            if (_statePort.pendingIoInterrupt() == nullptr)
            {
                _statePort.setPendingIoInterrupt(_BusyOffInterrput);
            }
        }
    }
}

void Kis1Controller::_raiseInputReadyOnInterrupt()
{
    if ((_interruptMask & _InputReadyOnInterrputMask) != 0)
    {   //  BUSY_OFF interrupt is permitted...
        if (!_inputReadyOnInterrputPending)
        {   //  ...and not yet pending - mark it as "pending"...
            _inputReadyOnInterrputPending = true;
            //  ...and update the IoInterrupt in the STATE port
            if (_statePort.pendingIoInterrupt() == nullptr)
            {
                _statePort.setPendingIoInterrupt(_InputReadyOnInterrput);
            }
        }
    }
}

void Kis1Controller::_synchronizeInterruptState()
{
    //  If an interrupt currently pending is being disabled, drop it
    if (_statePort.pendingIoInterrupt() != nullptr)
    {
        if ((_interruptMask & _BusyOffInterrputMask) == 0 &&
            _statePort.pendingIoInterrupt()->interruptStatusCode() == _BusyOffInterrput)
        {   //  Drop it
            delete _statePort.releasePendingIoInterrupt();
            _busyOffInterrputPending = false;
        }
        if ((_interruptMask & _InputReadyOnInterrputMask) == 0 &&
            _statePort.pendingIoInterrupt()->interruptStatusCode() == _InputReadyOnInterrput)
        {   //  Drop it
            delete _statePort.releasePendingIoInterrupt();
            _inputReadyOnInterrputPending = false;
        }
    }
    //  If there are pending interrupts, reflect their status in I/O port
    if (_statePort.pendingIoInterrupt() == nullptr)
    {
        if (_busyOffInterrputPending)
        {
            _statePort.setPendingIoInterrupt(_BusyOffInterrput);
        }
        else if (_inputReadyOnInterrputPending)
        {
            _statePort.setPendingIoInterrupt(_InputReadyOnInterrput);
        }
    }
}

//////////
//  Kis1::_StatePort
IoInterrupt * Kis1Controller::_StatePort::releasePendingIoInterrupt()
{
    QMutexLocker lock(&_kis1Controller->_runtimeStateGuard);

    IoInterrupt * ioInterrupt = IIoPort::releasePendingIoInterrupt();
    if (ioInterrupt != nullptr)
    {   //  The interrupt was actually pending in this port and is now "released"
        switch (ioInterrupt->interruptStatusCode())
        {
            case _BusyOffInterrput:
                _kis1Controller->_busyOffInterrputPending = false;
                break;
            case _InputReadyOnInterrput:
                _kis1Controller->_inputReadyOnInterrputPending = false;
                break;
            default:
                failure_with_message("Not implemented");
        }
    }
    return ioInterrupt;
}

uint8_t Kis1Controller::_StatePort::readByte() throws (IoError)
{
    QMutexLocker lock(&_kis1Controller->_runtimeStateGuard);

    uint8_t result = 0x01;
    switch (_kis1Controller->_operationalState)
    {
        case _OperationalState::_Ready:
            //  Not BUSY, not INPUT_READY
            break;
        case _OperationalState::_InputReady:
            //  Not BUSY, INPUT_READY
            result |= 0x04;
            break;
        case _OperationalState::_ChangingCurrentDevice:
        case _OperationalState::_ChangingInterruptMask:
        case _OperationalState::_ChangingDeviceState:
            //  BUSY, not INPUT_READY
            result |= 0x02;
            break;
        default:
            failure();
    }
    return result;
}

void Kis1Controller::_StatePort::writeByte(uint8_t /*value*/) throws(IoError)
{   //  Writes to STATE port are ignored
}

//////////
//  Kis1::_CurrentDevicePort
uint8_t Kis1Controller::_CurrentDevicePort::readByte() throws(IoError)
{
    QMutexLocker lock(&_kis1Controller->_runtimeStateGuard);

    switch (_kis1Controller->_operationalState)
    {
        case _OperationalState::_Ready:
        case _OperationalState::_InputReady:
            return _kis1Controller->_currentDevice;
        case _OperationalState::_ChangingCurrentDevice:
        case _OperationalState::_ChangingInterruptMask:
        case _OperationalState::_ChangingDeviceState:
            //  Unpredictable value
            break;
        default:
            failure();
    }
    return 0;
}

void Kis1Controller::_CurrentDevicePort::writeByte(uint8_t value) throws(IoError)
{
    QMutexLocker lock(&_kis1Controller->_runtimeStateGuard);

    switch (_kis1Controller->_operationalState)
    {
        case _OperationalState::_Ready:
        case _OperationalState::_InputReady:
            if (_kis1Controller->_keyboardsMap[value] != nullptr)
            {   //  Compartment exists - make it "current"...
                _kis1Controller->_currentDevice = value;
                //  ...and perform state change with simulated register write delay
                _kis1Controller->_operationalState = _OperationalState::_ChangingCurrentDevice;
                _kis1Controller->_timeout = _RegisterWriteDelay;
            }   //  else device does not exist - suppress the write
            break;
        case _OperationalState::_ChangingCurrentDevice:
        case _OperationalState::_ChangingInterruptMask:
        case _OperationalState::_ChangingDeviceState:
            //  In any other state writes to CURRENT_DEVICE are suppressed
            break;
        default:
            failure();
    }
}

//////////
//  Kis1::_InterruptMaskPort
uint8_t Kis1Controller::_InterruptMaskPort::readByte() throws(IoError)
{
    QMutexLocker lock(&_kis1Controller->_runtimeStateGuard);

    switch (_kis1Controller->_operationalState)
    {
        case _OperationalState::_Ready:
        case _OperationalState::_InputReady:
            return _kis1Controller->_interruptMask;
        case _OperationalState::_ChangingCurrentDevice:
        case _OperationalState::_ChangingInterruptMask:
        case _OperationalState::_ChangingDeviceState:
            //  Unpredictable value
            break;
        default:
            failure();
    }
    return 0;
}

void Kis1Controller::_InterruptMaskPort::writeByte(uint8_t value) throws(IoError)
{
    QMutexLocker lock(&_kis1Controller->_runtimeStateGuard);

    switch (_kis1Controller->_operationalState)
    {
        case _OperationalState::_Ready:
        case _OperationalState::_InputReady:
            //  Write...
            _kis1Controller->_interruptMask = value;
            _kis1Controller->_synchronizeInterruptState();
            //  ...and perform state change with simulated register write delay
            _kis1Controller->_operationalState = _OperationalState::_ChangingInterruptMask;
            _kis1Controller->_timeout = _RegisterWriteDelay;
            break;
        case _OperationalState::_ChangingCurrentDevice:
        case _OperationalState::_ChangingInterruptMask:
        case _OperationalState::_ChangingDeviceState:
            //  In any other state writes to INTERRUPT_MASK are suppressed
            break;
        default:
            failure();
    }
}

//////////
//  Kis1::_InputSourcePort
uint8_t Kis1Controller::_InputSourcePort::readByte() throws(IoError)
{
    QMutexLocker lock(&_kis1Controller->_runtimeStateGuard);

    switch (_kis1Controller->_operationalState)
    {
        case _OperationalState::_Ready:
        case _OperationalState::_InputReady:
            return _kis1Controller->_inputSource;
        case _OperationalState::_ChangingCurrentDevice:
        case _OperationalState::_ChangingInterruptMask:
        case _OperationalState::_ChangingDeviceState:
            //  Unpredictable value
            break;
        default:
            failure();
    }
    return 0;
}

void Kis1Controller::_InputSourcePort::writeByte(uint8_t /*value*/) throws(IoError)
{   //  INPUT_SOURCE is a read-only port
}

//////////
//  Kis1::_DeviceStatePort
uint8_t Kis1Controller::_DeviceStatePort::readByte() throws(IoError)
{
    QMutexLocker lock(&_kis1Controller->_runtimeStateGuard);

    Kis1Keyboard * currentKeyboard = _kis1Controller->_keyboardsMap[_kis1Controller->_currentDevice];
    switch (_kis1Controller->_operationalState)
    {
        case _OperationalState::_Ready:
        case _OperationalState::_InputReady:
            return (currentKeyboard != nullptr) ?
                        currentKeyboard->readDeviceState() :
                        0;
        case _OperationalState::_ChangingCurrentDevice:
        case _OperationalState::_ChangingInterruptMask:
        case _OperationalState::_ChangingDeviceState:
            //  Unpredictable value
            break;
        default:
            failure();
    }
    return 0;
}

void Kis1Controller::_DeviceStatePort::writeByte(uint8_t value) throws(IoError)
{
    QMutexLocker lock(&_kis1Controller->_runtimeStateGuard);

    Kis1Keyboard * currentKeyboard = _kis1Controller->_keyboardsMap[_kis1Controller->_currentDevice];
    switch (_kis1Controller->_operationalState)
    {
        case _OperationalState::_Ready:
        case _OperationalState::_InputReady:
            if (currentKeyboard != nullptr)
            {   //  Delegate access there
                currentKeyboard->writeDeviceState(value);
                //  ...and perform state change with simulated register write delay
                _kis1Controller->_operationalState = _OperationalState::_ChangingDeviceState;
                _kis1Controller->_timeout = _RegisterWriteDelay;
            }   //  else device does not exist - suppress the write
            break;
        case _OperationalState::_ChangingCurrentDevice:
        case _OperationalState::_ChangingInterruptMask:
        case _OperationalState::_ChangingDeviceState:
            //  Not ready to handle the write - suppress it
            break;
        default:
            failure();
    }
}

//////////
//  Kis1::_DataInPort
uint8_t Kis1Controller::_DataInPort::readByte() throws(IoError)
{
    QMutexLocker lock(&_kis1Controller->_runtimeStateGuard);

    Kis1Keyboard * currentKeyboard = _kis1Controller->_keyboardsMap[_kis1Controller->_currentDevice];
    switch (_kis1Controller->_operationalState)
    {
        case _OperationalState::_Ready:
        case _OperationalState::_InputReady:
            return static_cast<uint8_t>(
                    (currentKeyboard != nullptr) ?
                        currentKeyboard->readDataIn() :
                        0);
        case _OperationalState::_ChangingCurrentDevice:
        case _OperationalState::_ChangingInterruptMask:
        case _OperationalState::_ChangingDeviceState:
            //  Unpredictable value
            break;
        default:
            failure();
    }
    return 0;
}

void Kis1Controller::_DataInPort::writeByte(uint8_t /*value*/) throws(IoError)
{   //  DATA_IN is a read-only port
}

//////////
//  Kis1::_LayoutPort
uint8_t Kis1Controller::_LayoutPort::readByte() throws(IoError)
{
    QMutexLocker lock(&_kis1Controller->_runtimeStateGuard);

    Kis1Keyboard * currentKeyboard = _kis1Controller->_keyboardsMap[_kis1Controller->_currentDevice];
    switch (_kis1Controller->_operationalState)
    {
        case _OperationalState::_Ready:
        case _OperationalState::_InputReady:
            return static_cast<uint8_t>(
                    (currentKeyboard != nullptr) ?
                        currentKeyboard->layout()->code() :
                        Kis1KeyboardLayout::Code::Unknown);
        case _OperationalState::_ChangingCurrentDevice:
        case _OperationalState::_ChangingInterruptMask:
        case _OperationalState::_ChangingDeviceState:
            //  Unpredictable value
            break;
        default:
            failure();
    }
    return 0;
}

void Kis1Controller::_LayoutPort::writeByte(uint8_t /*value*/) throws(IoError)
{   //  Writes to LAYOUT register are suppressed
}

//  End of hadesvm-cereon/Kis1Controller.cpp
