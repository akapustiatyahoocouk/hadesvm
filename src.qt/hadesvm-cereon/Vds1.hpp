//
//  hadesvm-cereon/Vds1.hpp
//
//  The Cereon VDS1 subsystem
//
//////////

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The Cereon VDS1 video controller
        class HADESVM_CEREON_PUBLIC Vds1Controller : public hadesvm::core::Component,
                                                     public virtual hadesvm::core::IClockedComponent,
                                                     public virtual IIoController

        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Vds1Controller)

            friend class Vds1Display;
            friend class Vds1DisplayWidget;

            //////////
            //  Constants
        public:
            static const uint16_t   DefaultStatePortAddress;
            static const uint16_t   DefaultCommandPortAddress;
            static const uint16_t   DefaultDataPortAddress;
            static const unsigned   DefaultNumberOfCompartments;
            static const hadesvm::core::ClockFrequency DefaultClockFrequency;

            //////////
            //  Types
        public:
            //  The type of a Cereon VDS1 component
            class HADESVM_CEREON_PUBLIC Type : public hadesvm::core::ComponentType
            {
                HADESVM_DECLARE_SINGLETON(Type);

                //////////
                //  hadesvm::util::StockObject
            public:
                virtual QString mnemonic() const override;
                virtual QString displayName() const override;

                //////////
                //  hadesvm::core::ComponentType
            public:
                virtual hadesvm::core::ComponentCategory *  category() const override;
                virtual bool    suspendable() const override;
                virtual bool    isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const override;
                virtual bool    isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const override;
                virtual Vds1Controller *    createComponent() override;
            };

            //////////
            //  Construction/destruction
        public:
            Vds1Controller();   //  Constructs a VDS1 with the default configuration
            virtual ~Vds1Controller() noexcept;

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
            uint16_t            statePortAddress() const { return _statePortAddress; }
            void                setStatePortAddress(uint16_t statePortAddress);
            uint16_t            commandPortAddress() const { return _commandPortAddress; }
            void                setCommandPortAddress(uint16_t commandPortAddress);
            uint16_t            dataPortAddress() const { return _dataPortAddress; }
            void                setDataPortAddress(uint16_t dataPortAddress);
            unsigned            numberOfCompartments() const { return _numberOfCompartments; }
            void                setNumberOfCompartments(unsigned numberOfCompartments);
            void                setClockFrequency(const hadesvm::core::ClockFrequency & clockFrequency);

            //////////
            //  Implementation
        private:
            State               _state = State::Constructed;

            //  Configuration
            uint16_t            _statePortAddress;
            uint16_t            _commandPortAddress;
            uint16_t            _dataPortAddress;
            unsigned            _numberOfCompartments;  //  1..256
            hadesvm::core::ClockFrequency   _clockFrequency;

            //  Runtime state - accessed from CPU worker threads (via I/O ports)
            //  and CMOS1 worker thread (directly)
            hadesvm::util::Spinlock _runtimeStateGuard;
            IoPortList          _ioPorts;   //  does not change during runtime
            std::atomic<uint8_t>_interruptMask; //  private register

            class HADESVM_CEREON_PUBLIC _Compartment;

            //  The video signal emitted for a given compartment
            class HADESVM_CEREON_PUBLIC _VideoSignal final /*   TODO uncomment ? : public hadesvm::core::IRasterVideoSignal */
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_VideoSignal)

                //////////
                //  Constants
            public:
                static const unsigned   PixelWidth = 512;
                static const unsigned   PixelHeight = 384;

                //////////
                //  Construction/destruction
            public:
                explicit _VideoSignal(_Compartment * compartment);

                //////////
                //  hadesvm::core::IRasterVideoSignal   TODO uncomment
            public:
                virtual unsigned    getPixelWidth() const { return PixelWidth; }
                virtual unsigned    getPixelHeight() const { return PixelHeight; }
                virtual unsigned    getAspectRatioX() const { return 4; }
                virtual unsigned    getAspectRatioY() const { return 3; }
                virtual QRgb        getPixelColor(unsigned x, unsigned y) const;
                virtual void        getPixelColors(unsigned x, unsigned y, unsigned width, unsigned height, QRgb * buffer) const;

                //////////
                //  Operations
            public:
                void            setPixelColor(unsigned x, unsigned y, QRgb color);
                void            clear(QRgb color = qRgb(0, 0, 0));

                //////////
                //  Implementation
            private:
                _Compartment *  _compartment;
                QRgb            _pixels[PixelHeight][PixelWidth];  //  [y][x]

                //  Helpers
                void            _actualize();
            };

            class HADESVM_CEREON_PUBLIC _Compartment final
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_Compartment)

                friend class _VideoSignal;

                //////////
                //  Construction/destruction
            public:
                explicit _Compartment(uint8_t number);

                //////////
                //  Operations
            public:
                uint8_t         number() const { return _number; }
                void            attachDisplay(Vds1Display * display);
                void            detachDisplay();
                Vds1Display *   attachedDisplay() const { return _display; }

                uint8_t         videoMode() const { return _videoMode; }
                void            setVideoMode(uint8_t videoMode);
                uint8_t         getPage() const { return _page; }
                uint16_t        source() const { return _source; }
                void            setSource(uint16_t source);
                uint16_t        destination() const { return _destination; }
                void            setDestination(uint16_t destination);
                uint16_t        length() const { return _length; }
                void            setLength(uint16_t length);

                void            reset();    //  compartment reset (or part of VDS1 reset)

                uint8_t         read();
                void            write(uint8_t value);
                void            writeAndAdvance(uint8_t value);
                void            move();

                //////////
                //  Implementation
            private:
                const uint8_t   _number;        //  ...of this compartment
                uint8_t         _videoMode;     //  private register
                uint8_t         _page;          //  private register
                uint16_t        _source;        //  private register
                uint16_t        _destination;   //  private register
                uint16_t        _length;        //  private register

                uint8_t         _videoMemory[65536];

                Vds1Display *   _display;       //  attached to this compartment, nullptr == none
                _VideoSignal    _videoSignal;   //  ...generated for this compartment
                bool            _videoSignalNeedsRefreshing = true;

                //  Resources
                const QRgb      _colorTable16[16];

                //  Helpers
                void            _regenerateVideoSignal();
                void            _regenerateText32x24VideoSignal();
                void            _regenerateText64x24VideoSignal();
            };

            QList<_Compartment*>    _allCompartments;   //  array of 1..256 items; does not change during runtime
            _Compartment *      _currentCompartment; //  points to one of _allCompartments items

            enum class _OperationalState
            {
                _Ready,             //  waiting for 1st byte of command
                _AcceptingCommand,  //  waiting for 2nd, etc. bytes of command
                _ExecutingCommand,  //  command is being executed
                _ProvidingResult    //  waiting for 1 or more result bytes to be read
            };
            std::atomic<_OperationalState>  _operationalState = _OperationalState::_Ready;

            QList<uint8_t>          _commandBytes;  //  command is assembled in this buffer
            QList<uint8_t>          _resultBytes;   //  ...to be read by CPU when _state == _ProvidingResult
            std::atomic<unsigned>   _executeDelay = 0;  //  clock ticks to spend in _ExecutingCommand state

            static const uint8_t    _ResetCommand = 0x00;
            static const uint8_t    _ResetCompartmentCommand = 0x01;
            static const uint8_t    _GetCapacityCommand = 0x02;
            static const uint8_t    _GetInterruptMaskCommand = 0x03;
            static const uint8_t    _SetInterruptMaskCommand = 0x04;
            static const uint8_t    _SenseDisplayCommand = 0x05;
            static const uint8_t    _GetCurrentCompartmentCommand = 0x10;
            static const uint8_t    _SetCurrentCompartmentCommand = 0x11;
            static const uint8_t    _GetVideoModeCommand = 0x20;
            static const uint8_t    _SetVideoModeCommand = 0x21;
            static const uint8_t    _GetVideoPageCommand = 0x22;
            static const uint8_t    _SetVideoPageCommand = 0x23;
            static const uint8_t    _GetSourceAddressCommand = 0x30;
            static const uint8_t    _SetSourceAddressCommand = 0x31;
            static const uint8_t    _GetDestinationAddressCommand = 0x32;
            static const uint8_t    _SetDestinationAddressCommand = 0x33;
            static const uint8_t    _GetLengthCommand = 0x34;
            static const uint8_t    _SetLengthCommand = 0x35;
            static const uint8_t    _ReadCommand = 0x40;
            static const uint8_t    _ReadAndAdvanceCommand = 0x41;
            static const uint8_t    _WriteCommand = 0x50;
            static const uint8_t    _WriteAndAdvanceCommand = 0x51;
            static const uint8_t    _WriteWithMaskCommand = 0x52;
            static const uint8_t    _WriteWithMaskAndAdvanceCommand = 0x53;
            static const uint8_t    _FillCommand = 0x54;
            static const uint8_t    _FillWithMaskCommand = 0x55;
            static const uint8_t    _MoveCommand = 0x60;
            static const uint8_t    _MoveBackwardCommand = 0x61;
            static const uint8_t    _MoveWithMaskCommand = 0x62;
            static const uint8_t    _MoveBackwardWithMaskCommand = 0x63;

            static const uint8_t    _Text32x24VideoMode = 0;
            static const uint8_t    _Text64x24VideoMode = 1;
            static const uint8_t    _Text40x24VideoMode = 2;
            static const uint8_t    _Text80x24VideoMode = 3;
            static const uint8_t    _Graph256x192VideoMode = 4;
            static const uint8_t    _Graph512x192VideoMode = 5;
            static const uint8_t    _Graph512x384VideoMode = 6;

            //  Helpers
            static unsigned     _getCommandLength(uint8_t commandByte);
            void                _executeCommand();

            void                _executeResetCommand();
            void                _executeResetCompartmentCommand();
            void                _executeGetCapacityCommand();
            void                _executeGetInterruptMaskCommand();
            void                _executeSetInterruptMaskCommand();
            void                _executeSenseDisplayCommand();
            void                _executeGetCurrentCompartmentCommand();
            void                _executeSetCurrentCompartmentCommand();
            void                _executeGetVideoModeCommand();
            void                _executeSetVideoModeCommand();
            void                _executeGetVideoPageCommand();
            void                _executeSetVideoPageCommand();
            void                _executeGetSourceAddressCommand();
            void                _executeSetSourceAddressCommand();
            void                _executeGetDestinationAddressCommand();
            void                _executeSetDestinationAddressCommand();
            void                _executeGetLengthCommand();
            void                _executeSetLengthCommand();
            void                _executeReadCommand();
            void                _executeReadAndAdvanceCommand();
            void                _executeWriteCommand();
            void                _executeWriteAndAdvanceCommand();
            void                _executeWriteWithMaskCommand();
            void                _executeWriteWithMaskAndAdvanceCommand();
            void                _executeFillCommand();
            void                _executeFillWithMaskCommand();
            void                _executeMoveCommand();
            void                _executeMoveBackwardCommand();
            void                _executeMoveWithMaskCommand();
            void                _executeMoveBackwardWithMaskCommand();

            void                _finishCommandExecution();
            void                _finishCommandExecution(uint8_t resultByte1);
            void                _finishCommandExecution(uint8_t resultByte1, uint8_t resultByte2);

            //////////
            //  I/O ports
        private:
            class HADESVM_CEREON_PUBLIC _StatePort : public virtual IByteIoPort
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_StatePort)

                //////////
                //  Construction/destruction
            public:
                explicit _StatePort(Vds1Controller * vds1Controller)
                    :   _vds1Controller(vds1Controller) {}

                //////////
                //  IIoPort
            public:
                virtual uint16_t    address() const override { return _vds1Controller->_statePortAddress; }

                //////////
                //  IByteIoPort
            public:
                virtual bool        readByte(uint8_t & value) override;
                virtual bool        writeByte(uint8_t value) override;

                //////////
                //  Implementation
            private:
                Vds1Controller *            _vds1Controller;
            };
            _StatePort                      _statePort;

            class HADESVM_CEREON_PUBLIC _CommandPort : public virtual IByteIoPort
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_CommandPort)

                //////////
                //  Construction/destruction
            public:
                explicit _CommandPort(Vds1Controller * vds1Controller)
                    :   _vds1Controller(vds1Controller) {}

                //////////
                //  IIoPort
            public:
                virtual uint16_t    address() const override { return _vds1Controller->_commandPortAddress; }

                //////////
                //  IByteIoPort
            public:
                virtual bool        readByte(uint8_t & value) override;
                virtual bool        writeByte(uint8_t value) override;

                //////////
                //  Implementation
            private:
                Vds1Controller *            _vds1Controller;
            };
            _CommandPort                    _commandPort;

            class HADESVM_CEREON_PUBLIC _DataPort : public virtual IByteIoPort
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_DataPort)

                //////////
                //  Construction/destruction
            public:
                explicit _DataPort(Vds1Controller * vds1Controller)
                    :   _vds1Controller(vds1Controller) {}

                //////////
                //  IIoPort
            public:
                virtual uint16_t    address() const override { return _vds1Controller->_dataPortAddress; }

                //////////
                //  IByteIoPort
            public:
                virtual bool        readByte(uint8_t & value) override;
                virtual bool        writeByte(uint8_t value) override;

                //////////
                //  Implementation
            private:
                Vds1Controller *            _vds1Controller;
            };
            _DataPort                       _dataPort;
        };

        //////////
        //  The Cereon VDS1 display unit
        class HADESVM_CEREON_PUBLIC Vds1Display final : public hadesvm::core::Component
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Vds1Display)

            friend class Vds1Controller;
            friend class Vds1DisplayWidget;

            //////////
            //  Constants
        public:
            static const uint16_t   DefaultControllerStatePortAddress;
            static const uint8_t    DefaultControllerCompartmentNumber;

            //////////
            //  Types
        public:
            //  The type of a Cereon VDS1 display
            class HADESVM_CEREON_PUBLIC Type final : public hadesvm::core::ComponentType
            {
                HADESVM_DECLARE_SINGLETON(Type);

                //////////
                //  hadesvm::util::StockObject
            public:
                virtual QString mnemonic() const override;
                virtual QString displayName() const override;

                //////////
                //  hadesvm::core::ComponentType
            public:
                virtual hadesvm::core::ComponentCategory *  category() const override;
                virtual bool    suspendable() const override;
                virtual bool    isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const override;
                virtual bool    isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const override;
                virtual Vds1Display *   createComponent() override;
            };

            //  The UI of a Cereon VDS1 display
            class HADESVM_CEREON_PUBLIC Ui final : public hadesvm::core::Component::Ui
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Ui)

                //////////
                //  Construction/destruction
            public:
                explicit Ui(Vds1Display * vds1Display);
                virtual ~Ui();

                //////////
                //  hadesvm::core::Component::Ui
            public:
                virtual Vds1Display *   component() const override { return _vds1Display; }
                virtual hadesvm::core::DisplayWidgetList    displayWidgets() const override { return _displayWidgets; }

                //////////
                //  Implementation
            private:
                Vds1Display *const  _vds1Display;

                Vds1DisplayWidget *const            _vds1DisplayWidget;
                hadesvm::core::DisplayWidgetList    _displayWidgets;
            };

            //  A visual representation of a Vds1Display in the UI
            enum class StretchMode
            {
                NoStretch,          //  1:1
                IntegralStretch,    //  stretch by integral factor, keeping aspect ratio
                Fill                //  fill the entire DisplayWidget
            };

            //////////
            //  Construction/destruction
        public:
            Vds1Display();
            virtual ~Vds1Display() noexcept;

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

            //////////
            //  Operations (configuration)
        public:
            uint16_t            controllerStatePortAddress() const { return _controllerStatePortAddress; }
            void                setControllerStatePortAddress(uint16_t controllerStatePortAddress);
            uint8_t             controllerCompartmentNumber() const { return _controllerCompartmentNumber; }
            void                setControllerCompartmentNumber(uint8_t controllerCompartmentNumber);
            StretchMode         stretchMode() const { return _stretchMode; }
            void                setStretchMode(StretchMode stretchMode);

            //////////
            //  Implementation
        private:
            State               _state = State::Constructed;

            //  Configuration
            uint16_t            _controllerStatePortAddress;
            uint8_t             _controllerCompartmentNumber;
            StretchMode         _stretchMode = StretchMode::Fill;

            //  Runtime state
            //  TODO what ? Kis1Keyboard *              _kis1Keyboard = nullptr;
            Vds1Controller::_VideoSignal *  _videoSignal = nullptr;
        };
    }
}

//  End of hadesvm-cereon/Vds1.hpp
