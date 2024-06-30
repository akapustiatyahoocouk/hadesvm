//
//  hadesvm-cereon/Kis1.hpp
//
//  The Cereon KIS1 controller
//
//////////

namespace hadesvm
{
    namespace cereon
    {
        enum class Kis1ScanCode : uint8_t
        {
            Invalid = 127   //  because 0 is used!
        };

        //////////
        //  Represents a Kis1 keyboard layout
        class HADESVM_CEREON_PUBLIC Kis1KeyboardLayout : public hadesvm::util::StockObject
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Kis1KeyboardLayout)

            //////////
            //  Types
        public:
            //  The 8-bit layout code
            enum class Code : uint8_t
            {
                Unknown = 0x00,
                UnitedKingdomExtended = 0x29,
                UnitedKingdom = 0x2C
            };

            //////////
            //  Construction/destruction
        public:
            Kis1KeyboardLayout();
            virtual ~Kis1KeyboardLayout();

            //////////
            //  Operations
        public:
            //  The KIS1-style keyboard layout code
            virtual Code        code() const = 0;

            //  Given the Qt's native scan code/virtual key from a QKeyEvent
            //  (as passed to keyPressEvent() and keyReleaseEvent()), works out
            //  which KIS1 scan code represents the corresponding key.
            //  Returns InvalidScanCode is no key translation is possible.
            virtual Kis1ScanCode    translateKey(uint32_t nativeScanCode, uint32_t nativeVirtualKey) = 0;

            //////////
            //  Registry
        public:
            //  Registers the specified KIS1 keyboard layout.
            //  Returns true on success, false on failure.
            static bool                     register(Kis1KeyboardLayout * kis1KeyboardLayout);

            //  Finds a registered KIS1 keyboard layout by mnemonic; returns
            //  nullptr if not found.
            static Kis1KeyboardLayout *     findByMnemonic(const QString & mnemonic);

            //  Returns an unordered list of all registered KIS1 keyboard layouts.
            static Kis1KeyboardLayoutList   all();

            //  Returns the "system" keyboard layout (as configured by host OS)
            static Kis1KeyboardLayout *     system();
        };

        //  The "UK extended" keyboard layout
        class HADESVM_CEREON_PUBLIC Kis1UnitedKingdomExtendedKeyboardLayout final : public Kis1KeyboardLayout
        {
            HADESVM_DECLARE_SINGLETON(Kis1UnitedKingdomExtendedKeyboardLayout)

            //////////
            //  hadesvm::util::StockObject
        public:
            virtual QString     mnemonic() const override;
            virtual QString     displayName() const override;

            //////////
            //  Kis1KeyboardLayout
        public:
            virtual Code        code() const override;
            virtual Kis1ScanCode    translateKey(uint32_t nativeScanCode, uint32_t nativeVirtualKey) override;

            //////////
            //  Implementation
        private:
            QMap<uint32_t, uint8_t> _kis1ScanCodesForNativeScanCodes;
        };

        //  The "UK" keyboard layout
        class HADESVM_CEREON_PUBLIC Kis1UnitedKingdomKeyboardLayout final : public Kis1KeyboardLayout
        {
            HADESVM_DECLARE_SINGLETON(Kis1UnitedKingdomKeyboardLayout)

            //////////
            //  hadesvm::util::StockObject
        public:
            virtual QString     mnemonic() const override;
            virtual QString     displayName() const override;

            //////////
            //  Kis1KeyboardLayout
        public:
            virtual Code        code() const override;
            virtual Kis1ScanCode    translateKey(uint32_t nativeScanCode, uint32_t nativeVirtualKey) override;

            //////////
            //  Implementation
        private:
            QMap<uint32_t, uint8_t> _kis1ScanCodesForNativeScanCodes;
        };

        //////////
        //  The Cereon KIS1 keyboard controller
        class HADESVM_CEREON_PUBLIC Kis1Controller : public hadesvm::core::Component,
                                                     public virtual hadesvm::core::IClockedComponent,
                                                     public virtual IIoController

        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Kis1Controller)

            //////////
            //  Constants
        public:
            static const uint16_t   DefaultStatePortAddress = 0x0280;
            static const uint16_t   DefaultCurrentDevicePortAddress = 0x0281;
            static const uint16_t   DefaultInterruptMaskPortAddress = 0x0282;
            static const uint16_t   DefaultInputSourcePortAddress = 0x0283;
            static const uint16_t   DefaultDeviceStatePortAddress = 0x0284;
            static const uint16_t   DefaultDataInPortAddress = 0x0285;
            static const uint16_t   DefaultLayoutPortAddress = 0x0286;
            static const unsigned   DefaultNumberOfCompartments = 1;
            static const hadesvm::core::ClockFrequency  DefaultClockFrequency;

            //////////
            //  Types
        public:
            //  The type of a Cereon KIS1 component
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
                virtual Kis1Controller *    createComponent() override;
            };

            //////////
            //  Construction/destruction
        public:
            Kis1Controller();
            virtual ~Kis1Controller() noexcept;

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
            uint16_t            currentDevicePortAddress() const { return _currentDevicePortAddress; }
            void                setCurrentDevicePortAddress(uint16_t currentDevicePortAddress);
            uint16_t            interruptMaskPortAddress() const { return _interruptMaskPortAddress; }
            void                setInterruptMaskPortAddress(uint16_t interruptMaskPortAddress);
            uint16_t            inputSourcePortAddress() const { return _inputSourcePortAddress; }
            void                setInputSourcePortAddress(uint16_t inputSourcePortAddress);
            uint16_t            deviceStatePortAddress() const { return _deviceStatePortAddress; }
            void                setDeviceStatePortAddress(uint16_t deviceStatePortAddress);
            uint16_t            dataInPortAddress() const { return _dataInPortAddress; }
            void                setDataInPortAddress(uint16_t dataInPortAddress);
            uint16_t            layoutPortAddress() const { return _layoutPortAddress; }
            void                setLayoutPortAddress(uint16_t  layoutPortAddress);
            unsigned            numberOfCompartments() const { return _numberOfCompartments; }
            void                setNumberOfCompartments(unsigned numberOfCompartments);
            void                setClockFrequency(const hadesvm::core::ClockFrequency & clockFrequency);

            //////////
            //  Implementation
        private:
            static const unsigned   _RegisterWriteDelay = 10;   //  clock ticks
            State               _state = State::Constructed;

            //  Configuration
            uint16_t            _statePortAddress;
            uint16_t            _currentDevicePortAddress;
            uint16_t            _interruptMaskPortAddress;
            uint16_t            _inputSourcePortAddress;
            uint16_t            _deviceStatePortAddress;
            uint16_t            _dataInPortAddress;
            uint16_t            _layoutPortAddress;
            unsigned            _numberOfCompartments;  //  1..256
            hadesvm::core::ClockFrequency   _clockFrequency;

            //  Runtime state - accessed from CPU worker threads (via I/O ports)
            //  and CMOS1 worker thread (directly)
            hadesvm::util::Spinlock _runtimeStateGuard;
            IoPortList          _ioPorts;

            //  All attached keyboards
            QList<Kis1Keyboard*>_keyboards;
            //  ...and a fast-lookup map (with compartment number as index)
            Kis1Keyboard *      _keyboardsMap[256]; //  set up by connect()

            //  State
            enum class _OperationalState
            {
                _Ready,                     //  Ready & no input pending
                _InputReady,                //  Ready & some input pending
                _ChangingCurrentDevice,     //  write to CURRENT_DEVICE is in progress
                _ChangingInterruptMask,     //  write to INTERRUPT_MASK is in progress
                _ChangingDeviceState        //  write to DEVICE_STATE is in progress
            };
            _OperationalState   _operationalState = _OperationalState::_Ready;
            unsigned            _timeout = 0;   //  clock cycles until no longer busy

            //  Interrupts
            static const uint8_t    _BusyOffInterrput = 0x01;
            static const uint8_t    _InputReadyOnInterrput = 0x02;

            bool                _busyOffInterrputPending = false;
            bool                _inputReadyOnInterrputPending = false;

            //  Controller registers
            uint8_t             _currentDevice = 0;
            uint8_t             _interruptMask = 0;
            uint8_t             _inputSource = 0;

            static const uint8_t    _BusyOffInterrputMask = 0x01;
            static const uint8_t    _InputReadyOnInterrputMask = 0x02;

            //  Helpers
            void                _raiseBusyOffInterrupt();
            void                _raiseInputReadyOnInterrupt();
            void                _synchronizeInterruptState();

            //////////
            //  I/O ports
        private:
            class HADESVM_CEREON_PUBLIC _StatePort : public virtual IByteIoPort
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_StatePort)

                //////////
                //  Construction/destruction
            public:
                explicit _StatePort(Kis1Controller * kis1Controller)
                    :   _kis1Controller(kis1Controller) {}

                //////////
                //  IIoPort
            public:
                virtual uint16_t    address() const override { return _kis1Controller->_statePortAddress; }
                virtual IoInterrupt*releasePendingIoInterrupt() override;

                //////////
                //  IByteIoPort
            public:
                virtual uint8_t readByte() throws(IoError) override;
                virtual void    writeByte(uint8_t value) throws(IoError) override;

                //////////
                //  Implementation
            private:
                Kis1Controller *const   _kis1Controller;
            };
            _StatePort          _statePort;

            class HADESVM_CEREON_PUBLIC _CurrentDevicePort : public virtual IByteIoPort
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_CurrentDevicePort)

                //////////
                //  Construction/destruction
            public:
                explicit _CurrentDevicePort(Kis1Controller * kis1Controller)
                    :   _kis1Controller(kis1Controller) {}

                //////////
                //  IIoPort
            public:
                virtual uint16_t    address() const override { return _kis1Controller->_currentDevicePortAddress; }

                //////////
                //  IByteIoPort
            public:
                virtual uint8_t readByte() throws(IoError) override;
                virtual void    writeByte(uint8_t value) throws(IoError) override;

                //////////
                //  Implementation
            private:
                Kis1Controller *const   _kis1Controller;
            };
            _CurrentDevicePort  _currentDevicePort;

            class HADESVM_CEREON_PUBLIC _InterruptMaskPort : public virtual IByteIoPort
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_InterruptMaskPort)

                //////////
                //  Construction/destruction
            public:
                explicit _InterruptMaskPort(Kis1Controller * kis1Controller)
                    :   _kis1Controller(kis1Controller) {}

                //////////
                //  IIoPort
            public:
                virtual uint16_t    address() const override { return _kis1Controller->_interruptMaskPortAddress; }

                //////////
                //  IByteIoPort
            public:
                virtual uint8_t readByte() throws(IoError) override;
                virtual void    writeByte(uint8_t value) throws(IoError) override;

                //////////
                //  Implementation
            private:
                Kis1Controller *const   _kis1Controller;
            };
            _InterruptMaskPort  _interruptMaskPort;

            class HADESVM_CEREON_PUBLIC _InputSourcePort : public virtual IByteIoPort
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_InputSourcePort)

                //////////
                //  Construction/destruction
            public:
                explicit _InputSourcePort(Kis1Controller * kis1Controller)
                    :   _kis1Controller(kis1Controller) {}

                //////////
                //  IIoPort
            public:
                virtual uint16_t    address() const override { return _kis1Controller->_inputSourcePortAddress; }

                //////////
                //  IByteIoPort
            public:
                virtual uint8_t readByte() throws(IoError) override;
                virtual void    writeByte(uint8_t value) throws(IoError) override;

                //////////
                //  Implementation
            private:
                Kis1Controller *const   _kis1Controller;
            };
            _InputSourcePort    _inputSourcePort;

            class HADESVM_CEREON_PUBLIC _DeviceStatePort : public virtual IByteIoPort
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_DeviceStatePort)

                //////////
                //  Construction/destruction
            public:
                explicit _DeviceStatePort(Kis1Controller * kis1Controller)
                    :   _kis1Controller(kis1Controller) {}

                //////////
                //  IIoPort
            public:
                virtual uint16_t    address() const override { return _kis1Controller->_deviceStatePortAddress; }

                //////////
                //  IByteIoPort
            public:
                virtual uint8_t readByte() throws(IoError) override;
                virtual void    writeByte(uint8_t value) throws(IoError) override;

                //////////
                //  Implementation
            private:
                Kis1Controller *const   _kis1Controller;
            };
            _DeviceStatePort    _deviceStatePort;

            class HADESVM_CEREON_PUBLIC _DataInPort : public virtual IByteIoPort
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_DataInPort)

                //////////
                //  Construction/destruction
            public:
                explicit _DataInPort(Kis1Controller * kis1Controller)
                    :   _kis1Controller(kis1Controller) {}

                //////////
                //  IIoPort
            public:
                virtual uint16_t    address() const override { return _kis1Controller->_dataInPortAddress; }

                //////////
                //  IByteIoPort
            public:
                virtual uint8_t readByte() throws(IoError) override;
                virtual void    writeByte(uint8_t value) throws(IoError) override;

                //////////
                //  Implementation
            private:
                Kis1Controller *const   _kis1Controller;
            };
            _DataInPort         _dataInPort;

            class HADESVM_CEREON_PUBLIC _LayoutPort : public virtual IByteIoPort
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_LayoutPort)

                //////////
                //  Construction/destruction
            public:
                explicit _LayoutPort(Kis1Controller * kis1Controller)
                    :   _kis1Controller(kis1Controller) {}

                //////////
                //  IIoPort
            public:
                virtual uint16_t    address() const override { return _kis1Controller->_layoutPortAddress; }

                //////////
                //  IByteIoPort
            public:
                virtual uint8_t readByte() throws(IoError) override;
                virtual void    writeByte(uint8_t value) throws(IoError) override;

                //////////
                //  Implementation
            private:
                Kis1Controller *const   _kis1Controller;
            };
            _LayoutPort         _layoutPort;
        };

        //////////
        //  The Cereon KIS1 keyboard
        class HADESVM_CEREON_PUBLIC Kis1Keyboard : public hadesvm::core::Component
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Kis1Keyboard)

            //////////
            //  Constants
        public:
            static const uint16_t   DefaultControllerStatePortAddress = Kis1Controller::DefaultStatePortAddress;
            static const uint8_t    DefaultCompartmentNumber = 0;

            //////////
            //  Types
        public:
            //  The type of a Cereon KIS1 keyboard
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
                virtual Kis1Keyboard *  createComponent() override;
            };

            //////////
            //  Construction/destruction
        public:
            Kis1Keyboard();
            virtual ~Kis1Keyboard() noexcept;

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
            Kis1KeyboardLayout *layout() const { return _layout; }
            void                setLayout(Kis1KeyboardLayout * layout);

            //////////
            //  Operations
        public:
            bool                isInputReady() const;
            uint8_t             readDeviceState();
            void                writeDeviceState(uint8_t deviceState);
            uint8_t             readDataIn();

            //  Called ON UI THREAD to handle user input
            void                handleKeyPressed(QKeyEvent * keyEvent);
            void                handleKeyReleased(QKeyEvent * keyEvent);

            //////////
            //  Implementation
        private:
            State               _state = State::Constructed;

            //  Configuration
            uint16_t            _controllerStatePortAddress;
            uint8_t             _controllerCompartmentNumber;
            Kis1KeyboardLayout *_layout;    //  never nullptr

            //  Keyboard registers
            bool                _numLockOn = false;     //  bit 0x02 of DEVICE_STATE
            bool                _capsLockOn = false;    //  bit 0x04 of DEVICE_STATE
            bool                _scrollLockOn = false;  //  bit 0x08 of DEVICE_STATE

            //  Incoming data
            QQueue<uint8_t>     _readyInputQueue;       //  written to on EDT read from on master clock thread
            mutable hadesvm::util::Spinlock _readyInputQueueGuard;  //  spinlock (for performance)
        };
    }
}

//  End of hadesvm-cereon/Kis1.hpp
