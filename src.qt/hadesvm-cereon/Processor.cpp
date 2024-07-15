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
        //  Cores - as QList (for configuration stage)...
        _cores(),
        //  ...and as a plain C list - for runtime stage
        _coresAsArray(),
        _numCores(0)
{
}

Processor::~Processor() noexcept
{
}

//////////
//  hadesvm::core::Component
QString Processor::displayName() const
{
    return hadesvm::util::toString(_clockFrequency) + " " +
           componentType()->displayName() + " #" + hadesvm::util::toString(id(), "%02X");
}

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
    hadesvm::core::ClockFrequency clockFrequency = _clockFrequency;
    if (hadesvm::util::fromString(componentElement.attribute("ClockFrequency"), clockFrequency))
    {
        _clockFrequency = clockFrequency;
    }

    uint8_t id = _id;
    if (hadesvm::util::fromString(componentElement.attribute("Id"), "%X", id))
    {
        _id = id;
    }

    ByteOrder byteOrder = _byteOrder;
    if (hadesvm::util::fromString(componentElement.attribute("ByteOrder"), byteOrder))
    {
        _byteOrder = byteOrder;
    }

    bool canChangeByteOrder = _canChangeByteOrder;
    if (hadesvm::util::fromString(componentElement.attribute("CanChangeByteOrder"), canChangeByteOrder))
    {
        _canChangeByteOrder = canChangeByteOrder;
    }

    uint64_t restartAddress = _restartAddress;
    if (hadesvm::util::fromString(componentElement.attribute("RestartAddress"), "%X", restartAddress))
    {   //  TODO validate restartAddress - must be a multiple of 4
        _restartAddress = restartAddress;
    }

    bool isPrimaryProcessor = _isPrimaryProcessor;
    if (hadesvm::util::fromString(componentElement.attribute("IsPrimaryProcessor"), isPrimaryProcessor))
    {
        _isPrimaryProcessor = isPrimaryProcessor;
    }
}

hadesvm::core::ComponentEditor * Processor::createEditor()
{
    return new ProcessorEditor(this);
}

Processor::Ui * Processor::createUi()
{
    return nullptr;
}

//////////
//  hadesvm::core::Component (state management)
//  Must only be called from the QApplication's main thread (except state())
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

    //  Prepare a C-style array of cores - for faster iteration over
    if (_cores.count() == 0 || _cores.count() > 256)
    {
        throw hadesvm::core::VirtualApplianceException(
                    componentType()->displayName() + " must have between 1 and 256 cores");
    }
    //  ...TODO and a single primary core!!!

    for (qsizetype i = 0; i < _cores.count(); i++)
    {
        _cores[i]->reset();
        _coresAsArray[i] = _cores[i];
    }
    _numCores = _cores.count();

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

    _workerThread = new _WorkerThread(this);
    _workerThread->start();

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

    _workerThread->requestStop();
    _workerThread->wait(15 * 1000); //  wait 15 seconds...
    if (_workerThread->isRunning())
    {   //  ...then force-kill it as a last resort
        _workerThread->terminate();
        _workerThread->wait(ULONG_MAX);
    }
    delete _workerThread;
    _workerThread = nullptr;

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

    _numCores = 0;

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

void Processor::reset() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    for (size_t i = 0; i < _numCores; i++)
    {
        _coresAsArray[i]->reset();
    }
}

//////////
//  hadesvm::core::IClockedComponentAspect
void Processor::onClockTick() noexcept
{
    for (size_t i = 0; i < _numCores; i++)
    {
        _coresAsArray[i]->onClockTick();
    }
}

//////////
//  Operations (configuration)
void Processor::setClockFrequency(const hadesvm::core::ClockFrequency & clockFrequency)
{
    Q_ASSERT(_state == State::Constructed);

    _clockFrequency = clockFrequency;
}

void Processor::setId(uint8_t id)
{
    Q_ASSERT(_state == State::Constructed);

    _id = id;
}

void Processor::setByteOrder(ByteOrder byteOrder)
{
    Q_ASSERT(_state == State::Constructed);

    _byteOrder = byteOrder;
}

void Processor::setRestartAddress(uint64_t restartAddress)
{
    Q_ASSERT(_state == State::Constructed);

    if ((restartAddress & 0x00000003) != 0)
    {   //  OOPS! Not a valid instruction boundary!
        throw hadesvm::core::VirtualApplianceException("Invalid restart address " +
                                                       hadesvm::util::toString(restartAddress, "%016X"));
    }
    _restartAddress = restartAddress;
}

void Processor::setPrimaryProcessor(bool primaryProcessor)
{
    Q_ASSERT(_state == State::Constructed);

    _isPrimaryProcessor = primaryProcessor;
}

Features Processor::features() const
{
    Features result;
    for (ProcessorCore * core : _cores)
    {
        result += core->features();
    }
    return result;
}

//////////
//  Processor::_WorkerThread
void Processor::_WorkerThread::run()
{
    uint64_t requiredClockFrequencyHz = _processor->clockFrequency().toHz();
    uint64_t requiredNsPerTick = 1000000000 / requiredClockFrequencyHz;
    unsigned ticksBetweenDelayAdjustment = static_cast<unsigned>(qMin(qMax(1u, requiredClockFrequencyHz / 10), UINT_MAX));
    unsigned delayPerTickNs = 0;
    uint64_t accumulatedDelayNs = 0;

    QElapsedTimer elapsedTimer;
    while (!_stopRequested)
    {
        //  Execute a bunch of instructions...
        elapsedTimer.restart();
        for (unsigned n = 0; n < ticksBetweenDelayAdjustment; n++)
        {
            _processor->onClockTick();
            accumulatedDelayNs += delayPerTickNs;
        }
        uint64_t accumulatedDelayMs = accumulatedDelayNs / 1000000;
        accumulatedDelayNs -= 1000000 * accumulatedDelayMs;
        if (accumulatedDelayMs > 0)
        {
            msleep(static_cast<unsigned long>(accumulatedDelayMs));
        }

        qint64 idealNsElapsed = requiredNsPerTick * ticksBetweenDelayAdjustment;
        qint64 actualNsElapsed = elapsedTimer.nsecsElapsed();

        static int n = 0;
        if (n++ >= 100)
        {
            n = 0;
            //TODO kill off qDebug() << idealNsElapsed << " / " << actualNsElapsed << " / " << delayPerTickNs;

            qint64 actualNsPerTick = actualNsElapsed / ticksBetweenDelayAdjustment;;
            qint64 actualClockFrequencyHz = 1000000000 / actualNsPerTick;
            //TODO kill off qDebug() << "Processor Running at " << actualClockFrequencyHz << " Hz";

            _processor->virtualAppliance()->recordAchievedClockFrequency(
                _processor,
                hadesvm::core::ClockFrequency::hertz(actualClockFrequencyHz));
        }

        //qDebug() << actualNsElapsed;
        if (actualNsElapsed < idealNsElapsed)
        {   //  Going too fast!
            delayPerTickNs++;
        }
        else if (actualNsElapsed > idealNsElapsed && delayPerTickNs > 0)
        {   //  Going too slow!
            delayPerTickNs--;
        }
    }
}

//  End of hadesvm-cereon/Processor.cpp
