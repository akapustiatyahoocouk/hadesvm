//
//  hadesvm-cereon/Kis1Keyboard.cpp
//
//  hadesvm::cereon::Kis1Keyboard class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Construction/destruction
Kis1Keyboard::Kis1Keyboard()
    :   //  Configuration
        _controllerStatePortAddress(DefaultControllerStatePortAddress),
        _controllerCompartmentNumber(DefaultCompartmentNumber),
        _layout(Kis1KeyboardLayout::system()),
        //  Runtime state
        _readyInputQueue(),
        _readyInputQueueGuard()
{
}

Kis1Keyboard::~Kis1Keyboard() noexcept
{
}

//////////
//  hadesvm::core::Component
QString Kis1Keyboard::displayName() const
{
    return Type::instance()->displayName() +
           " @ " +
           hadesvm::util::toString(_controllerStatePortAddress, "%04X") +
           ":" +
           hadesvm::util::toString(_controllerCompartmentNumber);
}

void Kis1Keyboard::serialiseConfiguration(QDomElement componentElement) const
{
    componentElement.setAttribute("ControllerStatePortAddress", hadesvm::util::toString(_controllerStatePortAddress, "%04X"));
    componentElement.setAttribute("ControllerCompartmentNumber", hadesvm::util::toString(_controllerCompartmentNumber));
    componentElement.setAttribute("Layout", _layout->mnemonic());
}

void Kis1Keyboard::deserialiseConfiguration(QDomElement componentElement)
{
    uint16_t controllerStatePortAddress = 0;
    if (hadesvm::util::fromString(componentElement.attribute("ControllerStatePortAddress"), "%X", controllerStatePortAddress))
    {
        _controllerStatePortAddress = controllerStatePortAddress;
    }

    uint8_t controllerCompartmentNumber = 0;
    if (hadesvm::util::fromString(componentElement.attribute("ControllerCompartmentNumber"), controllerCompartmentNumber))
    {
        _controllerCompartmentNumber = controllerCompartmentNumber;
    }

    if (Kis1KeyboardLayout * layout = Kis1KeyboardLayout::findByMnemonic(componentElement.attribute("Layout")))
    {
        _layout = layout;
    }
}

hadesvm::core::ComponentEditor * Kis1Keyboard::createEditor(QWidget * parent)
{
    return new Kis1KeyboardEditor(parent, this);
}

Kis1Keyboard::Ui * Kis1Keyboard::createUi()
{
    return nullptr;
}

//////////
//  hadesvm::core::Component (state management)
Kis1Keyboard::State Kis1Keyboard::state() const noexcept
{
    return _state;
}

void Kis1Keyboard::connect() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Constructed)
    {   //  OOPS! Can't
        return;
    }

    //  If a KIS1 controller does not exist at _controllerStatePortAddress
    //  or has less than _controllerCompartmentNumber compartments, it's an error
    QList<Kis1Controller*> kis1Controllers;
    for (auto kis1Controller : virtualAppliance()->componentsImplementing<Kis1Controller>())
    {
        if (kis1Controller->statePortAddress() == _controllerStatePortAddress)
        {
            kis1Controllers.append(kis1Controller);
        }
    }
    if (kis1Controllers.count() == 0)
    {
        throw hadesvm::core::VirtualApplianceException("TODO proper error message");
    }
    if (kis1Controllers.count() > 1)
    {
        throw hadesvm::core::VirtualApplianceException("TODO proper error message");
    }
    if (_controllerCompartmentNumber >= kis1Controllers[0]->numberOfCompartments())
    {
        throw hadesvm::core::VirtualApplianceException("TODO proper error message");
    }

    _state = State::Connected;
}

void Kis1Keyboard::initialize() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Initialized;
}

void Kis1Keyboard::start() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Running;
}

void Kis1Keyboard::stop() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Running)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Initialized;
}

void Kis1Keyboard::deinitialize() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Connected;
}

void Kis1Keyboard::disconnect() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Constructed;
}

//////////
//  Operations (configuration)
void Kis1Keyboard::setControllerStatePortAddress(uint16_t controllerStatePortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    //  TODO validate "controllerStatePortAddress"
    _controllerStatePortAddress = controllerStatePortAddress;
}

void Kis1Keyboard::setControllerCompartmentNumber(uint8_t controllerCompartmentNumber)
{
    Q_ASSERT(_state == State::Constructed);

    //  TODO validate "controllerCompartmentNumber"
    _controllerCompartmentNumber = controllerCompartmentNumber;
}

void Kis1Keyboard::setLayout(Kis1KeyboardLayout * layout)
{
    Q_ASSERT(_state == State::Constructed);
    Q_ASSERT(layout != nullptr);

    _layout = layout;
}

//////////
//  Operations
bool Kis1Keyboard::isInputReady() const
{
    QMutexLocker lock(&_readyInputQueueGuard);

    return !_readyInputQueue.isEmpty();
}

uint8_t Kis1Keyboard::readDeviceState()
{
    uint8_t result = 0x01;  //  ACTIVE
    if (_numLockOn)
    {
        result |= 0x02;
    }
    if (_capsLockOn)
    {
        result |= 0x04;
    }
    if (_scrollLockOn)
    {
        result |= 0x08;
    }
    if (isInputReady())
    {
        result |= 0x80;
    }
    return result;
}

void Kis1Keyboard::writeDeviceState(uint8_t deviceState)
{
    QMutexLocker lock(&_readyInputQueueGuard);

    //  LED states
    _numLockOn = ((deviceState & 0x02) != 0);
    _capsLockOn = ((deviceState & 0x04) != 0);
    _scrollLockOn = ((deviceState & 0x08) != 0);

    //  Input state
    if (!_readyInputQueue.isEmpty() && (deviceState & 0x80) == 0)
    {   //  Done handling current input byte
        _readyInputQueue.dequeue();
    }
}

uint8_t Kis1Keyboard::readDataIn()
{
    QMutexLocker lock(&_readyInputQueueGuard);

    return _readyInputQueue.isEmpty() ? 0 : _readyInputQueue.first();
}

void Kis1Keyboard::handleKeyPressed(QKeyEvent * keyEvent)
{
    Kis1ScanCode scanCode = _layout->translateKey(keyEvent->nativeScanCode(), keyEvent->nativeVirtualKey());
    if (scanCode != Kis1ScanCode::Invalid)
    {
        QMutexLocker lock(&_readyInputQueueGuard);
        _readyInputQueue.enqueue(static_cast<uint8_t>(scanCode));
    }
}

void Kis1Keyboard::handleKeyReleased(QKeyEvent * keyEvent)
{
    Kis1ScanCode scanCode = _layout->translateKey(keyEvent->nativeScanCode(), keyEvent->nativeVirtualKey());
    if (scanCode != Kis1ScanCode::Invalid)
    {
        QMutexLocker lock(&_readyInputQueueGuard);
        _readyInputQueue.enqueue(static_cast<uint8_t>(scanCode) | 0x80);
    }
}

//////////
//  hadesvm::cereon::Kis1Keyboard::Type
HADESVM_IMPLEMENT_SINGLETON(Kis1Keyboard::Type)
Kis1Keyboard::Type::Type() {}
Kis1Keyboard::Type::~Type() {}

QString Kis1Keyboard::Type::mnemonic() const
{
    return "CereonKis1Keyboard";
}

QString Kis1Keyboard::Type::displayName() const
{
    return "Cereon KIS1 keyboard";
}

hadesvm::core::ComponentCategory * Kis1Keyboard::Type::category() const
{
    return hadesvm::core::StandardComponentCategories::UiDevices;
}

bool Kis1Keyboard::Type::suspendable() const
{
    return true;
}

bool Kis1Keyboard::Type::isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const
{
    return architecture == CereonWorkstationArchitecture::instance();
}

bool Kis1Keyboard::Type::isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const
{
    return type == hadesvm::core::VirtualMachineType::instance();
}

Kis1Keyboard * Kis1Keyboard::Type::createComponent()
{
    return new Kis1Keyboard();
}

//  End of hadesvm-cereon/Kis1Keyboard.cpp
