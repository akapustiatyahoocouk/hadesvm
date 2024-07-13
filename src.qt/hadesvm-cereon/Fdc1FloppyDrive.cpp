//
//  hadesvm-cereon/Fdc1FloppyDrive.cpp
//
//  hadesvm::cereon::Fdc1FloppyDrive class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
    using namespace hadesvm::cereon;

//////////
//  Constants
const QString   Fdc1FloppyDrive::DefaultDiskImagePath = "floppy.flp";

//////////
//  Construction/destruction
Fdc1FloppyDrive::Fdc1FloppyDrive()
    :   //  Configuration
        _channel(DefaultChannel),
        _diskImagePath(DefaultDiskImagePath),
        _mounted(false),
        //  Runtime state
        _mountedFloppyGuard()
{
}

Fdc1FloppyDrive::~Fdc1FloppyDrive() noexcept
{
}

//////////
//  hadesvm::core::Component
QString Fdc1FloppyDrive::displayName() const
{
    return componentType()->displayName() +
           " #" + hadesvm::util::toString(_channel);
}

void Fdc1FloppyDrive::serialiseConfiguration(QDomElement componentElement) const
{
    componentElement.setAttribute("Channel", hadesvm::util::toString(_channel));
    componentElement.setAttribute("DiskImagePath", _diskImagePath);
    componentElement.setAttribute("Mounted", hadesvm::util::toString(_mounted));
}

void Fdc1FloppyDrive::deserialiseConfiguration(QDomElement componentElement)
{
    unsigned channel;
    if (hadesvm::util::fromString(componentElement.attribute("Channel"), channel) &&
        /*channel >= 0 &&*/ channel <= 3)
    {
        _channel = channel;
    }

    _diskImagePath = componentElement.attribute("DiskImagePath");

    bool mounted;
    if (hadesvm::util::fromString(componentElement.attribute("Mounted"), mounted))
    {
        _mounted = mounted;
    }
}

hadesvm::core::ComponentEditor * Fdc1FloppyDrive::createEditor(QWidget * /*parent*/)
{
    return nullptr;
}

Fdc1FloppyDrive::Ui * Fdc1FloppyDrive::createUi()
{
    return nullptr;
}

//////////
//  hadesvm::core::Component (state management)
Fdc1FloppyDrive::State Fdc1FloppyDrive::state() const noexcept
{
    return _state;
}

void Fdc1FloppyDrive::connect() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Constructed)
    {   //  OOPS! Can't
        return;
    }

    //  If there isn't exactly 1 FDC1 controller, we have an error
    QList<Fdc1Controller*> fdc1Controllers = virtualAppliance()->componentsImplementing<Fdc1Controller>();
    if (fdc1Controllers.count() == 0)
    {
        throw hadesvm::core::VirtualApplianceException("TODO proper error message");
    }
    if (fdc1Controllers.count() > 1)
    {
        throw hadesvm::core::VirtualApplianceException("TODO proper error message");
    }

    _state = State::Connected;
}

void Fdc1FloppyDrive::initialize() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Initialized;
}

void Fdc1FloppyDrive::start() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    //  Mount the image ?
    _operationalState = _OperationalState::_IdleNotMounted;
    if (_mounted && !_diskImagePath.isEmpty())
    {   //  mount whe  floppy drive is "started"
        if (_mountImage(_diskImagePath))
        {
            _operationalState = _OperationalState::_IdleMounted;
        }
    }

    //  Start worker thread
    _workerThread = new _WorkerThread(this);
    _workerThread->start();

    _state = State::Running;
}

void Fdc1FloppyDrive::stop() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Running)
    {   //  OOPS! Can't
        return;
    }

    //  Stop the worker thread
    if (_workerThread != nullptr)
    {   //  Started - ask to stop now
        _workerThread->requestStop();
        _workerThread->wait(15 * 1000); //  wait 15 seconds...
        if (_workerThread->isRunning())
        {   //  ...then force-kill it as a last resort
            _workerThread->terminate();
            _workerThread->wait(ULONG_MAX);
        }
        delete _workerThread;
        _workerThread = nullptr;
    }

    //  If an image is mounted, un-mount it (but keep the "_diskImagePath"!)
    if (_mountedFloppyImage != nullptr)
    {
        _unmountImage();
    }

    _state = State::Initialized;
}

void Fdc1FloppyDrive::deinitialize() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Connected;
}

void Fdc1FloppyDrive::disconnect() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Constructed;
}

void Fdc1FloppyDrive::reset() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    //  TODO what?
}

//////////
//  Operations (configuration)
void Fdc1FloppyDrive::setChannel(unsigned channel)
{
    Q_ASSERT(_state == State::Constructed);

    if (/*channel >= 0 && */ channel <= 3)
    {
        _channel = channel;
    }
}

void Fdc1FloppyDrive::setDiskImagePath(const QString & diskImagePath)
{
    Q_ASSERT(_state == State::Constructed);

    _diskImagePath = diskImagePath;
}

void Fdc1FloppyDrive::setMounted(bool mounted)
{
    Q_ASSERT(_state == State::Constructed);

    _mounted = mounted;
}

//////////
//  Operations (media control)
bool Fdc1FloppyDrive::mountImage(const QString & imageFilePath)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_workerThread == nullptr)
    {   //  FDD is stopped - execute directly
        //  We allow this because floppy images may need to be mounted/unmounted
        //  while VM is suspended
        bool result = _mountImage(imageFilePath);
        _operationalState = result ? _OperationalState::_IdleMounted : _OperationalState::_IdleNotMounted;
        return result;
    }

    //  Send command to worker thread
    _workerThread->enqueue(new _MountCommand(imageFilePath));
    return true;
}

void Fdc1FloppyDrive::unmountImage()
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_workerThread == nullptr)
    {   //  FDD is stopped - execute directly
        //  We allow this because floppy images may need to be mounted/unmounted
        //  while VM is suspended
        _unmountImage();
        _operationalState = _OperationalState::_IdleNotMounted;
        return;
    }

    //  Send command to worker thread
    _workerThread->enqueue(new _UnmountCommand());
}

//////////
//  Operations (thread-safe status queries)
bool Fdc1FloppyDrive::isSeekInProgress()
{
    _OperationalState operationalState = _operationalState;
    return operationalState == _OperationalState::_CalibrateInProgress ||
           operationalState == _OperationalState::_SeekInProgress;
}

bool Fdc1FloppyDrive::isReadInProgress()
{
    return _operationalState == _OperationalState::_ReadInProgress;
}

bool Fdc1FloppyDrive::isWriteInProgress()
{
    return _operationalState == _OperationalState::_WriteInProgress;
}

bool Fdc1FloppyDrive::isFloppyImageMounted()
{
    return _mountedFloppyImage != nullptr;
}

//////////
//  Operations (commands)
void Fdc1FloppyDrive::beginReset(ICompletionHandler * completionHandler)
{
    //Q_ASSERT(carl::Thread::getCurrentThread() == this->getMasterClockThread());
    Q_ASSERT(_workerThread != nullptr);

    _workerThread->enqueue(new _ResetCommand(completionHandler));
}

void Fdc1FloppyDrive::beginStartMotor(ICompletionHandler * completionHandler)
{
    //Q_ASSERT(carl::Thread::getCurrentThread() == this->getMasterClockThread());
    Q_ASSERT(_workerThread != nullptr);

    _workerThread->enqueue(new _StartMotorCommand(completionHandler));
}

void Fdc1FloppyDrive::beginStopMotor(ICompletionHandler * completionHandler)
{
    //Q_ASSERT(carl::Thread::getCurrentThread() == this->getMasterClockThread());
    Q_ASSERT(_workerThread != nullptr);

    _workerThread->enqueue(new _StopMotorCommand(completionHandler));
}

void Fdc1FloppyDrive::beginCalibrate(ICalibrateCompletionHandler * completionHandler)
{
    //Q_ASSERT(carl::Thread::getCurrentThread() == this->getMasterClockThread());
    Q_ASSERT(_workerThread != nullptr);

    //  Enqueue the "calibrate" action
    _workerThread->enqueue(new _CalibrateCommand(completionHandler));
}

void Fdc1FloppyDrive::beginFormatTrack(IFormatTrackCompletionHandler * completionHandler)
{
    //Q_ASSERT(carl::Thread::getCurrentThread() == this->getMasterClockThread());
    Q_ASSERT(_workerThread != nullptr);

    //  Enqueue the "format track" action
    _workerThread->enqueue(new _FormatTrackCommand(completionHandler));
}

void Fdc1FloppyDrive::beginGetDriveStatus(IGetDriveStatusCompletionHandler * completionHandler)
{
    //Q_ASSERT(carl::Thread::getCurrentThread() == this->getMasterClockThread());
    Q_ASSERT(_workerThread != nullptr);

    //  Enqueue the "get drive status" action
    _workerThread->enqueue(new _GetDriveStatusCommand(completionHandler));
}

void Fdc1FloppyDrive::beginPark(IParkCompletionHandler * completionHandler)
{
    //Q_ASSERT(carl::Thread::getCurrentThread() == this->getMasterClockThread());
    Q_ASSERT(_workerThread != nullptr);

    //  Enqueue the "park" action
    _workerThread->enqueue(new _ParkCommand(completionHandler));
}

void Fdc1FloppyDrive::beginSeek(unsigned cylinder, ISeekCompletionHandler * completionHandler)
{
    //Q_ASSERT(carl::Thread::getCurrentThread() == this->getMasterClockThread());
    Q_ASSERT(_workerThread != nullptr);

    //  Enqueue the "seek" action
    _workerThread->enqueue(new _SeekCommand(cylinder, completionHandler));
}

void Fdc1FloppyDrive::beginRead(unsigned head, unsigned startSector, unsigned sectorsToRead,
                                IReadCompletionHandler * completionHandler)
{
    //Q_ASSERT(carl::Thread::getCurrentThread() == this->getMasterClockThread());
    Q_ASSERT(_workerThread != nullptr);

    //  Enqueue the "read" action
    _workerThread->enqueue(new _ReadCommand(head, startSector, sectorsToRead, completionHandler));
}

void Fdc1FloppyDrive::beginWrite(unsigned head, unsigned startSector, unsigned sectorsToRead,
                                 const uint8_t * dataBytes, IWriteCompletionHandler * completionHandler)
{
    Q_ASSERT(dataBytes != nullptr);
    //Q_ASSERT(carl::Thread::getCurrentThread() == this->getMasterClockThread());
    Q_ASSERT(_workerThread != nullptr);

    //  Enqueue the "write" action
    _workerThread->enqueue(new _WriteCommand(head, startSector, sectorsToRead, dataBytes, completionHandler));
}

//////////
//  Implementation helpers
bool Fdc1FloppyDrive::_mountImage(const QString & imageFilePath)
{
    QMutexLocker lock(&_mountedFloppyGuard);

    if (_mountedFloppyImage != nullptr && imageFilePath == _diskImagePath)
    {   //  Already mounted the same image
        return true;
    }
    //  Try to open the channel to the new image file
    QString absoluteImageFilePath = virtualAppliance()->toAbsolutePath(imageFilePath);
    QFile * raf = new QFile(absoluteImageFilePath);
    bool readOnly = false;
    //  Try mounting as r/w first
    raf->open(QIODeviceBase::OpenModeFlag::ReadWrite |
              QIODeviceBase::OpenModeFlag::ExistingOnly);
    if (!raf->isOpen())
    {   //  On error try mounting as r/o
        raf->open(QIODeviceBase::OpenModeFlag::ReadOnly |
                  QIODeviceBase::OpenModeFlag::ExistingOnly);
        if (raf->isOpen())
        {
            readOnly = true;
        }
        else
        {
            return false;
        }
    }

    //  Validate image format
    if (raf->size() != 80 * 2 * 18 * 512)    //  CYLS * HEADS * SPT * BPS
    {   //  OOPS! Can't!
        delete raf;
        return false;
    }

    //  If another image is already mounted, release it
    delete _mountedFloppyImage; //  "delete NULL" is safe

    //  Mount new image
    _mountedFloppyImage = raf;
    _mountedFloppyIsReadOnly = readOnly;

    //  Adjust drive state to match
    _diskImagePath = imageFilePath;

    //  Done
    return true;
}

void Fdc1FloppyDrive::_unmountImage()
{
    QMutexLocker lock(&_mountedFloppyGuard);

    if (_mountedFloppyImage != nullptr)
    {   //  Unmount image
        _mountedFloppyImage->close();
        delete _mountedFloppyImage;
        _mountedFloppyImage = nullptr;
        _mountedFloppyIsReadOnly = false;

        //  Adjust drive state to match
        _motorStatus = MotorStatus::Stopped;
    }
}

//////////
//  Command execution - runs on worker thread
void Fdc1FloppyDrive::_executeResetCommand(const _ResetCommand & command)
{
    switch (_operationalState)
    {
        case _OperationalState::_IdleNotMounted:
        case _OperationalState::_IdleMounted:
            //  reset, but don't change state
            //  TODO reset FDD
            break;
        case _OperationalState::_IdleSpinning:
            //  stop motor and then reset TODO properly
            _executeStopMotorCommand(_StopMotorCommand(nullptr));
            //  TODO reset FDD
            break;
        case _OperationalState::_CalibrateInProgress:
        case _OperationalState::_SeekInProgress:
        case _OperationalState::_ReadInProgress:
        case _OperationalState::_WriteInProgress:
        default:
            //  OOPS! Shouldn't happen!
            failure();
            break;
    }

    //  Inform completion handler
    if (command._completionHandler != nullptr)
    {
        command._completionHandler->onOperationCompleted(this);
    }
}

void Fdc1FloppyDrive::_executeStartMotorCommand(const _StartMotorCommand & command)
{
    if (_operationalState != _OperationalState::_IdleMounted)
    {   //  Not allowed
        if (command._completionHandler != nullptr)
        {
            command._completionHandler->onOperationCompleted(this);
        }
        return;
    }

    //  Simulate start delay & change state
    QThread::msleep(_MotorStartDelayMs);
    _operationalState = _OperationalState::_IdleSpinning;

    //  Inform completion handler
    if (command._completionHandler != nullptr)
    {
        command._completionHandler->onOperationCompleted(this);
    }
}

void Fdc1FloppyDrive::_executeStopMotorCommand(const _StopMotorCommand & command)
{
    if (_operationalState != _OperationalState::_IdleSpinning)
    {   //  Not allowed
        if (command._completionHandler != nullptr)
        {
            command._completionHandler->onOperationCompleted(this);
        }
        return;
    }

    //  Simulate stop delay & change state
    QThread::msleep(_MotorStopDelayMs);
    _operationalState = _OperationalState::_IdleMounted;

    //  Inform completion handler
    if (command._completionHandler != nullptr)
    {
        command._completionHandler->onOperationCompleted(this);
    }
}

void Fdc1FloppyDrive::_executeCalibrateCommand(const _CalibrateCommand & command)
{
    if (_operationalState != _OperationalState::_IdleSpinning)
    {   //  Not allowed in this state...
        if (command._completionHandler != nullptr)
        {   //  ...so inform the completion handler...
            command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::NotReady);
        }
        return; //  ...and abort
    }

    //  Simulate calibration delay - up to 79 "step" impulses
    _operationalState = _OperationalState::_CalibrateInProgress;
    QThread::msleep(_CalibrateDelayMs);
    _currentCylinder = 0;
    _operationalState = _OperationalState::_IdleSpinning;

    //  Success...
    if (command._completionHandler != nullptr)
    {   //  ...so inform the completion handler
        command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::NoError);
    }
}

void Fdc1FloppyDrive::_executeGetDriveStatusCommand(const _GetDriveStatusCommand & command)
{
    /*  TODO kill off
    if (_state != _State::_IdleSpinning)
    {   //  Not allowed in this state...
        if (command._completionHandler != nullptr)
        {   //  ...so inform the completion handler...
            command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::NotReady, 0);
        }
        return; //  ...and abort
    }
    */

    //  Execute the command
    uint8_t statusByte1 = static_cast<uint8_t>(0x80 | _currentCylinder);

    //  Success...
    if (command._completionHandler != nullptr)
    {   //  ...so inform the completion handler
        command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::NoError, statusByte1);
    }
}

void Fdc1FloppyDrive::_executeSeekCommand(const _SeekCommand & command)
{
    if (_operationalState != _OperationalState::_IdleSpinning)
    {   //  Not allowed in this state...
        if (command._completionHandler != nullptr)
        {   //  ...so inform the completion handler...
            command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::NotReady);
        }
        return; //  ...and abort
    }

    //  Validate parameters
    if (command._cylinder > 79)
    {   //  OOPS! Not allowed...
        if (command._completionHandler != nullptr)
        {   //  ...so inform the completion handler...
            command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::InvalidParameter);
        }
        return; //  ...and abort
    }

    //  Simulate seek delay
    _operationalState = _OperationalState::_SeekInProgress;
    int deltaCylinders = abs(static_cast<int>(command._cylinder) - static_cast<int>(_currentCylinder));
    QThread::msleep(deltaCylinders * _SeekDelayMs);
    _currentCylinder = command._cylinder;
    _operationalState = _OperationalState::_IdleSpinning;

    //  Success...
    if (command._completionHandler != nullptr)
    {   //  ...so inform the completion handler
        command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::NoError);
    }
}

void Fdc1FloppyDrive::_executeReadCommand(const _ReadCommand & command)
{
    if (_operationalState != _OperationalState::_IdleSpinning)
    {   //  Not allowed in this state...
        if (command._completionHandler != nullptr)
        {   //  ...so inform the completion handler...
            command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::NotReady, nullptr, 0);
        }
        return; //  ...and abort
    }
    if (_currentCylinder == _CurrentCylinderParked)
    {   //  OOPS! Can't read from there...
        if (command._completionHandler != nullptr)
        {   //  ...so inform the completion handler...
            command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::InvalidCylinder, nullptr, 0);
        }
        return; //  ...and abort
    }

    //  Validate parameters
    if (command._head > 1 ||
        command._startSector > 17 ||
        command._sectorsToRead == 0 || command._sectorsToRead > 18 ||
        command._startSector + command._sectorsToRead > 18)
    {   //  OOPS! Not allowed...
        if (command._completionHandler != nullptr)
        {   //  ...so inform the completion handler...
            command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::InvalidParameter, nullptr, 0);
        }
        return; //  ...and abort
    }

    //  Perform reading
    unsigned startLba = (_currentCylinder * 2 + command._head) * 18 + (command._startSector);
    unsigned bytesToRead = command._sectorsToRead * 512;
    uint8_t dataBytes[512 * 18];

    //  TODO kill off ? try
    {
        _mountedFloppyImage->seek(startLba * 512);
        if (_mountedFloppyImage->read(reinterpret_cast<char*>(dataBytes), bytesToRead) != bytesToRead)
        {   //  OOPS! Floppy image corrupt...
            if (command._completionHandler != nullptr)
            {   //  ...so inform the completion handler...
                command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::DataError, nullptr, 0);
            }
            return; //  ...and abort
        }
    }
    /*  TODO kill off ?
    catch (carl::IOException &)
    {   //  OOPS! I/O error...
        if (command._completionHandler != nullptr)
        {   //  ...so inform the completion handler...
            command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::DataError, nullptr, 0);
        }
        return; //  ...and abort
    }
    */
    //  TODO simulate rotational + read delay

    //  Success...
    if (command._completionHandler != nullptr)
    {   //  ...so inform the completion handler
        command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::NoError, dataBytes, bytesToRead);
    }
}

void Fdc1FloppyDrive::_executeWriteCommand(const _WriteCommand & command)
{
    if (_operationalState != _OperationalState::_IdleSpinning)
    {   //  Not allowed in this state...
        if (command._completionHandler != nullptr)
        {   //  ...so inform the completion handler...
            command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::NotReady);
        }
        return; //  ...and abort
    }
    if (_currentCylinder == _CurrentCylinderParked)
    {   //  OOPS! Can't write to there...
        if (command._completionHandler != nullptr)
        {   //  ...so inform the completion handler...
            command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::InvalidCylinder);
        }
        return; //  ...and abort
    }

    //  Validate parameters
    if (command._head > 1 ||
        command._startSector > 17 ||
        command._sectorsToWrite == 0 || command._sectorsToWrite > 18 ||
        command._startSector + command._sectorsToWrite > 18)
    {   //  OOPS! Not allowed...
        if (command._completionHandler != nullptr)
        {   //  ...so inform the completion handler...
            command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::InvalidParameter);
        }
        return; //  ...and abort
    }

    //  Perform writing
    unsigned startLba = (_currentCylinder * 2 + command._head) * 18 + (command._startSector);
    unsigned bytesToWrite = command._sectorsToWrite * 512;

    //  TODO kill off ? try
    {
        _mountedFloppyImage->seek(startLba * 512);
        _mountedFloppyImage->write(reinterpret_cast<const char*>(command._dataBytes), bytesToWrite);
    }
    /*  TODO kill off ?
    catch (carl::IOException &)
    {   //  OOPS! I/O error...
        if (command._completionHandler != nullptr)
        {   //  ...so inform the completion handler...
            command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::DataError);
        }
        return; //  ...and abort
    }
    */
    //  TODO simulate rotational + write delay

    //  Success...
    if (command._completionHandler != nullptr)
    {   //  ...so inform the completion handler
        command._completionHandler->onOperationCompleted(this, Fdc1Controller::Status::NoError);
    }
}

//////////
//  hadesvm::cereon::Fdc1FloppyDrive::Type
HADESVM_IMPLEMENT_SINGLETON(Fdc1FloppyDrive::Type)
Fdc1FloppyDrive::Type::Type() {}
Fdc1FloppyDrive::Type::~Type() {}

QString Fdc1FloppyDrive::Type::mnemonic() const
{
    return "CereonFdc1FloppyDrive";
}

QString Fdc1FloppyDrive::Type::displayName() const
{
    return "Cereon FDC1 floppy drive";
}

hadesvm::core::ComponentCategory * Fdc1FloppyDrive::Type::category() const
{
    return hadesvm::core::StandardComponentCategories::Storage;
}

bool Fdc1FloppyDrive::Type::isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const
{
    return architecture == CereonWorkstationArchitecture::instance();
}

bool Fdc1FloppyDrive::Type::isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const
{
    return type == hadesvm::core::VirtualMachineType::instance();
}

Fdc1FloppyDrive * Fdc1FloppyDrive::Type::createComponent()
{
    return new Fdc1FloppyDrive();
}

//////////
//  Fdc1FloppyDrive::_WorkerThread
Fdc1FloppyDrive::_WorkerThread::_WorkerThread(Fdc1FloppyDrive * floppyDrive)
    :   _floppyDrive(floppyDrive),
        _stopRequested(false),
        _pendingCommands()
{
}

Fdc1FloppyDrive::_WorkerThread::~_WorkerThread() noexcept
{
    //  Delete whatever commands are still stack in the input queue
    _Command * command;
    while (_pendingCommands.tryDequeue(0, command))
    {
        delete command;
    }
}

void Fdc1FloppyDrive::_WorkerThread::run()
{
    _Command * command;

    const long WaitChunkMs = 500;

    while (!_stopRequested)
    {
        //  Wait for a command to arrive
        if (!_pendingCommands.tryDequeue(WaitChunkMs, command))
        {   //  Nothing - keep waiting
            continue;
        }
        //  Handle the command
        if (_MountCommand * mountCommand = dynamic_cast<_MountCommand*>(command))
        {
            _floppyDrive->_operationalState =
                _floppyDrive->_mountImage(mountCommand->_imageFilePath) ?
                    _OperationalState::_IdleMounted :
                    _OperationalState::_IdleNotMounted;
        }
        else if (/*_UnmountCommand * unmountCommand =*/ dynamic_cast<_UnmountCommand*>(command))
        {
            _floppyDrive->_unmountImage();
        }
        else if (_ResetCommand * resetCommand = dynamic_cast<_ResetCommand*>(command))
        {
            _floppyDrive->_executeResetCommand(*resetCommand);
        }
        else if (_StartMotorCommand * startMotorCommand = dynamic_cast<_StartMotorCommand*>(command))
        {
            _floppyDrive->_executeStartMotorCommand(*startMotorCommand);
        }
        else if (_StopMotorCommand * stopMotorCommand = dynamic_cast<_StopMotorCommand*>(command))
        {
            _floppyDrive->_executeStopMotorCommand(*stopMotorCommand);
        }
        //  "true" commands
        else if (_CalibrateCommand * calibrateCommand = dynamic_cast<_CalibrateCommand*>(command))
        {
            _floppyDrive->_executeCalibrateCommand(*calibrateCommand);
        }
        else if (_GetDriveStatusCommand * getDriveStatusCommand = dynamic_cast<_GetDriveStatusCommand*>(command))
        {
            _floppyDrive->_executeGetDriveStatusCommand(*getDriveStatusCommand);
        }
        else if (_SeekCommand * seekCommand = dynamic_cast<_SeekCommand*>(command))
        {
            _floppyDrive->_executeSeekCommand(*seekCommand);
        }
        else if (_ReadCommand * readCommand = dynamic_cast<_ReadCommand*>(command))
        {
            _floppyDrive->_executeReadCommand(*readCommand);
        }
        else if (_WriteCommand * writeCommand = dynamic_cast<_WriteCommand *>(command))
        {
            _floppyDrive->_executeWriteCommand(*writeCommand);
        }
        else
        {
            failure();
        }
        delete command;

        /*  TODO kill off
        //  Got one - the FDD will be busy running it
        _floppyDrive->_busy = true;
        //  ...right after we make sure the FDD state correctly reflects
        //  the state of the currently mounted image
        {
            carl::CriticalSectionLock lock(_floppyDrive->_mountedFloppyGuard);
            if (_floppyDrive->_mountedFloppyImage == nullptr)
            {   //  No image is mounted!
                _floppyDrive->_doorClosed = false;
                _floppyDrive->_seekInProgress = false;
                _floppyDrive->_readInProgress = false;
                _floppyDrive->_writeInProgress = false;
            }
        }
        //  Run this "activity" (and whatever else is queued in the process)
        for (; ; )
        {
            //  Run & delete
            activity->run();    //  TODO do we need to handle exceptions ?
            delete activity;
            //  More ?
            if (!_pendingActivities.tryDequeue(activity, carl::TimeSpan::Empty))
            {   //  Nothing
                break;
            }
        }
        //  Now FDD is no longer busy
        _floppyDrive->_busy = false;
        */
    }
}

void Fdc1FloppyDrive::_WorkerThread::enqueue(_Command * command)
{
    _pendingCommands.enqueue(command);
}

//  End of hadesvm-cereon/Fdc1FloppyDrive.cpp
