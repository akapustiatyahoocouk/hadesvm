//
//  hadesvm-cereon/Cmos1.cpp
//
//  hadesvm::cereon::Cmos1 class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Constants
const hadesvm::core::TimeInterval    Cmos1::DefaultReadDelay = hadesvm::core::TimeInterval::microseconds(1);
const hadesvm::core::TimeInterval    Cmos1::DefaultWriteDelay = hadesvm::core::TimeInterval::milliseconds(10);
const uint16_t                       Cmos1::DefaultStatePortAddress = 0x0110;
const uint16_t                       Cmos1::DefaultAddressPortAddress = 0x0111;
const uint16_t                       Cmos1::DefaultDataPortAddress = 0x0112;
const uint16_t                       Cmos1::DefaultInterruptMaskPortAddress = 0x0113;
const hadesvm::core::ClockFrequency  Cmos1::DefaultClockFrequency = hadesvm::core::ClockFrequency::megahertz(1);
const QString                        Cmos1::DefaultContentFilePath = "./cmos.dat";

//////////
//  Construction/destruction
Cmos1::Cmos1()
    :   //  Configuration
        _readDelay(DefaultReadDelay),
        _writeDelay(DefaultWriteDelay),
        _statePortAddress(DefaultStatePortAddress),
        _addressPortAddress(DefaultAddressPortAddress),
        _dataPortAddress(DefaultDataPortAddress),
        _interruptMaskPortAddress(DefaultInterruptMaskPortAddress),
        _clockFrequency(DefaultClockFrequency),
        _contentFilePath(DefaultContentFilePath),
        _clockTicksBetweenTimeUpdates(static_cast<unsigned>(_clockFrequency.toHz() / 1000)),
        //  Runtime state
        _ioPorts(),
        _content(),
        _contentNeedsSaving(false),
        _clockTicksUntilTimeUpdate(0),
        _statePort(this, _statePortAddress),
        _addressPort(this, _addressPortAddress),
        _dataPort(this, _dataPortAddress)
{
    _ioPorts.append(&_statePort);
    _ioPorts.append(&_addressPort);
    _ioPorts.append(&_dataPort);
}

Cmos1::~Cmos1() noexcept
{
}

//////////
//  hadesvm::core::Component
QString Cmos1::displayName() const
{
    return hadesvm::util::toString(_clockFrequency) + " " + Type::instance()->displayName();
}

void Cmos1::serialiseConfiguration(QDomElement componentElement) const
{
    componentElement.setAttribute("ReadDelay", hadesvm::util::toString(_readDelay));
    componentElement.setAttribute("WriteDelay", hadesvm::util::toString(_writeDelay));
    componentElement.setAttribute("StatePortAddress", hadesvm::util::toString(_statePortAddress, "%04X"));
    componentElement.setAttribute("AddressPortAddress", hadesvm::util::toString(_addressPortAddress, "%04X"));
    componentElement.setAttribute("DataPortAddress", hadesvm::util::toString(_dataPortAddress, "%04X"));
    componentElement.setAttribute("InterruptMaskPortAddress", hadesvm::util::toString(_interruptMaskPortAddress, "%04X"));
    componentElement.setAttribute("ClockFrequency", hadesvm::util::toString(_clockFrequency));
    componentElement.setAttribute("ContentFilePath", _contentFilePath);
}

void Cmos1::deserialiseConfiguration(QDomElement /*componentElement*/)
{
    //  TODO implement
}

hadesvm::core::ComponentEditor * Cmos1::createEditor(QWidget * /*parent*/)
{
    return nullptr; //  TODO editor!
}

Cmos1::Ui * Cmos1::createUi()
{
    return nullptr;
}

//////////
//  hadesvm::core::Component (state management)
Cmos1::State Cmos1::state() const noexcept
{
    return _state;
}

void Cmos1::connect() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Constructed)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Connected;
}

void Cmos1::initialize() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    //  Load content
    memset(_content, 0, sizeof(_content));

    QString fileName = virtualAppliance()->toAbsolutePath(_contentFilePath);
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        QDataStream in(&file);
        in.readRawData(_content, sizeof(_content));
    }


    _state = State::Initialized;
}

void Cmos1::start() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Running;
}

void Cmos1::stop() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Running)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Initialized;
}

void Cmos1::deinitialize() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    if (_contentNeedsSaving)
    {   //  Save content
        QString fileName = virtualAppliance()->toAbsolutePath(_contentFilePath);
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly))
        {
            QDataStream in(&file);
            in.writeRawData(_content, sizeof(_content));
        }
    }

    _state = State::Connected;
}

void Cmos1::disconnect() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Constructed;
}

//////////
//  hadesvm::core::IClockedComponentAspect
#if 0   //  TODO uncomment & implemnent
void Cmos1::onClockTick()
{
    if (_clockTicksBetweenTimeUpdates == 0 || _clockTicksUntilTimeUpdate == 0)
    {   //  Update calendar time in bytes 0..15 of the content
        _clockTicksUntilTimeUpdate = _clockTicksBetweenTimeUpdates;
        carl::DateTime now = _utcTimeZone->timeToLocal(carl::TimeStamp::now());
        _content[0] = static_cast<uint8_t>('0' + (now.getYear() / 1000) % 10);
        _content[1] = static_cast<uint8_t>('0' + (now.getYear() / 100) % 10);
        _content[2] = static_cast<uint8_t>('0' + (now.getYear() / 10) % 10);
        _content[3] = static_cast<uint8_t>('0' + (now.getYear() / 1) % 10);
        _content[4] = static_cast<uint8_t>('0' + (now.getMonth() / 10) % 10);
        _content[5] = static_cast<uint8_t>('0' + (now.getMonth() / 1) % 10);
        _content[6] = static_cast<uint8_t>('0' + (now.getDay() / 10) % 10);
        _content[7] = static_cast<uint8_t>('0' + (now.getDay() / 1) % 10);
        _content[8] = static_cast<uint8_t>('0' + (now.getHour() / 10) % 10);
        _content[9] = static_cast<uint8_t>('0' + (now.getHour() / 1) % 10);
        _content[10] = static_cast<uint8_t>('0' + (now.getMinute() / 10) % 10);
        _content[11] = static_cast<uint8_t>('0' + (now.getMinute() / 1) % 10);
        _content[12] = static_cast<uint8_t>('0' + (now.getSecond() / 10) % 10);
        _content[13] = static_cast<uint8_t>('0' + (now.getSecond() / 1) % 10);
        _content[14] = static_cast<uint8_t>('0' + (now.getNanosecond() / 100000000) % 10);
        _content[15] = static_cast<uint8_t>('0' + (now.getNanosecond() / 10000000) % 10);
        _contentNeedsSaving = true;
    }
    if (_clockTicksUntilTimeUpdate > 0)
    {
        _clockTicksUntilTimeUpdate--;
    }

    //  Busy ?
    if (_clockTicksToDelay > 0)
    {
        Q_ASSERT(_state != _OperationalState::_Ready);
        if (--_clockTicksToDelay == 0)
        {   //  End of delay
            _state = _OperationalState::_Ready;
        }
    }
}
#endif

//////////
//  IIoControllerAspect
IoPortList Cmos1::ioPorts()
{
    return _ioPorts;
}

//////////
//  Operations (component configuration)
void Cmos1::setReadDelay(const hadesvm::core::TimeInterval & readDelay)
{
    Q_ASSERT(_state == State::Constructed);

    //  TODO validate "readDelay"
    _readDelay = readDelay;
}

void Cmos1::setWriteDelay(const hadesvm::core::TimeInterval & writeDelay)
{
    Q_ASSERT(_state == State::Constructed);

    //  TODO validate "writeDelay"
    _writeDelay = writeDelay;
}

void Cmos1::setStatePortAddress(uint16_t statePortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    //  TODO validate "statePortAddress"
    _statePortAddress = statePortAddress;
}

void Cmos1::setAddressPortAddress(uint16_t addressPortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    //  TODO validate "addressPortAddress"
    _addressPortAddress = addressPortAddress;
}

void Cmos1::setDataPortAddress(uint16_t dataPortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    //  TODO validate "dataPortAddress"
    _dataPortAddress = dataPortAddress;
}

void Cmos1::setInterruptMaskPortAddress(uint16_t interruptMaskPortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    //  TODO validate "interruptMaskPortAddress"
    _interruptMaskPortAddress = interruptMaskPortAddress;
}

void Cmos1::setClockFrequency(const hadesvm::core::ClockFrequency & clockFrequency)
{
    Q_ASSERT(_state == State::Constructed);

    //  TODO validate "clockFrequency"
    _clockFrequency = clockFrequency;
}

void Cmos1::setContentFilePath(const QString & contentFilePath)
{
    Q_ASSERT(_state == State::Constructed);

    //  TODO validate "contentFilePath"
    _contentFilePath = contentFilePath;
}

//////////
//  Cmos1::_StatePort
bool Cmos1::_StatePort::readByte(uint8_t & value)
{
    value = 0x01;
    if (_cmos1->_operationalState == _OperationalState::_Ready)
    {
        value |= 0x06; //  ADDRESS_READY + DATA_READY
    }
    return true;
}

bool Cmos1::_StatePort::writeByte(uint8_t /*value*/)
{   //  Writes to STATE port are ignored
    return true;
}

//////////
//  Cmos1::_AddressPort
bool Cmos1::_AddressPort::readByte(uint8_t & value)
{
    if (_cmos1->_operationalState != _OperationalState::_Ready)
    {   //  OOPS! Busy!
        value = 0;
        return true;
    }

    //  Simulate read delay
    //  Note that we're reading from the controller's register, not from EPROM
    _cmos1->_clockTicksToDelay = 1;

    //  Perform read
    _cmos1->_operationalState = _OperationalState::_ReadingAddress;
    value = _cmos1->_currentAddress;
    return true;
}

bool Cmos1::_AddressPort::writeByte(uint8_t value)
{
    if (_cmos1->_operationalState != _OperationalState::_Ready)
    {   //  OOPS! Busy!
        return true;
    }

    //  Simulate write delay.
    //  Note that we're writing to the controller's register, not to EPROM
    _cmos1->_clockTicksToDelay = 1;

    //  Perform write
    _cmos1->_operationalState = _OperationalState::_WritingAddress;
    _cmos1->_currentAddress = value;

    return true;
}

//////////
//  Cmos1::_DataPort
bool Cmos1::_DataPort::readByte(uint8_t & value)
{
    if (_cmos1->_operationalState != _OperationalState::_Ready)
    {   //  OOPS! Busy!
        value = 0;
        return true;
    }

    //  Simulate read delay
    _cmos1->_clockTicksToDelay =
        static_cast<unsigned>(
            _cmos1->_readDelay.toNs() * _cmos1->_clockFrequency.toHz() / 1000000000);

    //  Perform read
    _cmos1->_operationalState = (_cmos1->_clockTicksToDelay > 0) ? _OperationalState::_ReadingData : _OperationalState::_Ready;
    value = _cmos1->_content[_cmos1->_currentAddress];
    return true;
}

bool Cmos1::_DataPort::writeByte(uint8_t value)
{
    if (_cmos1->_operationalState != _OperationalState::_Ready)
    {   //  OOPS! Busy!
        return true;
    }

    //  Simulate write delay.
    _cmos1->_clockTicksToDelay =
        static_cast<unsigned>(
            _cmos1->_writeDelay.toNs() * _cmos1->_clockFrequency.toHz() / 1000000000);

    //  Perform write
    _cmos1->_operationalState = (_cmos1->_clockTicksToDelay > 0) ? _OperationalState::_WritingData : _OperationalState::_Ready;
    _cmos1->_content[_cmos1->_currentAddress] = value;

    return true;
}

//////////
//  hadesvm::cereon::Cmos1::Type
HADESVM_IMPLEMENT_SINGLETON(Cmos1::Type)
Cmos1::Type::Type() {}
Cmos1::Type::~Type() {}

QString Cmos1::Type::mnemonic() const
{
    return "CereonCmos1";
}

QString Cmos1::Type::displayName() const
{
    return "Cereon CMOS1";
}

hadesvm::core::ComponentCategory * Cmos1::Type::category() const
{
    return hadesvm::core::StandardComponentCategories::InternalDevices;
}

bool Cmos1::Type::suspendable() const
{
    return true;
}

bool Cmos1::Type::isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const
{
    return architecture == CereonWorkstationArchitecture::instance();
}

bool Cmos1::Type::isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const
{
    return type == hadesvm::core::VirtualMachineType::instance();
}

Cmos1 * Cmos1::Type::createComponent()
{
    return new Cmos1();
}

//  End of hadesvm-cereon/Cmos1.cpp
