//
//  hadesvm-cereon/Cmos1.hpp
//
//  The Cereon CMOS1 nonvolatile memory controller
//
//////////

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The Cereon CMOS1 nonvolatile memory controller
        class HADESVM_CEREON_PUBLIC Cmos1 : public hadesvm::core::Component,
                                            public virtual hadesvm::core::IClockedComponent,
                                            public virtual IIoController

        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Cmos1)

            //////////
            //  Constants
        public:
            static const hadesvm::core::TimeInterval    DefaultReadDelay;
            static const hadesvm::core::TimeInterval    DefaultWriteDelay;
            static const uint16_t                       DefaultStatePortAddress;
            static const uint16_t                       DefaultAddressPortAddress;
            static const uint16_t                       DefaultDataPortAddress;
            static const uint16_t                       DefaultInterruptMaskPortAddress;
            static const hadesvm::core::ClockFrequency  DefaultClockFrequency;
            static const QString                        DefaultContentFilePath;

            //////////
            //  Types
        public:
            //  The type of a Cereon CMOS1 component
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
                virtual Cmos1 * createComponent() override;
            };

            //////////
            //  Construction/destruction
        public:
            Cmos1();    //  Constructs a CMOS1 bundle with the default configuration
            virtual ~Cmos1() noexcept;

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
            hadesvm::core::TimeInterval readDelay() const { return _readDelay; }
            void                setReadDelay(const hadesvm::core::TimeInterval & readDelay);
            hadesvm::core::TimeInterval writeDelay() const { return _writeDelay; }
            void                setWriteDelay(const hadesvm::core::TimeInterval & writeDelay);
            uint16_t            statePortAddress() const { return _statePortAddress; }
            void                setStatePortAddress(uint16_t statePortAddress);
            uint16_t            addressPortAddress() const { return _addressPortAddress; }
            void                setAddressPortAddress(uint16_t addressPortAddress);
            uint16_t            dataPortAddress() const { return _dataPortAddress; }
            void                setDataPortAddress(uint16_t dataPortAddress);
            uint16_t            interruptMaskPortAddress() const { return _interruptMaskPortAddress; }
            void                setInterruptMaskPortAddress(uint16_t interruptMaskPortAddress);
            void                setClockFrequency(const hadesvm::core::ClockFrequency & clockFrequency);
            QString             contentFilePath() const { return _contentFilePath; }
            void                setContentFilePath(const QString & contentFilePath);

            //////////
            //  Implementation
        private:
            State               _state = State::Constructed;

            //  Configuration
            hadesvm::core::TimeInterval _readDelay;
            hadesvm::core::TimeInterval _writeDelay;
            uint16_t            _statePortAddress;
            uint16_t            _addressPortAddress;
            uint16_t            _dataPortAddress;
            uint16_t            _interruptMaskPortAddress;
            hadesvm::core::ClockFrequency   _clockFrequency;
            QString             _contentFilePath;   //  if relative, use VM location's directory as root

            unsigned            _clockTicksBetweenTimeUpdates;

            //  Runtime state - accessed from CPU worker threads (via I/O ports)
            //  and CMOS1 worker thread (directly)
            hadesvm::util::Spinlock _runtimeStateGuard;
            IoPortList          _ioPorts;   //  fixed at runtime
            char                _content[256];
            std::atomic<bool>   _contentNeedsSaving;
            std::atomic<unsigned>   _clockTicksUntilTimeUpdate;

            enum class _OperationalState
            {
                _Ready,
                _WritingAddress,
                _WritingData,
                _ReadingAddress,
                _ReadingData
            };
            std::atomic<_OperationalState>  _operationalState = _OperationalState::_Ready;
            std::atomic<unsigned>   _clockTicksToDelay = 0;

            std::atomic<uint8_t>    _currentAddress = 0;

            //////////
            //  I/O ports
        private:
            class HADESVM_CEREON_PUBLIC _StatePort final : public virtual IByteIoPort
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_StatePort)

                //////////
                //  Construction/destruction
            public:
                explicit _StatePort(Cmos1 * cmos1) : _cmos1(cmos1) {}

                //////////
                //  IIoPort
            public:
                virtual uint16_t    address() const override { return _cmos1->_statePortAddress; }

                //////////
                //  IByteIoPort
            public:
                virtual bool    readByte(uint8_t & value) override;
                virtual bool    writeByte(uint8_t value) override;

                //////////
                //  Implementation
            private:
                Cmos1 *         _cmos1;
            };
            _StatePort          _statePort;

            class HADESVM_CEREON_PUBLIC _AddressPort final : public virtual IByteIoPort
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_AddressPort)

                //////////
                //  Construction/destruction
            public:
                explicit _AddressPort(Cmos1 * cmos1) : _cmos1(cmos1) {}

                //////////
                //  IIoPort
            public:
                virtual uint16_t    address() const override { return _cmos1->_addressPortAddress; }

                //////////
                //  IByteIoPort
            public:
                virtual bool    readByte(uint8_t & value) override;
                virtual bool    writeByte(uint8_t value) override;

                //////////
                //  Implementation
            private:
                Cmos1 *         _cmos1;
            };
            _AddressPort        _addressPort;

            class HADESVM_CEREON_PUBLIC _DataPort final : public virtual IByteIoPort
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_DataPort)

                //////////
                //  Construction/destruction
            public:
                explicit _DataPort(Cmos1 * cmos1) : _cmos1(cmos1) {}

                //////////
                //  IIoPort
            public:
                virtual uint16_t    address() const override { return _cmos1->_dataPortAddress; }

                //////////
                //  IByteIoPort
            public:
                virtual bool    readByte(uint8_t & value) override;
                virtual bool    writeByte(uint8_t value) override;

                //////////
                //  Implementation
            private:
                Cmos1 *         _cmos1;
            };
            _DataPort           _dataPort;
        };
    }
}

//  End of hadesvm-cereon/Cmos1.hpp

