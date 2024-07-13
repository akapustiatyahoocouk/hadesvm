//
//  hadesvm-cereon/Fdc1.hpp
//
//  hadesvm-cereon FDC1 (floppy disk) subsystem
//
//////////

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The "FDC1 floppy drive" device
        class HADESVM_CEREON_PUBLIC Fdc1FloppyDrive : public hadesvm::core::Component
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Fdc1FloppyDrive)

            //////////
            //  Constants
        public:
            static const unsigned   DefaultChannel = 0;
            static const QString    DefaultDiskImagePath;

            //////////
            //  Types
        public:
            //  The type of Floppy Drive component
            class HADESVM_CEREON_PUBLIC Type final : public hadesvm::core::ComponentType
            {
                HADESVM_DECLARE_SINGLETON(Type);

                //////////
                //  hadesvm::util::StockObject
            public:
                virtual QString     mnemonic() const override;
                virtual QString     displayName() const override;

                //////////
                //  hadesvm::core::ComponentType
            public:
                virtual hadesvm::core::ComponentCategory *  category() const override;
                virtual bool        isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const override;
                virtual bool        isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const override;
                virtual Fdc1FloppyDrive *   createComponent() override;
            };

            //  The status of the drive's motor.
            enum class MotorStatus
            {
                Stopped,    //  The drive's motor is stopped.
                Starting,   //  The drive's motor is starting up.
                Spinning,   //  The drive's motor is running at full speed.
                Stopping    //  The drive's motor is slowing down.
            };

            //  The completion handler for asynchronous operations which have no "result" phase
            class HADESVM_CEREON_PUBLIC ICompletionHandler
            {
                //////////
                //  This is an interface
            public:
                virtual ~ICompletionHandler() noexcept = default;

                //////////
                //  Operations
            public:
                virtual void        onOperationCompleted(Fdc1FloppyDrive * floppyDrive) = 0;
            };

            //  The completion handler for asynchronous "calibrate" operation
            class HADESVM_CEREON_PUBLIC ICalibrateCompletionHandler
            {
                //////////
                //  This is an interface
            public:
                virtual ~ICalibrateCompletionHandler() noexcept = default;

                //////////
                //  Operations
            public:
                virtual void        onOperationCompleted(Fdc1FloppyDrive * floppyDrive, uint8_t commandStatusByte) = 0;
            };

            //  The completion handler for asynchronous "format track" operation
            class HADESVM_CEREON_PUBLIC IFormatTrackCompletionHandler
            {
                //////////
                //  This is an interface
            public:
                virtual ~IFormatTrackCompletionHandler() noexcept = default;

                //////////
                //  Operations
            public:
                virtual void        onOperationCompleted(Fdc1FloppyDrive * floppyDrive, uint8_t commandStatusByte) = 0;
            };

            //  The completion handler for asynchronous "get drive status" operation
            class HADESVM_CEREON_PUBLIC IGetDriveStatusCompletionHandler
            {
                //////////
                //  This is an interface
            public:
                virtual ~IGetDriveStatusCompletionHandler() noexcept = default;

                //////////
                //  Operations
            public:
                virtual void        onOperationCompleted(Fdc1FloppyDrive * floppyDrive, uint8_t commandStatusByte,
                                                         uint8_t statusByte1) = 0;
            };

            //  The completion handler for asynchronous "park" operation
            class HADESVM_CEREON_PUBLIC IParkCompletionHandler
            {
                //////////
                //  This is an interface
            public:
                virtual ~IParkCompletionHandler() noexcept = default;

                //////////
                //  Operations
            public:
                virtual void        onOperationCompleted(Fdc1FloppyDrive * floppyDrive, uint8_t commandStatusByte) = 0;
            };

            //  The completion handler for asynchronous "seek" operation
            class HADESVM_CEREON_PUBLIC ISeekCompletionHandler
            {
                //////////
                //  This is an interface
            public:
                virtual ~ISeekCompletionHandler() noexcept = default;

                //////////
                //  Operations
            public:
                virtual void        onOperationCompleted(Fdc1FloppyDrive * floppyDrive, uint8_t commandStatusByte) = 0;
            };

            //  The completion handler for asynchronous "read" operation
            class HADESVM_CEREON_PUBLIC IReadCompletionHandler
            {
                //////////
                //  This is an interface
            public:
                virtual ~IReadCompletionHandler() noexcept = default;

                //////////
                //  Operations
            public:
                virtual void        onOperationCompleted(Fdc1FloppyDrive * floppyDrive, uint8_t commandStatusByte,
                                                         const uint8_t * dataBytes, unsigned numDataBytes) = 0;
            };

            //  The completion handler for asynchronous "write" operation
            class HADESVM_CEREON_PUBLIC IWriteCompletionHandler
            {
                //////////
                //  This is an interface
            public:
                virtual ~IWriteCompletionHandler() noexcept = default;

                //////////
                //  Operations
            public:
                virtual void        onOperationCompleted(Fdc1FloppyDrive * floppyDrive, uint8_t commandStatusByte) = 0;
            };

            //////////
            //  Construction/destruction
        public:
            Fdc1FloppyDrive();
            virtual ~Fdc1FloppyDrive() noexcept;

            //////////
            //  hadesvm::core::Component
        public:
            virtual Type *      componentType() const override { return Type::instance(); }
            virtual QString     displayName() const override;
            virtual void        serialiseConfiguration(QDomElement componentElement) const override;
            virtual void        deserialiseConfiguration(QDomElement componentElement) override;
            virtual hadesvm::core::ComponentEditor *    createEditor(QWidget * parent) override;
            virtual Ui *        createUi() override;

            //////////
            //  hadesvm::core::Component (state management)
            //  Must only be called from the QApplication's main thread (except state())
        public:
            virtual State       state() const noexcept override;
            virtual void        connect() throws(hadesvm::core::VirtualApplianceException) override;
            virtual void        initialize() throws(hadesvm::core::VirtualApplianceException) override;
            virtual void        start() throws(hadesvm::core::VirtualApplianceException) override;
            virtual void        stop() noexcept override;
            virtual void        deinitialize() noexcept override;
            virtual void        disconnect() noexcept override;
            virtual void        reset() noexcept override;

            //////////
            //  Operations (configuration)
        public:
            unsigned            channel() const { return _channel; }
            void                setChannel(unsigned channel);
            QString             diskImagePath() const { return _diskImagePath; }
            void                setDiskImagePath(const QString & diskImagePath);
            bool                isMounted() const { return _mounted; }
            void                setMounted(bool mounted);

            //////////
            //  Operations (media control)
            //  Must only be called from the QApplication's main thread
        public:
            bool                mountImage(const QString & imageFilePath);
            void                unmountImage();

            //////////
            //  Operations (thread-safe status queries)
        public:
            MotorStatus         getMotorStatus() { return _motorStatus; }
            bool                isSeekInProgress();
            bool                isReadInProgress();
            bool                isWriteInProgress();

            bool                isFloppyImageMounted();

            //////////
            //  Operations (commands) - called on master clock thread TODO enforce
            //  The operations in this group start the corresponding activity within
            //  the floppy drive and then return ASAP. When the activity eventually
            //  finishes (successfully or with error), the specified "completionHandler"
            //  is called ON AN ARBITRARY THREAD (unless "completionHandler" is null).
        public:
            void                beginReset(ICompletionHandler * completionHandler);
            void                beginStartMotor(ICompletionHandler * completionHandler);
            void                beginStopMotor(ICompletionHandler * completionHandler);
            void                beginCalibrate(ICalibrateCompletionHandler * completionHandler);
            void                beginFormatTrack(IFormatTrackCompletionHandler * completionHandler);
            void                beginGetDriveStatus(IGetDriveStatusCompletionHandler * completionHandler);
            void                beginPark(IParkCompletionHandler * completionHandler);
            void                beginSeek(unsigned cylinder, ISeekCompletionHandler * completionHandler);
            void                beginRead(unsigned head, unsigned startSector, unsigned sectorsToRead,
                                          IReadCompletionHandler * completionHandler);
            void                beginWrite(unsigned head, unsigned startSector, unsigned sectorsToRead,
                                           const uint8_t * dataBytes, IWriteCompletionHandler * completionHandler);


            //////////
            //  Implementation
        private:
            State               _state = State::Constructed;

            //  Configuration
            unsigned            _channel;   //  0..3
            QString             _diskImagePath; //  invalid == none, else absolute path to "current" image
            bool                _mounted;

            static const unsigned   _ResetDelayMs = 50;
            static const unsigned   _MotorStartDelayMs = 300;
            static const unsigned   _MotorStopDelayMs = 300;
            static const unsigned   _CalibrateDelayMs = 250;
            static const unsigned   _SeekDelayMs = 10;    //  per track

            //  Current state
            enum class _OperationalState
            {
                _IdleNotMounted,
                _IdleMounted,
                _IdleSpinning,
                _CalibrateInProgress,
                _SeekInProgress,
                _ReadInProgress,
                _WriteInProgress
            };
            //  The "current state" is only modified by worker thread, but can
            //  be examined from an arbitrary thread
            std::atomic<_OperationalState>  _operationalState = _OperationalState::_IdleNotMounted;

            std::atomic<MotorStatus>    _motorStatus = MotorStatus::Stopped;
            std::atomic<unsigned>       _currentCylinder = 0;   //  0..79 or _CurrentCylinderParked
            static const unsigned       _CurrentCylinderParked = 0xFF;

            //  Currently "mounted" floppy image - these fields are assigned-to on
            //  UI thread (in response to user commands), but also queried on
            //  worker thread (which performs actual I/O).
            QFile *             _mountedFloppyImage = nullptr;  //  nullptr == no image is mounted
            bool                _mountedFloppyIsReadOnly = false;
            QMutex              _mountedFloppyGuard;

            //  Helpers
            bool                _mountImage(const QString & imageFilePath);
            void                _unmountImage();

            //  Command execution - runs on worker thread
            class _ResetCommand;
            class _StartMotorCommand;
            class _StopMotorCommand;
            class _CalibrateCommand;
            class _GetDriveStatusCommand;
            class _SeekCommand;
            class _ReadCommand;
            class _WriteCommand;

            void                _executeResetCommand(const _ResetCommand & command);
            void                _executeStartMotorCommand(const _StartMotorCommand & command);
            void                _executeStopMotorCommand(const _StopMotorCommand & command);
            void                _executeCalibrateCommand(const _CalibrateCommand & command);
            void                _executeGetDriveStatusCommand(const _GetDriveStatusCommand & command);
            void                _executeSeekCommand(const _SeekCommand & command);
            void                _executeReadCommand(const _ReadCommand & command);
            void                _executeWriteCommand(const _WriteCommand & command);

            //////////
            //  "Commands" sent to worker thread
            class _Command
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_Command)

                //////////
                //  Construction/destruction
            public:
                _Command() = default;
                virtual ~_Command() noexcept = default;
            };

            class _MountCommand : public _Command
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_MountCommand)

                //////////
                //  Construction/destruction
            public:
                explicit _MountCommand(const QString & imageFilePath) : _imageFilePath(imageFilePath) {}

                //////////
                //  Properties
            public:
                QString         _imageFilePath;
            };

            class _UnmountCommand : public _Command
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_UnmountCommand)

                //////////
                //  Construction/destruction
            public:
                _UnmountCommand() = default;
            };

            class _ResetCommand : public _Command
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_ResetCommand)

                //////////
                //  Construction/destruction
            public:
                explicit _ResetCommand(ICompletionHandler * completionHandler)
                    :   _completionHandler(completionHandler) {}

                //////////
                //  Properties
            public:
                ICompletionHandler *    _completionHandler;
            };

            class _StartMotorCommand : public _Command
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_StartMotorCommand)

                //////////
                //  Construction/destruction
            public:
                explicit _StartMotorCommand(ICompletionHandler * completionHandler)
                    :   _completionHandler(completionHandler) {}

                //////////
                //  Properties
            public:
                ICompletionHandler *    _completionHandler;
            };

            class _StopMotorCommand : public _Command
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_StopMotorCommand)

                //////////
                //  Construction/destruction
            public:
                explicit _StopMotorCommand(ICompletionHandler * completionHandler)
                    :   _completionHandler(completionHandler) {}

                //////////
                //  Properties
            public:
                ICompletionHandler *    _completionHandler;
            };

            class _CalibrateCommand : public _Command
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_CalibrateCommand)

                //////////
                //  Construction/destruction
            public:
                explicit _CalibrateCommand(ICalibrateCompletionHandler * completionHandler)
                    :   _completionHandler(completionHandler) {}

                //////////
                //  Properties
            public:
                ICalibrateCompletionHandler *   _completionHandler;
            };

            class _FormatTrackCommand : public _Command
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_FormatTrackCommand)

                //////////
                //  Construction/destruction
            public:
                explicit _FormatTrackCommand(IFormatTrackCompletionHandler * completionHandler)
                    :   _completionHandler(completionHandler) {}

                //////////
                //  Properties
            public:
                IFormatTrackCompletionHandler * _completionHandler;
            };

            class _GetDriveStatusCommand : public _Command
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_GetDriveStatusCommand)

                //////////
                //  Construction/destruction
            public:
                explicit _GetDriveStatusCommand(IGetDriveStatusCompletionHandler * completionHandler)
                    :   _completionHandler(completionHandler) {}

                //////////
                //  Properties
            public:
                IGetDriveStatusCompletionHandler * _completionHandler;
            };

            class _ParkCommand : public _Command
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_ParkCommand)

                //////////
                //  Construction/destruction
            public:
                explicit _ParkCommand(IParkCompletionHandler * completionHandler)
                    :   _completionHandler(completionHandler) {}

                //////////
                //  Properties
            public:
                IParkCompletionHandler * _completionHandler;
            };

            class _SeekCommand : public _Command
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_SeekCommand)

                //////////
                //  Construction/destruction
            public:
                _SeekCommand(unsigned cylinder, ISeekCompletionHandler * completionHandler)
                    :   _cylinder(cylinder),
                        _completionHandler(completionHandler) {}

                //////////
                //  Properties
            public:
                unsigned                _cylinder;
                ISeekCompletionHandler *_completionHandler;
            };

            class _ReadCommand : public _Command
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_ReadCommand)

                //////////
                //  Constants
            public:
                static const unsigned _MaxDataBytes = 512 * 18;

                //////////
                //  Construction/destruction
            public:
                _ReadCommand(unsigned head, unsigned startSector, unsigned sectorsToRead,
                             IReadCompletionHandler * completionHandler)
                    :   _head(head),
                        _startSector(startSector),
                        _sectorsToRead(sectorsToRead),
                        _completionHandler(completionHandler) {}

                //////////
                //  Properties
            public:
                unsigned        _head;
                unsigned        _startSector;
                unsigned        _sectorsToRead;
                IReadCompletionHandler *_completionHandler;
            };

            class _WriteCommand : public _Command
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_WriteCommand)

                //////////
                //  Constants
            public:
                static const unsigned _MaxDataBytes = 512 * 18;

                //////////
                //  Construction/destruction
            public:
                _WriteCommand(unsigned head, unsigned startSector, unsigned sectorsToWrite,
                              const uint8_t * dataBytes, IWriteCompletionHandler * completionHandler)
                    :   _head(head),
                        _startSector(startSector),
                        _sectorsToWrite(sectorsToWrite),
                        _completionHandler(completionHandler)
                {
                    Q_ASSERT(head <= 1);
                    Q_ASSERT(startSector <= 17);
                    Q_ASSERT(sectorsToWrite >= 1 && sectorsToWrite <= 18);
                    Q_ASSERT(startSector + sectorsToWrite <= 18);
                    Q_ASSERT(dataBytes != nullptr);

                    memcpy(_dataBytes, dataBytes, sectorsToWrite * 512);
                }

                //////////
                //  Properties
            public:
                unsigned        _head;
                unsigned        _startSector;
                unsigned        _sectorsToWrite;
                uint8_t         _dataBytes[_MaxDataBytes];
                IWriteCompletionHandler *   _completionHandler;
            };

            //////////
            //  Threads
        private:
            class HADESVM_CEREON_PUBLIC _WorkerThread : public QThread
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_WorkerThread)

                //////////
                //  Construction/destruction
            public:
                explicit _WorkerThread(Fdc1FloppyDrive * floppyDrive);
                virtual ~_WorkerThread() noexcept;

                //////////
                //  QThread
            protected:
                virtual void    run() override;

                //////////
                //  Operations
            public:
                void            requestStop() { _stopRequested = true; }
                void            enqueue(_Command * command);

                //////////
                //  Implementation
            private:
                Fdc1FloppyDrive *const  _floppyDrive;
                std::atomic<bool>   _stopRequested;

                hadesvm::util::InterthreadQueue<_Command*>  _pendingCommands; //  ...to process
            };
            _WorkerThread *     _workerThread = nullptr;
        };
        //////////
        //  The FDC1 floppy drive controller
        class HADESVM_CEREON_PUBLIC Fdc1Controller : public hadesvm::core::Component,
                                                     public virtual hadesvm::core::IClockedComponent,
                                                     public virtual IIoController
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Fdc1Controller)

            //////////
            //  Constants
        public:
            //  Command status byte flags
            class HADESVM_CEREON_PUBLIC Status final
            {
                HADESVM_UTILITY_CLASS(Status)

            public:
                static const uint8_t InvalidCommand   = 0x80;
                static const uint8_t InvalidParameter = 0x40;
                static const uint8_t NotReady         = 0x20;
                static const uint8_t NotWritable      = 0x10;
                static const uint8_t SeekError        = 0x08;
                static const uint8_t InvalidCylinder  = 0x04;
                static const uint8_t DataError        = 0x02;
                static const uint8_t Timeout          = 0x01;

                static const uint8_t NoError          = 0x00;
            };

            //  Configuration defaults
        public:
            static const uint16_t   DefaultStateAndControlPortAddress = 0x03F0;
            static const uint16_t   DefaultDataPortAddress = 0x03F1;
            static const uint16_t   DefaultInterruptMaskPortAddress = 0x03F2;
            static const hadesvm::core::ClockFrequency DefaultClockFrequency;

            //////////
            //  Types
        public:
            //  The type of FDC1 component
            class HADESVM_CEREON_PUBLIC Type final : public hadesvm::core::ComponentType
            {
                HADESVM_DECLARE_SINGLETON(Type);

                //////////
                //  hadesvm::util::StockObject
            public:
                virtual QString     mnemonic() const override;
                virtual QString     displayName() const override;

                //////////
                //  hadesvm::core::ComponentType
            public:
                virtual hadesvm::core::ComponentCategory *  category() const override;
                virtual bool        isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const override;
                virtual bool        isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const override;
                virtual Fdc1Controller *    createComponent() override;
            };

            //////////
            //  Construction/destruction
        public:
            Fdc1Controller();
            virtual ~Fdc1Controller() noexcept;

            //////////
            //  hadesvm::core::Component
        public:
            virtual Type *      componentType() const override { return Type::instance(); }
            virtual QString     displayName() const override;
            virtual void        serialiseConfiguration(QDomElement componentElement) const override;
            virtual void        deserialiseConfiguration(QDomElement componentElement) override;
            virtual hadesvm::core::ComponentEditor *    createEditor(QWidget * parent) override;
            virtual Ui *        createUi() override;

            //////////
            //  hadesvm::core::Component (state management)
            //  Must only be called from the QApplication's main thread (except state())
        public:
            virtual State       state() const noexcept override;
            virtual void        connect() throws(hadesvm::core::VirtualApplianceException) override;
            virtual void        initialize() throws(hadesvm::core::VirtualApplianceException) override;
            virtual void        start() throws(hadesvm::core::VirtualApplianceException) override;
            virtual void        stop() noexcept override;
            virtual void        deinitialize() noexcept override;
            virtual void        disconnect() noexcept override;
            virtual void        reset() noexcept override;

            //////////
            //  hadesvm::core::IClockedComponent
        public:
            virtual hadesvm::core::ClockFrequency
                                clockFrequency() const noexcept override { return _clockFrequency; }
            virtual void        onClockTick() noexcept override;

            //////////
            //  IIoController
        public:
            virtual IoPortList  ioPorts() override;

            //////////
            //  Operations (configuration)
        public:
            uint16_t            stateAndControlPortAddress() const { return _stateAndControlPortAddress; }
            void                setStateAndControlPortAddress(uint16_t stateAndControlPortAddress);
            uint16_t            dataPortAddress() const { return _dataPortAddress; }
            void                setDataPortAddress(uint16_t dataPortAddress);
            uint16_t            interruptMaskPortAddress() const { return _interruptMaskPortAddress; }
            void                setInterruptMaskPortAddress(uint16_t interruptMaskPortAddress);
            void                setClockFrequency(const hadesvm::core::ClockFrequency & clockFrequency);

            //////////
            //  Implementation
        private:
            State               _state = State::Constructed;

            //  Configuration
            uint16_t            _stateAndControlPortAddress;
            uint16_t            _dataPortAddress;
            uint16_t            _interruptMaskPortAddress;
            hadesvm::core::ClockFrequency   _clockFrequency;

            //  Runtime state - accessed from CPU worker threads (via I/O ports)
            //  and CMOS1 worker thread (directly)
            hadesvm::util::Spinlock _runtimeStateGuard;
            IoPortList          _ioPorts;   //  does not change during runtime

            Fdc1FloppyDrive *   _floppyDrives[4];   //  "nullptr"s for absent drives

            uint8_t             _interruptMask = 0;
            unsigned            _currentDeviceIndex = 0;

            enum class _OperationalState
            {
                _Idle,              //  controller is awaiting for the 1st byte of a command
                _AcceptingCommand,  //  controller has 1 or more command bytes and needs more parameters
                _ExecutingCommand,  //  controller is executing a command
                _Result             //  controller is providing result to CPU
            };
            std::atomic<_OperationalState> _operationalState = _OperationalState::_Idle;

            //  The buffer where command bytes are accumulated
            uint8_t             _commandBytes[18 * 512 + 1];
            unsigned            _numCommandBytes = 0;

            //  The buffer from which result bytes are served
            uint8_t             _resultBytes[18 * 512 + 1];
            unsigned            _numResultBytes = 0;
            unsigned            _nextResultByte = 0;

            //  The interrupt conditions currently pending
            uint8_t             _pendingInterruptConditions = 0;

            //////////
            //  Implementation helpers
        private:
            void                _performReset();
            static unsigned     _getCommandLength(uint8_t commandByte);
            void                _raiseBusyOffInterrupt();
            void                _raiseInputReadyOnInterrupt();
            void                _raiseOutputReadyOnInterrupt();
            void                _executeCommand();
            void                _executeGetDriveStatusCommand();
            void                _executeReadSectorCommand();
            void                _executeReadTrackCommand();
            void                _executeWriteSectorCommand();
            void                _executeSeekCommand();

            void                _completeGetDriveStatusCommand(uint8_t commandStatusByte, uint8_t statusByte1);
            void                _completeReadCommand(uint8_t commandStatusByte, const uint8_t * dataBytes, unsigned numDataBytes);
            void                _completeWriteCommand(uint8_t commandStatusByte);
            void                _completeSeekCommand(uint8_t commandStatusByte);

            //////////
            //  I/O ports
        private:
            class HADESVM_CEREON_PUBLIC _StateAndControlPort : public virtual IByteIoPort
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_StateAndControlPort)

                //////////
                //  Construction/destruction
            public:
                explicit _StateAndControlPort(Fdc1Controller * fdc1Controller)
                    :   _fdc1Controller(fdc1Controller) {}

                //////////
                //  IIoPort
            public:
                virtual uint16_t    address() const override { return _fdc1Controller->_stateAndControlPortAddress; }

                //////////
                //  IByteIoPort
            public:
                virtual uint8_t     readByte() override;
                virtual void        writeByte(uint8_t value) override;

                //////////
                //  Implementation
            private:
                Fdc1Controller *    _fdc1Controller;
            };
            _StateAndControlPort    _stateAndControlPort;

            class HADESVM_CEREON_PUBLIC _DataPort : public virtual IByteIoPort
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_DataPort)

                //////////
                //  Construction/destruction
            public:
                explicit _DataPort(Fdc1Controller * fdc1Controller)
                    :   _fdc1Controller(fdc1Controller) {}

                //////////
                //  IIoPort
            public:
                virtual uint16_t    address() const override { return _fdc1Controller->_dataPortAddress; }

                //////////
                //  IByteIoPort
            public:
                virtual uint8_t     readByte() override;
                virtual void        writeByte(uint8_t value) override;

                //////////
                //  Implementation
            private:
                Fdc1Controller *    _fdc1Controller;
            };
            _DataPort               _dataPort;

            class HADESVM_CEREON_PUBLIC _InterruptMaskPort : public virtual IByteIoPort
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_InterruptMaskPort)

                //////////
                //  Construction/destruction
            public:
                explicit _InterruptMaskPort(Fdc1Controller * fdc1Controller)
                    :   _fdc1Controller(fdc1Controller) {}

                //////////
                //  IIoPort
            public:
                virtual uint16_t    address() const override { return _fdc1Controller->_interruptMaskPortAddress; }

                //////////
                //  IByteIoPort
            public:
                virtual uint8_t     readByte() override;
                virtual void        writeByte(uint8_t value) override;

                //////////
                //  Implementation
            private:
                Fdc1Controller *    _fdc1Controller;
            };
            _InterruptMaskPort      _interruptMaskPort;

            //////////
            //  Command completion handlers
        private:
            class _GetDriveStatusCompletionHandler : public virtual Fdc1FloppyDrive::IGetDriveStatusCompletionHandler
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_GetDriveStatusCompletionHandler)

                //////////
                //  Construction/destruction
            public:
                explicit _GetDriveStatusCompletionHandler(Fdc1Controller * fdc1Controller)
                    :   _fdc1Controller(fdc1Controller) {}

                //////////
                //  FloppyDrive::IGetDriveStatusCompletionHandler
            public:
                virtual void            onOperationCompleted(Fdc1FloppyDrive * floppyDrive, uint8_t commandStatusByte,
                                                             uint8_t statusByte1) override;

                //////////
                //  Implementation
            private:
                Fdc1Controller *        _fdc1Controller;
            };
            _GetDriveStatusCompletionHandler    _getDriveStatusCompletionHandler;

            class _ReadCompletionHandler : public virtual Fdc1FloppyDrive::IReadCompletionHandler
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_ReadCompletionHandler)

                //////////
                //  Construction/destruction
            public:
                explicit _ReadCompletionHandler(Fdc1Controller * fdc1Controller)
                    :   _fdc1Controller(fdc1Controller) {}

                //////////
                //  FloppyDrive::IReadCompletionHandler
            public:
                virtual void            onOperationCompleted(Fdc1FloppyDrive * floppyDrive, uint8_t commandStatusByte,
                                                             const uint8_t * dataBytes, unsigned numDataBytes) override;

                //////////
                //  Implementation
            private:
                Fdc1Controller *        _fdc1Controller;
            };
            _ReadCompletionHandler      _readCompletionHandler;

            class _WriteCompletionHandler : public virtual Fdc1FloppyDrive::IWriteCompletionHandler
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_WriteCompletionHandler)

                    //////////
                    //  Construction/destruction
            public:
                explicit _WriteCompletionHandler(Fdc1Controller * fdc1Controller)
                    :   _fdc1Controller(fdc1Controller) {}

                //////////
                //  FloppyDrive::IWriteCompletionHandler
            public:
                virtual void            onOperationCompleted(Fdc1FloppyDrive * floppyDrive, uint8_t commandStatusByte) override;

                //////////
                //  Implementation
            private:
                Fdc1Controller *        _fdc1Controller;
            };
            _WriteCompletionHandler     _writeCompletionHandler;

            class _SeekCompletionHandler : public virtual Fdc1FloppyDrive::ISeekCompletionHandler
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_SeekCompletionHandler)

                //////////
                //  Construction/destruction
            public:
                explicit _SeekCompletionHandler(Fdc1Controller * fdc1Controller)
                    :   _fdc1Controller(fdc1Controller) {}

                //////////
                //  FloppyDrive::ISeekCompletionHandler
            public:
                virtual void            onOperationCompleted(Fdc1FloppyDrive * floppyDrive, uint8_t commandStatusByte) override;

                //////////
                //  Implementation
            private:
                Fdc1Controller *        _fdc1Controller;
            };
            _SeekCompletionHandler      _seekCompletionHandler;

            //////////
            //  Async results - a completion handler called when a FDD finishes an async
            //  operation created an "async result", which is then enqueued for processing
            //  on the next clock tick
        private:
            class _AsyncResult
            {   //  A generic "async result"
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_AsyncResult)

                //////////
                //  Construction/destruction
            public:
                _AsyncResult() = default;
                virtual ~_AsyncResult() noexcept = default;
            };
            std::atomic<_AsyncResult*> _asyncResult = nullptr;   //  nullptr == none pending

            class _GetDriveStatusAsyncResult : public _AsyncResult
            {   //  An "async result" from "get drive status" command
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_GetDriveStatusAsyncResult)

                //////////
                //  Construction/destruction
            public:
                _GetDriveStatusAsyncResult(uint8_t commandStatusByte_, uint8_t statusByte1_)
                    :   commandStatusByte(commandStatusByte_),
                        statusByte1(statusByte1_) {}

                //////////
                //  Properties
            public:
                uint8_t             commandStatusByte;
                uint8_t             statusByte1;
            };

            class _ReadAsyncResult : public _AsyncResult
            {   //  An "async result" from "read" command
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_ReadAsyncResult)

                //////////
                //  Construction/destruction
            public:
                _ReadAsyncResult(uint8_t commandStatusByte_, const uint8_t * dataBytes_,
                                 unsigned numDataBytes_)
                    :   commandStatusByte(commandStatusByte_),
                        numDataBytes(numDataBytes_)
                {
                    if (this->numDataBytes != 0)
                    {
                        memcpy(this->dataBytes, dataBytes_, this->numDataBytes);
                    }
                }

                //////////
                //  Properties
            public:
                uint8_t             commandStatusByte;
                uint8_t             dataBytes[512 * 18];
                unsigned            numDataBytes;
            };

            class _WriteAsyncResult : public _AsyncResult
            {   //  An "async result" from "write" command
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_WriteAsyncResult)

                //////////
                //  Construction/destruction
            public:
                explicit _WriteAsyncResult(uint8_t commandStatusByte_)
                    :   commandStatusByte(commandStatusByte_)
                {
                }

                //////////
                //  Properties
            public:
                uint8_t             commandStatusByte;
            };

            class _SeekAsyncResult : public _AsyncResult
            {   //  An "async result" from "seek" command
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_SeekAsyncResult)

                //////////
                //  Construction/destruction
            public:
                explicit _SeekAsyncResult(uint8_t commandStatusByte_)
                    :   commandStatusByte(commandStatusByte_) {}

                //////////
                //  Properties
            public:
                uint8_t             commandStatusByte;
            };
        };
    }
}

//  End of hadesvm-cereon/Fdc.hpp
