//
//  hadesvm-cereon/Memory.hpp
//
//  Cereon memory components
//
//////////
#pragma once
#include "hadesvm-cereon/API.hpp"

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
        class HADESVM_CEREON_PUBLIC MemoryBlock : public hadesvm::core::Component
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(MemoryBlock)

            //////////
            //  Construction/destruction
        public:
            MemoryBlock() = default;
            virtual ~MemoryBlock() noexcept = default;

            //////////
            //  Operations
        public:
            //  Returns the starting address of this memory block
            virtual uint64_t    startAddress() const = 0;

            //  Returns the size of this memory block
            virtual MemorySize  size() const = 0;

            //  Loads a naturally aligned data item from the specified offset
            //  in this memory block.
            //  Throws MemoryAccessError if an error occurs
            virtual uint8_t     loadByte(size_t offset) throws(MemoryAccessError) = 0;
            virtual uint16_t    loadHalfWord(size_t offset, ByteOrder byteOrder) throws(MemoryAccessError) = 0;
            virtual uint32_t    loadWord(size_t offset, ByteOrder byteOrder) throws(MemoryAccessError) = 0;
            virtual uint64_t    loadLongWord(size_t offset, ByteOrder byteOrder) throws(MemoryAccessError) = 0;

            //  Stores a naturally aligned data item into this memory block
            //  at the specified offset.
            //  Throws MemoryAccessError if an error occurs
            virtual void        storeByte(size_t offset, uint8_t value) throws(MemoryAccessError) = 0;
            virtual void        storeHalfWord(size_t offset, uint16_t value, ByteOrder byteOrder) throws(MemoryAccessError) = 0;
            virtual void        storeWord(size_t offset, uint32_t value, ByteOrder byteOrder) throws(MemoryAccessError) = 0;
            virtual void        storeLongWord(size_t offset, uint64_t value, ByteOrder byteOrder) throws(MemoryAccessError) = 0;
        };

        //////////
        //  The Cereon memory bus delegates loads/stores to one of the
        //  attached memory blocks
        class HADESVM_CEREON_PUBLIC MemoryBus : public hadesvm::core::Component
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(MemoryBus)

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
                virtual QString mnemonic() const override;
                virtual QString displayName() const override;

                //////////
                //  hadesvm::core::ComponentType
            public:
                virtual hadesvm::core::ComponentCategory * category() const override;
                virtual bool    suspendable() const override;
                virtual bool    isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const override;
                virtual bool    isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const override;
                virtual MemoryBus *  createComponent() override;
            };

            //////////
            //  Construction/destruction
        public:
            MemoryBus();
            virtual ~MemoryBus() noexcept;

            //////////
            //  hadesvm::core::Component
        public:
            virtual Type *      type() const override { return Type::instance(); }
            virtual QString     displayName() const override;
            virtual void        serialiseConfiguration(QDomElement componentElement) const override;
            virtual void        deserialiseConfiguration(QDomElement componentElement) override;
            virtual ComponentEditor *   createEditor(QWidget * parent) override;
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
            //  Operations
        public:
            //  Attaches the specified "memoryBlock" to this memory bus, to handle
            //  accesses in range [startAddress .. startAddress + size).
            //  Has no effect if the memoryBlock is already attached.
            //  Throws hadesvm::core::VirtualApplianceException if an error occurs
            void                attachMemoryBlock(MemoryBlock * memoryBlock) throws(hadesvm::core::VirtualApplianceException);

            //  Detaches the specified "memoryBlock" from this memory bus; has
            //  no effect if not attached
            void                detachMemoryBlock(MemoryBlock * memoryBlock);

            //  Loads a naturally aligned data item at the specified address.
            //  Throws MemoryAccessError
            uint8_t             loadByte(uint64_t address) throws(MemoryAccessError);
            uint16_t            loadHalfWord(uint64_t address, ByteOrder byteOrder) throws(MemoryAccessError);
            uint32_t            loadWord(uint64_t address, ByteOrder byteOrder) throws(MemoryAccessError);
            uint64_t            loadLongWord(uint64_t address, ByteOrder byteOrder) throws(MemoryAccessError);

            //  Stores a naturally aligned data item at the specified address.
            //  Throws MemoryAccessError if an error occurs
            void                storeByte(uint64_t address, uint8_t value) throws(MemoryAccessError);
            void                storeHalfWord(uint64_t address, uint16_t value, ByteOrder byteOrder) throws(MemoryAccessError);
            void                storeWord(uint64_t address, uint32_t value, ByteOrder byteOrder) throws(MemoryAccessError);
            void                storeLongWord(uint64_t address, uint64_t value, ByteOrder byteOrder) throws(MemoryAccessError);

            //////////
            //  Implementation
        private:
            State               _state = State::Constructed;

            struct _Mapping
            {
                _Mapping() : _memoryBlock(nullptr), _startAddress(0), _endAddress(0) {}
                _Mapping(MemoryBlock * memoryBlock, uint64_t startAddress, uint64_t endAddress)
                    :   _memoryBlock(memoryBlock),
                        _startAddress(startAddress),
                        _endAddress(endAddress) {}

                MemoryBlock *   _memoryBlock;   //  mapped
                uint64_t        _startAddress;  //  inclusive
                uint64_t        _endAddress;    //  inclusive
            };

            _Mapping *          _mappings;      //  array of "_mappingsList.size()" elements
            _Mapping *          _endMappings;   //  _mappings + "_mappingsList.size()"

            //  Helpers
            _Mapping *          _findMapping(uint64_t address)
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
        //  A memory block whose content is kept entirely in host RAM
        class HADESVM_CEREON_PUBLIC ResidentMemoryBlock : public MemoryBlock
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ResidentMemoryBlock)

            //////////
            //  Construction/destruction
        public:
            ResidentMemoryBlock(uint64_t startAddress, const MemorySize & size);
            virtual ~ResidentMemoryBlock() noexcept;

            //////////
            //  hadesvm::core::Component
        public:
            virtual void        serialiseConfiguration(QDomElement componentElement) const override;
            virtual void        deserialiseConfiguration(QDomElement componentElement) override;

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
            //  MemoryBlock
        public:
            virtual uint64_t    startAddress() const override { return _startAddress; }
            virtual MemorySize  size() const override { return _size; }
            virtual uint8_t     loadByte(size_t offset) throws(MemoryAccessError) override;
            virtual uint16_t    loadHalfWord(size_t offset, ByteOrder byteOrder) throws(MemoryAccessError) override;
            virtual uint32_t    loadWord(size_t offset, ByteOrder byteOrder) throws(MemoryAccessError) override;
            virtual uint64_t    loadLongWord(size_t offset, ByteOrder byteOrder) throws(MemoryAccessError) override;
            virtual void        storeByte(size_t offset, uint8_t value) throws(MemoryAccessError) override;
            virtual void        storeHalfWord(size_t offset, uint16_t value, ByteOrder byteOrder) throws(MemoryAccessError) override;
            virtual void        storeWord(size_t offset, uint32_t value, ByteOrder byteOrder) throws(MemoryAccessError) override;
            virtual void        storeLongWord(size_t offset, uint64_t value, ByteOrder byteOrder) throws(MemoryAccessError) override;

            //////////
            //  Operations (configuration)
        public:
            void                setStartAddress(uint64_t startAddress);
            void                setSize(const MemorySize & size);

            //////////
            //  Implementation
        private:
            State               _state = State::Constructed;

            //  Configuration
            uint64_t            _startAddress;
            MemorySize          _size;

            size_t              _sizeInBytes;   //  same as "_size", but expressed in bytes

            //  Runtime state
            uint8_t *           _data;  //  array of "_sizeInBytes" bytes
        };

        //////////
        //  A RAM block whose content is kept entirely in host RAM
        class HADESVM_CEREON_PUBLIC ResidentRamBlock : public ResidentMemoryBlock
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ResidentRamBlock)

            //////////
            //  Constants
        public:
            static const uint64_t   DefaultStartAddress;
            static const MemorySize DefaultSize;

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
                virtual QString mnemonic() const override;
                virtual QString displayName() const override;

                //////////
                //  hadesvm::core::ComponentType
            public:
                virtual hadesvm::core::ComponentCategory * category() const override;
                virtual bool    suspendable() const override;
                virtual bool    isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const override;
                virtual bool    isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const override;
                virtual ResidentRamBlock *  createComponent() override;
            };

            //////////
            //  Construction/destruction
        public:
            ResidentRamBlock();
            virtual ~ResidentRamBlock() noexcept;

            //////////
            //  hadesvm::core::Component
        public:
            virtual Type *      type() const override { return Type::instance(); }
            virtual QString     displayName() const override;
            virtual ComponentEditor *   createEditor(QWidget * parent) override;
            virtual Ui *        createUi() override;
        };

        //////////
        //  A ROM block whose content is loaded from a file and kept entirely in host RAM
        class HADESVM_CEREON_PUBLIC ResidentRomBlock : public ResidentMemoryBlock
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ResidentRomBlock)

            //////////
            //  Constants
        public:
            static const uint64_t   DefaultStartAddress = 0xFFFFFFFFFFF00000;
            static const MemorySize DefaultSize;
            static const QString    DefailtContentFilePath;

            //////////
            //  Types
        public:
            //  The type of a resident ROM block
            class HADESVM_CEREON_PUBLIC Type final : public hadesvm::core::ComponentType
            {
                HADESVM_DECLARE_SINGLETON(Type)

                //////////
                //  hadesvm::core::ComponentType
            public:
                virtual hadesvm::core::ComponentCategory * category() const override;
                virtual bool    suspendable() const override;
                virtual bool    isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const override;
                virtual bool    isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const override;
                virtual ResidentRomBlock *  createComponent() override;
            };

            //////////
            //  Construction/destruction
        public:
            ResidentRomBlock();
            virtual ~ResidentRomBlock() noexcept;

            //////////
            //  hadesvm::core::Component
        public:
            virtual Type *      type() const override { return Type::instance(); }

            //////////
            //  MemoryBlock
        public:
            virtual void        storeByte(size_t offset, uint8_t value) throws(MemoryAccessError) override;
            virtual void        storeHalfWord(size_t offset, uint16_t value, ByteOrder byteOrder) throws(MemoryAccessError) override;
            virtual void        storeWord(size_t offset, uint32_t value, ByteOrder byteOrder) throws(MemoryAccessError) override;
            virtual void        storeLongWord(size_t offset, uint64_t value, ByteOrder byteOrder) throws(MemoryAccessError) override;

            //////////
            //  Operations (configuration)
        public:
            QString             contentFilePath() const { return _contentFilePath; }
            void                setContentFilePath(const QString & contentFilePath);

            //////////
            //  Implementation
        private:
            QString             _contentFilePath;   //  if relative, uses VM location dir as root

            //  Helpers
            QString             _resolveContentFilePath(const QString & path) const;
        };

#if 0
//  TODO define in a separate header
        //  The editor of resident ROM block properties
        class HADESVM_CEREON_PUBLIC ResidentRomBlockEditor final : public hadesvm::core::ComponentEditor
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ResidentRomBlockEditor)

            //////////
            //  Construction/destruction
        public:
            explicit ResidentRomBlockEditor(QWidget * parent);

            //////////
            //  hadesvm::core::ComponentEditor
        public:
            virtual void    loadComponentConfiguration() override;
            virtual bool    canSaveComponentConfiguration() const override;
            virtual void    saveComponentConfiguration() override;

            //////////
            //  Implementation
        private:
            ResidentRomBlock *  _residentRomBlock;  //  nullptr == none
        };
#endif
    }
}

//  End of hadesvm-cereon/Memory.hpp
