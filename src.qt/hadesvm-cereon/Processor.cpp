//
//  hadesvm-cereon/Processor.cpp
//
//  hadesvm::cereon::Processor class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Construction/destruction
Processor::Processor(const hadesvm::core::ClockFrequency & clockFrequency, uint8_t id,
                     ByteOrder byteOrder, bool canChangeByteOrder,
                     uint64_t restartAddress, bool isPrimaryProcessor)
    :   //  Configuration
        _clockFrequency(clockFrequency),
        _id(id),
        _byteOrder(byteOrder),
        _canChangeByteOrder(canChangeByteOrder),
        _restartAddress(restartAddress),
        _isPrimaryProcessor(isPrimaryProcessor),
        _cores()
{
}

Processor::~Processor() noexcept
{
}

//////////
//  hadesvm::core::Component
void Processor::serialiseConfiguration(QDomElement componentElement) const
{
    componentElement.setAttribute("ClockFrequency", hadesvm::util::toString(_clockFrequency));
    componentElement.setAttribute("Id", hadesvm::util::toString(_id, "%02X"));
    componentElement.setAttribute("ByteOrder", hadesvm::util::toString(_byteOrder));
    componentElement.setAttribute("CanChangeByteOrder", hadesvm::util::toString(_canChangeByteOrder));
    componentElement.setAttribute("RestartAddress", hadesvm::util::toString(_restartAddress, "%016X"));
    componentElement.setAttribute("IsPrimaryProcessor", hadesvm::util::toString(_isPrimaryProcessor));
}

void Processor::deserialiseConfiguration(QDomElement componentElement)
{
    //  TODO implement
}

hadesvm::core::ComponentEditor * Processor::createEditor(QWidget * parent)
{
    return new ProcessorEditor(parent, this);
}

Processor::Ui * Processor::createUi()
{
    return nullptr;
}

//////////
//  hadesvm::core::Component (state management)
//  Must only be called from the QApplication's main thread (except state())
Processor::State Processor::state() const noexcept
{
    return _state;
}

void Processor::connect() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Constructed)
    {   //  OOPS! Can't
        return;
    }

    //  Connect to memory and I/O buses
    QList<MemoryBus *> memoryBuses = virtualAppliance()->componentsImplementing<MemoryBus>();
    if (memoryBuses.isEmpty() || memoryBuses.size() > 1)
    {   //  OOPS!
        throw hadesvm::core::VirtualApplianceException("A Cereon processor requires the presence of a single memory bus");
    }
    _memoryBus = memoryBuses[0];

    QList<IoBus*> ioBuses = virtualAppliance()->componentsImplementing<IoBus>();
    if (ioBuses.isEmpty() || ioBuses.size() > 1)
    {   //  OOPS!
        throw hadesvm::core::VirtualApplianceException("A Cereon processor requires the presence of a single I/O bus");
    }
    _ioBus = ioBuses[0];

    //  Done
    _state = State::Connected;
}

void Processor::initialize() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    //  Done
    _state = State::Initialized;
}

void Processor::start() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    //  Done
    _state = State::Running;
}

void Processor::stop() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Running)
    {   //  OOPS! Can't
        return;
    }

    //  Done
    _state = State::Initialized;
}

void Processor::deinitialize() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    //  Done
    _state = State::Connected;
}

void Processor::disconnect() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    //  Disconnect from memory and I/O buses
    _memoryBus = nullptr;
    _ioBus = nullptr;

    //  Done
    _state = State::Constructed;
}

//////////
//  TODO
/*
void Processor::reset() noexcept
{
    edp::core::Component::reset();

    for (carl::UIntSize i = 0; i < _numCores; i++)
    {
        _cores[i]->reset();
    }
}
*/

#if 0
//////////
//  Operations (configuration)
void Processor::setClockFrequency(const edp::core::ClockFrequency & clockFrequency)
{
    require(getState() == State::Constructed);

    _clockFrequency = clockFrequency;
}

void Processor::setId(carl::UInt8 id)
{
    require(getState() == State::Constructed);

    _id = id;
}

void Processor::setByteOrder(carl::ByteOrder byteOrder)
{
    require(getState() == State::Constructed);

    _byteOrder = byteOrder;
}

void Processor::setRestartAddress(carl::UInt64 restartAddress)
{
    require(getState() == State::Constructed);

    if ((restartAddress & 0x00000003) != 0)
    {   //  OOPS! Not a valid instruction boundary!
        throw edp::core::ConfigurationException("Invalid restart address " + carl::toString(restartAddress, "%016X"));
    }
    _restartAddress = restartAddress;
}
#endif

Features Processor::features() const
{
    Features result;
    for (ProcessorCore * core : _cores)
    {
        result += core->features();
    }
    return result;
}

//  End of hadesvm-cereon/Processor.cpp
