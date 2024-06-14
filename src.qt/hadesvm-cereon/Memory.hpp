//
//  hadesvm-cereon/Memory.hpp
//
//  Cereon memory components
//
//////////
#pragma once
#include "hadesvm-cereon/API.hpp"

static_assert(sizeof(size_t) >= 4);

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  Thrown if a memory access fails
        enum class MemoryAccessError
        {
            InvalidAddress,     //  accessing nonexistent memory
            InvalidAlignment,   //  access not naturally aligned
            AccessDenied,       //  writing to ROM
            HardwareFault       //  other error
        };

        //////////
        //  A generic "memory block" handles accesses to a continuous range
        //  of a 64-bit address space
        class HADESVM_CEREON_PUBLIC IMemoryBlock
        {
            //////////
            //  This is an interface
        public:
            virtual ~IMemoryBlock() noexcept = default;

            //////////
            //  Operations
        public:
            //  Returns the starting address of this memory block
            virtual uint64_t        startAddress() const = 0;

            //  Returns the size of this memory block
            virtual hadesvm::core::MemorySize
                                    size() const = 0;

            //  Loads a naturally aligned data item from the specified offset
            //  in this memory block.
            //  Throws MemoryAccessError if an error occurs
            virtual uint8_t         loadByte(size_t offset) throws(MemoryAccessError) = 0;
            virtual uint16_t        loadHalfWord(size_t offset, ByteOrder byteOrder) throws(MemoryAccessError) = 0;
            virtual uint32_t        loadWord(size_t offset, ByteOrder byteOrder) throws(MemoryAccessError) = 0;
            virtual uint64_t        loadLongWord(size_t offset, ByteOrder byteOrder) throws(MemoryAccessError) = 0;

            //  Stores a naturally aligned data item into this memory block
            //  at the specified offset.
            //  Throws MemoryAccessError if an error occurs
            virtual void            storeByte(size_t offset, uint8_t value) throws(MemoryAccessError) = 0;
            virtual void            storeHalfWord(size_t offset, uint16_t value, ByteOrder byteOrder) throws(MemoryAccessError) = 0;
            virtual void            storeWord(size_t offset, uint32_t value, ByteOrder byteOrder) throws(MemoryAccessError) = 0;
            virtual void            storeLongWord(size_t offset, uint64_t value, ByteOrder byteOrder) throws(MemoryAccessError) = 0;
        };

        //////////
        //  A "memory unit" is a component aspect that exposes 1 or more
        //  MemoryBlocks I/O which can be attached to a MemoryBus.
        class HADESVM_CEREON_PUBLIC IMemoryUnitAspect : public virtual hadesvm::core::IComponentAspect
        {
            //////////
            //  Operations
        public:
            //  Returns an unordered list of memory blocks provided by this memory unit.
            virtual MemoryBlockList memoryBlocks() const = 0;
        };

        //////////
        //  The Cereon memory bus delegates loads/stores to one of the
        //  attached memory blocks
        class HADESVM_CEREON_PUBLIC MemoryBus : public hadesvm::core::Component,
                                                public virtual hadesvm::core::IClockedComponentAspect
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(MemoryBus)

            //////////
            //  Constants
        public:
            static const hadesvm::core::ClockFrequency  DefaultClockFrequency;

            //////////
            //  Types
        public:
            //  The type of a Cereon memory bus
            class HADESVM_CEREON_PUBLIC Type final : public hadesvm::core::ComponentType
            {
                HADESVM_DECLARE_SINGLETON(Type)

                //////////
                //  hadesvm::util::StockObject
            public:
                virtual QString     mnemonic() const override;
                virtual QString     displayName() const override;

                //////////
                //  hadesvm::core::ComponentType
            public:
                virtual hadesvm::core::ComponentCategory *  category() const override;
                virtual bool        suspendable() const override;
                virtual bool        isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const override;
                virtual bool        isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const override;
                virtual MemoryBus * createComponent() override;
            };

            //////////
            //  Construction/destruction
        public:
            MemoryBus();
            virtual ~MemoryBus() noexcept;

            //////////
            //  hadesvm::core::Component
        public:
            virtual Type *          type() const override { return Type::instance(); }
            virtual QString         displayName() const override;
            virtual void            serialiseConfiguration(QDomElement componentElement) const override;
            virtual void            deserialiseConfiguration(QDomElement componentElement) override;
            virtual hadesvm::core::ComponentEditor *
                                    createEditor(QWidget * parent) override;
            virtual Ui *            createUi() override;

            //////////
            //  hadesvm::core::Component (state management)
            //  Must only be called from the QApplication's main thread (except state())
        public:
            virtual State           state() const noexcept override;
            virtual void            connect() throws(hadesvm::core::VirtualApplianceException) override;
            virtual void            initialize() throws(hadesvm::core::VirtualApplianceException) override;
            virtual void            start() throws(hadesvm::core::VirtualApplianceException) override;
            virtual void            stop() noexcept override;
            virtual void            deinitialize() noexcept override;
            virtual void            disconnect() noexcept override;

            //////////
            //  hadesvm::core::IComponentAspect
        public:
            virtual MemoryBus *     getComponent() const override { return const_cast<MemoryBus*>(this); }

            //////////
            //  hadesvm::core::IClockedComponentAspect
        public:
            hadesvm::core::ClockFrequency   clockFrequency() const { return _clockFrequency; }

            //////////
            //  Operations (configuration)
        public:
            void                    setClockFrequency(const hadesvm::core::ClockFrequency & clockFrequency);

            //////////
            //  Operations
        public:
            //  Attaches the specified "memoryBlock" to this memory bus, to handle
            //  accesses in range [startAddress .. startAddress + size).
            //  Has no effect if the memoryBlock is already attached.
            //  Throws hadesvm::core::VirtualApplianceException if an error occurs
            void                    attachMemoryBlock(IMemoryBlock * memoryBlock) throws(hadesvm::core::VirtualApplianceException);

            //  Detaches the specified "memoryBlock" from this memory bus; has
            //  no effect if not attached
            void                    detachMemoryBlock(IMemoryBlock * memoryBlock);

            //  Loads a naturally aligned data item at the specified address.
            //  Throws MemoryAccessError
            uint8_t                 loadByte(uint64_t address) throws(MemoryAccessError);
            uint16_t                loadHalfWord(uint64_t address, ByteOrder byteOrder) throws(MemoryAccessError);
            uint32_t                loadWord(uint64_t address, ByteOrder byteOrder) throws(MemoryAccessError);
            uint64_t                loadLongWord(uint64_t address, ByteOrder byteOrder) throws(MemoryAccessError);

            //  Stores a naturally aligned data item at the specified address.
            //  Throws MemoryAccessError if an error occurs
            void                    storeByte(uint64_t address, uint8_t value) throws(MemoryAccessError);
            void                    storeHalfWord(uint64_t address, uint16_t value, ByteOrder byteOrder) throws(MemoryAccessError);
            void                    storeWord(uint64_t address, uint32_t value, ByteOrder byteOrder) throws(MemoryAccessError);
            void                    storeLongWord(uint64_t address, uint64_t value, ByteOrder byteOrder) throws(MemoryAccessError);

            //////////
            //  Bus locking
        public:
            //  Locks/unlocks this MemoryBus, simulating hardware lock of a bus.
            //  This p[rotocol is compatible with QMutexLocker (on purpose!)
            void                    lock() { _lock.lock(); }
            void                    unlock() { _lock.unlock(); }

            //////////
            //  Implementation
        private:
            State                   _state = State::Constructed;

            //  Configuration
            hadesvm::core::ClockFrequency   _clockFrequency;

            //  Runtime state
            struct _Mapping
            {
                _Mapping() : _memoryBlock(nullptr), _startAddress(0), _endAddress(0) {}
                _Mapping(IMemoryBlock * memoryBlock, uint64_t startAddress, uint64_t endAddress)
                    :   _memoryBlock(memoryBlock),
                        _startAddress(startAddress),
                        _endAddress(endAddress) {}

                IMemoryBlock *      _memoryBlock;   //  mapped
                uint64_t            _startAddress;  //  inclusive
                uint64_t            _endAddress;    //  inclusive
            };

            _Mapping *              _mappings;      //  array of "_mappingsList.size()" elements
            _Mapping *              _endMappings;   //  _mappings + "_mappingsList.size()"

            QMutex                  _lock;

            //  Helpers
            _Mapping *              _findMapping(uint64_t address)
            {
                for (_Mapping * mapping = _mappings; mapping < _endMappings; mapping++)
                {
                    if (address >= mapping->_startAddress && address <= mapping->_endAddress)
                    {
                        return mapping;
                    }
                }
                return nullptr;
            }
        };

        //////////
        //  A memory unit whose content is kept entirely in host RAM.
        //  Technically a component with an IMemoryUnitAspect aspect, which
        //  also doubles at one and only IMemoryBlock it provides
        class HADESVM_CEREON_PUBLIC ResidentMemoryUnit : public hadesvm::core::Component,
                                                         public virtual IMemoryUnitAspect,
                                                         public virtual IMemoryBlock
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ResidentMemoryUnit)

            //////////
            //  Construction/destruction
        public:
            ResidentMemoryUnit(uint64_t startAddress, const hadesvm::core::MemorySize & size);
            virtual ~ResidentMemoryUnit() noexcept;

            //////////
            //  hadesvm::core::Component
        public:
            virtual void            serialiseConfiguration(QDomElement componentElement) const override;
            virtual void            deserialiseConfiguration(QDomElement componentElement) override;

            //////////
            //  hadesvm::core::Component (state management)
            //  Must only be called from the QApplication's main thread (except state())
        public:
            virtual State           state() const noexcept override;
            virtual void            connect() throws(hadesvm::core::VirtualApplianceException) override;
            virtual void            initialize() throws(hadesvm::core::VirtualApplianceException) override;
            virtual void            start() throws(hadesvm::core::VirtualApplianceException) override;
            virtual void            stop() noexcept override;
            virtual void            deinitialize() noexcept override;
            virtual void            disconnect() noexcept override;

            //////////
            //  hadesvm::core::IComponentAspect
        public:
            virtual Component *     getComponent() const override { return const_cast<ResidentMemoryUnit*>(this); }

            //////////
            //  IMemoryUnitAspect
        public:
            virtual MemoryBlockList memoryBlocks() const override { return _memoryBlocks; }

            //////////
            //  IMemoryBlock
        public:
            virtual uint64_t        startAddress() const override { return _startAddress; }
            virtual hadesvm::core::MemorySize
                                    size() const override { return _size; }
            virtual uint8_t         loadByte(size_t offset) throws(MemoryAccessError) override;
            virtual uint16_t        loadHalfWord(size_t offset, ByteOrder byteOrder) throws(MemoryAccessError) override;
            virtual uint32_t        loadWord(size_t offset, ByteOrder byteOrder) throws(MemoryAccessError) override;
            virtual uint64_t        loadLongWord(size_t offset, ByteOrder byteOrder) throws(MemoryAccessError) override;
            virtual void            storeByte(size_t offset, uint8_t value) throws(MemoryAccessError) override;
            virtual void            storeHalfWord(size_t offset, uint16_t value, ByteOrder byteOrder) throws(MemoryAccessError) override;
            virtual void            storeWord(size_t offset, uint32_t value, ByteOrder byteOrder) throws(MemoryAccessError) override;
            virtual void            storeLongWord(size_t offset, uint64_t value, ByteOrder byteOrder) throws(MemoryAccessError) override;

            //////////
            //  Operations (configuration)
        public:
            void                    setStartAddress(uint64_t startAddress);
            void                    setSize(const hadesvm::core::MemorySize & size);

            //////////
            //  Implementation
        private:
            State                   _state = State::Constructed;
            MemoryBlockList         _memoryBlocks;

            //  Configuration
            uint64_t                _startAddress;
            hadesvm::core::MemorySize   _size;

            size_t                  _sizeInBytes;   //  same as "_size", but expressed in bytes

            //  Runtime state
            uint8_t *               _data;  //  array of "_sizeInBytes" bytes
        };

        //////////
        //  A RAM block whose content is kept entirely in host RAM
        class HADESVM_CEREON_PUBLIC ResidentRamUnit : public ResidentMemoryUnit
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ResidentRamUnit)

            //////////
            //  Constants
        public:
            static const uint64_t   DefaultStartAddress;
            static const hadesvm::core::MemorySize  DefaultSize;

            //////////
            //  Types
        public:
            //  The type of a resident RAM block
            class HADESVM_CEREON_PUBLIC Type final : public hadesvm::core::ComponentType
            {
                HADESVM_DECLARE_SINGLETON(Type)

                //////////
                //  hadesvm::util::StockObject
            public:
                virtual QString     mnemonic() const override;
                virtual QString     displayName() const override;

                //////////
                //  hadesvm::core::ComponentType
            public:
                virtual hadesvm::core::ComponentCategory *
                                    category() const override;
                virtual bool        suspendable() const override;
                virtual bool        isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const override;
                virtual bool        isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const override;
                virtual ResidentRamUnit *
                                    createComponent() override;
            };

            //////////
            //  Construction/destruction
        public:
            ResidentRamUnit();
            virtual ~ResidentRamUnit() noexcept;

            //////////
            //  hadesvm::core::Component
        public:
            virtual Type *          type() const override { return Type::instance(); }
            virtual QString         displayName() const override;
            virtual hadesvm::core::ComponentEditor *
                                    createEditor(QWidget * parent) override;
            virtual Ui *            createUi() override;
        };

        //////////
        //  A ROM block whose content is loaded from a file and kept entirely in host RAM
        class HADESVM_CEREON_PUBLIC ResidentRomUnit : public ResidentMemoryUnit
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ResidentRomUnit)

            //////////
            //  Constants
        public:
            static const uint64_t   DefaultStartAddress;
            static const hadesvm::core::MemorySize DefaultSize;
            static const QString    DefailtContentFilePath;

            //////////
            //  Types
        public:
            //  The type of a resident ROM block
            class HADESVM_CEREON_PUBLIC Type final : public hadesvm::core::ComponentType
            {
                HADESVM_DECLARE_SINGLETON(Type)

                //////////
                //  hadesvm::util::StockObject
            public:
                virtual QString     mnemonic() const override;
                virtual QString     displayName() const override;

                //////////
                //  hadesvm::core::ComponentType
            public:
                virtual hadesvm::core::ComponentCategory *
                                    category() const override;
                virtual bool        suspendable() const override;
                virtual bool        isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const override;
                virtual bool        isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const override;
                virtual ResidentRomUnit *
                                    createComponent() override;
            };

            //////////
            //  Construction/destruction
        public:
            ResidentRomUnit();
            virtual ~ResidentRomUnit() noexcept;

            //////////
            //  hadesvm::core::Component
        public:
            virtual Type *          type() const override { return Type::instance(); }
            virtual QString         displayName() const override;
            virtual void            serialiseConfiguration(QDomElement componentElement) const override;
            virtual void            deserialiseConfiguration(QDomElement componentElement) override;
            virtual hadesvm::core::ComponentEditor *
                                    createEditor(QWidget * parent) override;
            virtual Ui *            createUi() override;

            //////////
            //  hadesvm::core::Component (state management)
            //  Must only be called from the QApplication's main thread (except state())
        public:
            virtual void            initialize() throws(hadesvm::core::VirtualApplianceException) override;

            //////////
            //  IMemoryBlock
        public:
            Q_NORETURN virtual void storeByte(size_t offset, uint8_t value) throws(MemoryAccessError) override;
            Q_NORETURN virtual void storeHalfWord(size_t offset, uint16_t value, ByteOrder byteOrder) throws(MemoryAccessError) override;
            Q_NORETURN virtual void storeWord(size_t offset, uint32_t value, ByteOrder byteOrder) throws(MemoryAccessError) override;
            Q_NORETURN virtual void storeLongWord(size_t offset, uint64_t value, ByteOrder byteOrder) throws(MemoryAccessError) override;

            //////////
            //  Operations (configuration)
        public:
            QString                 contentFilePath() const;
            void                    setContentFilePath(const QString & contentFilePath);

            //////////
            //  Implementation
        private:
            QString                 _contentFilePath;   //  if relative, uses VM location dir as root

            //  Helpers
            QString                 _resolveContentFilePath(const QString & path) const;
        };
    }
}

//  End of hadesvm-cereon/Memory.hpp
