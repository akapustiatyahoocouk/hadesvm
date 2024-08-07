//
//  hadesvm-cereon/Processors.hpp
//
//  Cereon processor components
//
//////////

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The Cereon processor
        class HADESVM_CEREON_PUBLIC Processor : public hadesvm::core::Component,
                                                public virtual hadesvm::core::IClockedComponent,
                                                public virtual hadesvm::core::IActiveComponent
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Processor)

            friend class ProcessorCore;

            //////////
            //  Construction/destruction
        public:
            Processor(const hadesvm::core::ClockFrequency & clockFrequency, uint8_t id,
                      ByteOrder byteOrder, bool canChangeByteOrder,
                      uint64_t restartAddress, bool isPrimaryProcessor);
            virtual ~Processor() noexcept;

            //////////
            //  hadesvm::core::Component
        public:
            virtual QString     displayName() const override;
            virtual void        serialiseConfiguration(QDomElement componentElement) const override;
            virtual void        deserialiseConfiguration(QDomElement componentElement) override;
            virtual hadesvm::core::ComponentEditor *    createEditor() override;
            virtual Ui *        createUi() override;

            //////////
            //  hadesvm::core::Component (state management)
            //  Must only be called from the QApplication's main thread (except state())
        public:
            virtual State       state() const noexcept override { return _state; }
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
            //  hadesvm::core::IActiveComponent
        public:

            //////////
            //  Operations (configuration)
        public:
            void                setClockFrequency(const hadesvm::core::ClockFrequency & clockFrequency);
            uint8_t             id() const { return _id; }
            void                setId(uint8_t id);
            ByteOrder           byteOrder() const { return _byteOrder; }
            void                setByteOrder(ByteOrder byteOrder);
            bool                canChangeByteOrder() const { return _canChangeByteOrder; }
            uint64_t            restartAddress() const { return _restartAddress; }
            void                setRestartAddress(uint64_t restartAddress);
            bool                isPrimaryProcessor() const { return _isPrimaryProcessor; }
            void                setPrimaryProcessor(bool primaryProcessor);

            //  The union of Feature sets of all cores
            Features            features() const;

            //////////
            //  Implementation
        private:
            State               _state = State::Constructed;

            //  Configuration
            hadesvm::core::ClockFrequency   _clockFrequency;
            uint8_t             _id;
            ByteOrder           _byteOrder;
            bool                _canChangeByteOrder;
            uint64_t            _restartAddress;
            bool                _isPrimaryProcessor;

            //  Cores - as QList (for configuration stage)...
            QList<ProcessorCore*>   _cores;
            //  ...and as a plain C list - for runtime stage
            ProcessorCore *     _coresAsArray[256];
            size_t              _numCores;

            //  Links to other VM components
            MemoryBus *         _memoryBus = nullptr;   //  nullptr == not attached
            IoBus *             _ioBus = nullptr;   //  nullptr == not attached

            //  Threads
            class _WorkerThread final : public QThread
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_WorkerThread)

                //////////
                //  Construction/destruction
            public:
                explicit _WorkerThread(Processor * processor)
                    :   _processor(processor), _stopRequested(false) {}
                virtual ~_WorkerThread() = default;

                //////////
                //  QThread
            protected:
                virtual void    run() override;

                //////////
                //  Operations
            public:
                void            requestStop() { _stopRequested = true; }

                //////////
                //  Implementation
            private:
                Processor *const    _processor;
                std::atomic<bool>   _stopRequested;
            };
            _WorkerThread *     _workerThread = nullptr;
        };

        //////////
        //  The Cereon-1P1B processor
        class HADESVM_CEREON_PUBLIC Cereon1P1B final : public Processor
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Cereon1P1B)

            //////////
            //  Constants
        public:
            static const ByteOrder      DefaultByteOrder;
            static const uint64_t       DefaultRestartAddress;
            static const hadesvm::core::ClockFrequency  DefaultClockFrequency;

            //////////
            //  Types
        public:
            //  The type of a Cereon-1P1B processor
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
                virtual bool                isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const override;
                virtual bool                isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const override;
                virtual Cereon1P1B *        createComponent() override;
            };

            //////////
            //  Construction/destruction
        public:
            explicit Cereon1P1B(uint8_t id);
            virtual ~Cereon1P1B() noexcept;

            //////////
            //  hadesvm::core::Component
        public:
            virtual Type *      componentType() const override { return Type::instance(); }

            //////////
            //  Implementation
        private:

            //  Processor blocks
            DummyMmu            _mmu;
            ProcessorCore       _core;
        };
    }
}

//  End of hadesvm-cereon/Processors.hpp

