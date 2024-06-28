//
//  hadesvm-cereon/Io.hpp
//
//  I/O subsystem
//
//////////

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  An I/O interrupt.
        //  Can be in one of two states - "standalone" or "pending in an I/O port".
        //  A "pending" interrupt cannot be destroyed.
        //  TODO make operators new/delete faster by re-using "deleted" instances from "new"
        class HADESVM_CEREON_PUBLIC IoInterrupt final
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(IoInterrupt)

            friend class IIoPort;

            //////////
            //  Construction/destruction
        public:
            IoInterrupt(uint16_t ioPortNumber, uint16_t interruptStatusCode)
                :   _ioPortNumber(ioPortNumber), _interruptStatusCode(interruptStatusCode) {}
            ~IoInterrupt() { Q_ASSERT(_ioPort == nullptr); }

            //////////
            //  Operations
        public:
            //  The I/O port number where the interrupt has occurred
            uint16_t            getIoPortNumber() const { return _ioPortNumber; }

            //  The ISC of the I/O interrupt
            uint16_t            getInterruptStatusCode() const { return _interruptStatusCode; }

            //////////
            //  Implementation
        private:
            IIoPort *           _ioPort = nullptr;  //  where it is pending; nullptr == standalone
            uint16_t            _ioPortNumber;
            uint16_t            _interruptStatusCode;
        };

        //////////
        //  A generic I/O port.
        //  I/O port methods are only safe to call from the master clock thread
        class HADESVM_CEREON_PUBLIC IIoPort
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(IIoPort)

            friend class IoBus;

            //////////
            //  Construction/destruction
        public:
            IIoPort() = default;
            virtual ~IIoPort() noexcept = default;

            //////////
            //  Operations
        public:
            //  Returns the address of this I/O port
            virtual uint16_t    address() const = 0;

            //  Checks whether interrupts are I/O enabled for this I/O port
            bool                interruptsEnabled() const { return _interruptsEnabled; }
            void                enableInterrupts();
            void                disableInterrupts();

            //  Makes sure an I/O interrupt with the specified ISC is pending
            //  in this I/O port. If another I/O interrupt is already pending
            //  in this I/O port, it is replaced
            void                setPendingIoInterrupt(uint16_t interruptStatusCode);

            //  If an I/O interrupt is pending in this port, returns it and
            //  switches its status to "standalone", so it is no longer pending
            //  in this I/O port. It is the caller's responsibility to "delete"
            //  the returned IoInterrupt instance.
            //  If no I/O interrupt is pending in this port, returns nullptr.
            //  A concrete IIoPort subclass may override this method, typically
            //  to detect when a pending interrupt is "released". Such overriding
            //  implementation must first call the base implementation and may
            //  then do some extra work.
            virtual IoInterrupt*releasePendingIoInterrupt();

            //  Returns the IoInterrupt currently "pending" in this port
            const IoInterrupt * pendingIoInterrupt() const;

            //////////
            //  Implementation
        private:
            IoBus *             _ioBus = nullptr;   //  nullptr == not yet attached to one
            bool                _interruptsEnabled = false;
            IoInterrupt *       _pendingIoInterrupt = nullptr;  //  nullptr == none
        };

        //////////
        //  A byte I/O port
        class HADESVM_CEREON_PUBLIC IByteIoPort : public virtual IIoPort
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(IByteIoPort)

            //////////
            //  Construction/destruction
        public:
            IByteIoPort() = default;

            //////////
            //  Operations
        public:
            //  Reads/writes a byte from/to this I/O port.
            //  Returns "true" on success, "false" if the reading/writing cannot
            //  be finished immediately. In the latter case the CPU must repeat
            //  the read/write call again and again until it finally succeeds -
            //  and the I/O port implementation MUST ensure that it eventually
            //  succeeds.
            virtual bool        readByte(uint8_t & value) = 0;
            virtual bool        writeByte(uint8_t value) = 0;
        };

        //////////
        //  A half-word I/O port
        class HADESVM_CEREON_PUBLIC IHalfWordIoPort : public virtual IIoPort
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(IHalfWordIoPort)

            //////////
            //  Construction/destruction
        public:
            IHalfWordIoPort() = default;

            //////////
            //  Operations
        public:
            //  Reads/writes a half-word from/to this I/O port
            //  Returns "true" on success, "false" if the reading/writing cannot
            //  be finished immediately. In the latter case the CPU must repeat
            //  the read/write call again and again until it finally succeeds -
            //  and the I/O port implementation MUST ensure that it eventually
            //  succeeds.
            virtual bool        readHalfWord(uint16_t & value) = 0;
            virtual bool        writeHalfWord(uint16_t value) = 0;
        };

        //////////
        //  A word I/O port
        class HADESVM_CEREON_PUBLIC IWordIoPort : public virtual IIoPort
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(IWordIoPort)

            //////////
            //  Construction/destruction
        public:
            IWordIoPort() = default;

            //////////
            //  Operations
        public:
            //  Reads/writes a word from/to this I/O port
            //  Returns "true" on success, "false" if the reading/writing cannot
            //  be finished immediately. In the latter case the CPU must repeat
            //  the read/write call again and again until it finally succeeds -
            //  and the I/O port implementation MUST ensure that it eventually
            //  succeeds.
            virtual bool        readWord(uint32_t & value) = 0;
            virtual bool        writeWord(uint32_t value) = 0;
        };

        //////////
        //  A long word I/O port
        class HADESVM_CEREON_PUBLIC ILongWordIoPort : public virtual IIoPort
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ILongWordIoPort)

            //////////
            //  Construction/destruction
        public:
            ILongWordIoPort() = default;

            //////////
            //  Operations
        public:
            //  Reads/writes a long word from/to this I/O port
            //  Returns "true" on success, "false" if the reading/writing cannot
            //  be finished immediately. In the latter case the CPU must repeat
            //  the read/write call again and again until it finally succeeds -
            //  and the I/O port implementation MUST ensure that it eventually
            //  succeeds.
            virtual bool        readLongWord(uint64_t & value) = 0;
            virtual bool        writeLongWord(uint64_t value) = 0;
        };

        //////////
        //  The I/O bus.
        //  I/O bus methods are only safe to call from the master clock thread
        class HADESVM_CEREON_PUBLIC IoBus : public hadesvm::core::Component,
                                            public virtual hadesvm::core::IClockedComponent,
                                            public virtual hadesvm::core::IActiveComponent
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(IoBus)

            friend class IIoPort;

            //////////
            //  Constants
        public:
            static const hadesvm::core::ClockFrequency  DefaultClockFrequency;

            //////////
            //  Types
        public:
            //  The type of a Cereon I/O bus
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
                virtual IoBus * createComponent() override;
            };

            //////////
            //  Construction/destruction
        public:
            IoBus();
            virtual ~IoBus() noexcept;

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
            virtual void        onClockTick() noexcept override {}

            //////////
            //  hadesvm::core::IActiveComponent
        public:
            //  Although a memory bus is, technically, a clocked component,
            //  there isn't anything that must actually happen within a MemoryBus
            //  instance on every clock tick. Therefore, we make it implement
            //  IActiveComponent to make sure VA does not attempt to "tick"
            //  the MemoryBus on its own common worker thread, but do not create
            //  an actual bus-only worker thread

            //////////
            //  Operations (configuration)
        public:
            void                setClockFrequency(const hadesvm::core::ClockFrequency & clockFrequency);

            //////////
            //  Operations
        public:
            //  Attaches the specified I/O port to this memory bus.
            //  Throws hadesvm::core::VirtualApplianceException if an error occurs (port conflict, etc.)
            //  Must only be called from the QApplication's main thread
            void                attachIoPort(IIoPort * ioPort) throws(hadesvm::core::VirtualApplianceException);
            void                detachIoPort(IIoPort * ioPort);

            //  Reads/writes a value of the specified size from/to the I/O
            //  port at the specified address.
            //  Returns "true" on success, "false" if the reading/writing cannot
            //  be finished immediately. In the latter case the CPU must repeat
            //  the read/write call again and again until it finally succeeds -
            //  and the I/O port implementation MUST ensure that it eventually
            //  succeeds.
            //  If there is no I/O port at the specified address, "read" returns
            //  0 and "write" is ignored, and both are deemed a "success".
            //  Thread-safe; IoBus serialises accesses.
            bool                readByte(uint16_t address, uint8_t & value);
            bool                readHalfWord(uint16_t address, uint16_t & value);
            bool                readWord(uint16_t address, uint32_t & value);
            bool                readLongWord(uint16_t address, uint64_t & value);

            bool                writeByte(uint16_t address, uint8_t value);
            bool                writeHalfWord(uint16_t address, uint16_t value);
            bool                writeWord(uint16_t address, uint32_t value);
            bool                writeLongWord(uint16_t address, uint64_t value);

            //  Implementation of TSTP/SETP instruction behaviour
            //  Thread-safe; IoBus serialises accesses.
            uint64_t            testPortStatus(uint16_t address);
            void                setPortStatus(uint16_t address, uint64_t status);

            //  Returns the next I/O interrupt ready to handle (converting it from
            //  Pending to Standalone); the caller should "delete" the returned
            //  IoInterrupt when done with it. Returns "nullptr" if there are no
            //  pending I/O interrupts ready to handle.
            //  Thread-safe; IoBus serialises accesses.
            IoInterrupt *       getIoInterrupt();

            //////////
            //  Implementation
        private:
            State               _state = State::Constructed;

            //  Configuration
            hadesvm::core::ClockFrequency   _clockFrequency;

            //  Primary address -> I/O port map (nullptr elements for absent ports)
            IIoPort *           _ioPorts[65536];

            //  Secondary address -> I/O port maps (nullptr elements for absent ports).
            //  Maintained consistent with "_ioPorts" by attach/detach services
            //  in order to avoid down-casting IIoPort instances every time an
            //  I/O is performed
            IByteIoPort *       _byteIoPorts[65536];        //  nullptr for absent ports
            IHalfWordIoPort *   _halfWordIoPorts[65536];    //  nullptr for absent ports
            IWordIoPort *       _wordIoPorts[65536];        //  nullptr for absent ports
            ILongWordIoPort *   _longWordIoPorts[65536];    //  nullptr for absent ports

            //  The queue of all I/O interrupts that are pending in attached
            //  I/O ports where interrupts are enabled
            using IoInterruptQueue = QQueue<IoInterrupt*>;
            IoInterruptQueue    _interruptsReadyToHandle;

            //  Bus locking
            QMutex              _accessGuard;
        };

        //////////
        //  An "I/O controller" is a component that exposes 1 or more
        //  I/O ports through which it can be attached to an I/O bus.
        class HADESVM_CEREON_PUBLIC IIoController : public virtual hadesvm::core::IComponent
        {
            //////////
            //  Operations
        public:
            //  Returns an unordered list of I/O ports provided by this I/O controller.
            virtual IoPortList  ioPorts() = 0;
        };
    }
}

//  End of hadesvm-cereon/Io.hpp

