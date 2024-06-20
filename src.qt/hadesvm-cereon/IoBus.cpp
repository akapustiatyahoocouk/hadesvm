//
//  hadesvm-cereon/IoBus.cpp
//
//  hadesvm::cereon::IoBus class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Constants
const hadesvm::core::ClockFrequency  IoBus::DefaultClockFrequency = hadesvm::core::ClockFrequency::megahertz(10);

//////////
//  Construction/destruction
IoBus::IoBus()
    :   _clockFrequency(DefaultClockFrequency),
        _ioPorts(),
        _byteIoPorts(),
        _halfWordIoPorts(),
        _wordIoPorts(),
        _longWordIoPorts(),
        _interruptsReadyToHandle(),
        //  Bus locking
        _accessGuard()
{
    for (int i = 0; i < 65536; i++)
    {
        _ioPorts[i] = nullptr;
        _byteIoPorts[i] = nullptr;
        _halfWordIoPorts[i] = nullptr;
        _wordIoPorts[i] = nullptr;
        _longWordIoPorts[i] = nullptr;
    }
}

IoBus::~IoBus() noexcept
{
}

//////////
//  hadesvm::core::Component
QString IoBus::displayName() const
{
    return hadesvm::util::toString(_clockFrequency) + " " + Type::instance()->displayName();
}

void IoBus::serialiseConfiguration(QDomElement componentElement) const
{
    componentElement.setAttribute("ClockFrequency", hadesvm::util::toString(_clockFrequency));
}

void IoBus::deserialiseConfiguration(QDomElement componentElement)
{
    hadesvm::core::ClockFrequency clockFrequency = _clockFrequency;
    if (hadesvm::util::fromString(componentElement.attribute("ClockFrequency"), clockFrequency))
    {
        _clockFrequency = clockFrequency;
    }
}

hadesvm::core::ComponentEditor * IoBus::createEditor(QWidget * parent)
{
    return new IoBusEditor(parent, this);
}

IoBus::Ui * IoBus::createUi()
{
    return nullptr;
}

//////////
//  hadesvm::core::Component (state management)
IoBus::State IoBus::state() const noexcept
{
    return _state;
}

void IoBus::connect() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Constructed)
    {   //  OOPS! Can't
        return;
    }

    //  Locate all elements that implement the "IIoControllerAspect"
    //  and connect I/O ports they provide to this I/O bus
    try
    {
        for (IIoControllerAspect * ioController : virtualAppliance()->componentsImplementing<IIoControllerAspect>())
        {
            for (IIoPort * ioPort : ioController->ioPorts())
            {
                attachIoPort(ioPort);
            }
        }
    }
    catch (...)
    {   //  Cleanup & re-throw
        for (int i = 0; i < 65536; i++)
        {
            _ioPorts[i] = nullptr;
            _byteIoPorts[i] = nullptr;
            _halfWordIoPorts[i] = nullptr;
            _wordIoPorts[i] = nullptr;
            _longWordIoPorts[i] = nullptr;
        }
        throw;
    }

    //  Done
    _state = State::Connected;
}

void IoBus::initialize() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    //  Done
    _state = State::Initialized;
}

void IoBus::start() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    //  Done
    _state = State::Running;
}

void IoBus::stop() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Running)
    {   //  OOPS! Can't
        return;
    }

    //  Done
    _state = State::Initialized;
}

void IoBus::deinitialize() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    //  Done
    _state = State::Connected;
}

void IoBus::disconnect() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    //  Locate all elements that implement the "IIoControllerAspect"
    //  and disconnect I/O ports they provide from this I/O bus
    for (IIoControllerAspect * ioController : virtualAppliance()->componentsImplementing<IIoControllerAspect>())
    {
        for (IIoPort * ioPort : ioController->ioPorts())
        {
            detachIoPort(ioPort);
        }
    }

    //  Done
    _state = State::Constructed;
}

//////////
//  Operations (configuration)
void IoBus::setClockFrequency(const hadesvm::core::ClockFrequency & clockFrequency)
{
    Q_ASSERT(_state == State::Constructed);

    _clockFrequency = clockFrequency;
}

//////////
//  Operations
void IoBus::attachIoPort(IIoPort * ioPort) throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    Q_ASSERT(ioPort != nullptr);
    Q_ASSERT(ioPort->_ioBus == nullptr); //  not yet attached

    //  Don't attach more than once
    if (ioPort->_ioBus != nullptr)
    {   //  OOPS! Already attached...
        if (ioPort->_ioBus == this)
        {   //  ...to this same I/O bus - nothing to do
            return;
        }
        else
        {   //  ...to a different I/O bus - we have a problem
            throw hadesvm::core::VirtualApplianceException("I/O port " + hadesvm::util::toString(ioPort->address(), "%04X") + " is already attached");
        }
    }

    //  Make sure the I/O port is consistent
    if (dynamic_cast<IByteIoPort*>(ioPort) == nullptr &&
        dynamic_cast<IHalfWordIoPort*>(ioPort) == nullptr &&
        dynamic_cast<IWordIoPort*>(ioPort) == nullptr &&
        dynamic_cast<ILongWordIoPort*>(ioPort))
    {   //  OOPS! I/O port does not implement any size I/O
        throw hadesvm::core::VirtualApplianceException("I/O port " + hadesvm::util::toString(ioPort->address(), "%04X") + " does not have a size");
    }

    //  Avoid address conflicts
    if (_ioPorts[ioPort->address()] != nullptr)
    {   //  OOPS! Address already in use!
        throw hadesvm::core::VirtualApplianceException("I/O port " + hadesvm::util::toString(ioPort->address(), "%04X") + " already exists");
    }

    //  Add "ioPort" to primary ports map
    _ioPorts[ioPort->address()] = ioPort;
    ioPort->_ioBus = this;

    //  Add "ioPort" to secondary ports map
    if (IByteIoPort * byteIoPort = dynamic_cast<IByteIoPort*>(ioPort))
    {
        _byteIoPorts[ioPort->address()] = byteIoPort;
    }
    if (IHalfWordIoPort * halfWordIoPort = dynamic_cast<IHalfWordIoPort*>(ioPort))
    {
        _halfWordIoPorts[ioPort->address()] = halfWordIoPort;
    }
    if (IWordIoPort * wordIoPort = dynamic_cast<IWordIoPort*>(ioPort))
    {
        _wordIoPorts[ioPort->address()] = wordIoPort;
    }
    if (ILongWordIoPort * longWordIoPort = dynamic_cast<ILongWordIoPort*>(ioPort))
    {
        _longWordIoPorts[ioPort->address()] = longWordIoPort;
    }

    //  Keep "ready interrupts" queue consistent
    if (ioPort->_pendingIoInterrupt != nullptr && ioPort->_interruptsEnabled)
    {
        _interruptsReadyToHandle.enqueue(ioPort->_pendingIoInterrupt);
    }
}

void IoBus::detachIoPort(IIoPort * ioPort)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    Q_ASSERT(ioPort != nullptr);

    uint16_t address = ioPort->address();
    //  Attached ?
    if (_ioPorts[address] != ioPort)
    {   //  No - nothing to do
        return;
    }

    //  Remove "ioPort" from primary ports map
    Q_ASSERT(ioPort->_ioBus == this);
    _ioPorts[address] = nullptr;
    ioPort->_ioBus = nullptr;

    //  Remove "ioPort" from secondary ports map
    _byteIoPorts[address] = nullptr;
    _halfWordIoPorts[address] = nullptr;
    _wordIoPorts[address] = nullptr;
    _longWordIoPorts[address] = nullptr;

    //  Keep "ready interrupts" queue consistent
    if (ioPort->_pendingIoInterrupt != nullptr && ioPort->_interruptsEnabled)
    {
        Q_ASSERT(_interruptsReadyToHandle.contains(ioPort->_pendingIoInterrupt));
        _interruptsReadyToHandle.removeOne(ioPort->_pendingIoInterrupt);
    }
}

bool IoBus::readByte(uint16_t address, uint8_t & value)
{
    QMutexLocker lock(&_accessGuard);

    if (_byteIoPorts[address] != nullptr)
    {   //  I/O port exists
        return _byteIoPorts[address]->readByte(value);
    }
    else
    {   //  I/O port does not exist
        value = 0;
        return true;
    }
}

bool IoBus::readHalfWord(uint16_t address, uint16_t & value)
{
    QMutexLocker lock(&_accessGuard);

    if (_halfWordIoPorts[address] != nullptr)
    {   //  I/O port exists
        return _halfWordIoPorts[address]->readHalfWord(value);
    }
    else
    {   //  I/O port does not exist
        value = 0;
        return true;
    }
}

bool IoBus::readWord(uint16_t address, uint32_t & value)
{
    QMutexLocker lock(&_accessGuard);

    if (_wordIoPorts[address] != nullptr)
    {   //  I/O port exists
        return _wordIoPorts[address]->readWord(value);
    }
    else
    {   //  I/O port does not exist
        value = 0;
        return true;
    }
}

bool IoBus::readLongWord(uint16_t address, uint64_t & value)
{
    QMutexLocker lock(&_accessGuard);

    if (_longWordIoPorts[address] != nullptr)
    {   //  I/O port exists
        return _longWordIoPorts[address]->readLongWord(value);
    }
    else
    {   //  I/O port does not exist
        value = 0;
        return true;
    }
}

bool IoBus::writeByte(uint16_t address, uint8_t value)
{
    QMutexLocker lock(&_accessGuard);

    /*  TODO kill off - this is debug code  */
    if (address == 65535)
    {
        static QTextStream ts(stdout);
        ts << static_cast<char>(value);
        return true;
    }

    if (_byteIoPorts[address] != nullptr)
    {   //  I/O port exists
        return _byteIoPorts[address]->writeByte(value);
    }
    return true;    //  I/O port does not exist - write is suppressed
}

bool IoBus::writeHalfWord(uint16_t address, uint16_t value)
{
    QMutexLocker lock(&_accessGuard);

    if (_halfWordIoPorts[address] != nullptr)
    {   //  I/O port exists
        return _halfWordIoPorts[address]->writeHalfWord(value);
    }
    return true;    //  I/O port does not exist - write is suppressed
}

bool IoBus::writeWord(uint16_t address, uint32_t value)
{
    QMutexLocker lock(&_accessGuard);

    if (_wordIoPorts[address] != nullptr)
    {   //  I/O port exists
        return _wordIoPorts[address]->writeWord(value);
    }
    return true;    //  I/O port does not exist - write is suppressed
}

bool IoBus::writeLongWord(uint16_t address, uint64_t value)
{
    QMutexLocker lock(&_accessGuard);

    if (_longWordIoPorts[address] != nullptr)
    {   //  I/O port exists
        return _longWordIoPorts[address]->writeLongWord(value);
    }
    return true;    //  I/O port does not exist - write is suppressed
}

uint64_t IoBus::testPortStatus(uint16_t address)
{
    QMutexLocker lock(&_accessGuard);

    if (_ioPorts[address] != nullptr)
    {
        uint64_t result = 0x01; //  port exists
        if (IoInterrupt * ioInterrupt = _ioPorts[address]->releasePendingIoInterrupt())
        {
            result |= 0x02; //  was pending
            result |= static_cast<uint32_t>(ioInterrupt->getInterruptStatusCode()) << 16;
            delete ioInterrupt; //  handled!
        }
        if (_ioPorts[address]->interruptsEnabled())
        {
            result |= 0x04; //  enabled
        }
        return result;
    }
    return 0;   //  not there
}

void IoBus::setPortStatus(uint16_t address, uint64_t status)
{
    QMutexLocker lock(&_accessGuard);

    if (_ioPorts[address] != nullptr)
    {   //  Enabled status...
        if ((status & 0x04) != 0)
        {
            _ioPorts[address]->enableInterrupts();
        }
        else
        {
            _ioPorts[address]->disableInterrupts();
        }
        //  ...and pending interrupt
        if ((status & 0x02) != 0)
        {   //  set
            _ioPorts[address]->setPendingIoInterrupt(static_cast<uint16_t>(status >> 16));
        }
        else
        {   //  clear
            delete _ioPorts[address]->releasePendingIoInterrupt();  //  "delete nullptr" is safe
        }

    }
}

IoInterrupt * IoBus::getIoInterrupt()
{
    QMutexLocker lock(&_accessGuard);

    return _interruptsReadyToHandle.isEmpty() ? nullptr : _interruptsReadyToHandle.dequeue();
}

//////////
//  hadesvm::cereon::IoBus::Type
HADESVM_IMPLEMENT_SINGLETON(IoBus::Type)
IoBus::Type::Type() {}
IoBus::Type::~Type() {}

QString IoBus::Type::mnemonic() const
{
    return "CereonIoBus";
}

QString IoBus::Type::displayName() const
{
    return "Cereon I/O Bus";
}

hadesvm::core::ComponentCategory * IoBus::Type::category() const
{
    return hadesvm::core::StandardComponentCategories::InternalDevices;
}

bool IoBus::Type::suspendable() const
{
    return true;
}

bool IoBus::Type::isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const
{
    return architecture == CereonWorkstationArchitecture::instance();
}

bool IoBus::Type::isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const
{
    return type == hadesvm::core::VirtualMachineType::instance();
}

IoBus * IoBus::Type::createComponent()
{
    return new IoBus();
}

//  End of hadesvm-cereon/IoBus.cpp
