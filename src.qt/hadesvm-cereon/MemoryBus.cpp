//
//  hadesvm-cereon/MemoryBus.cpp
//
//  hadesvm::cereon::MemoryBus class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Construction/destruction
MemoryBus::MemoryBus()
    :   _mappings(new _Mapping[1]),
        _endMappings(_mappings)
{
}

MemoryBus::~MemoryBus() noexcept
{
    delete [] _mappings;
}

//////////
//  hadesvm::core::Component
QString MemoryBus::displayName() const
{
    return Type::instance()->displayName();
}

void MemoryBus::serialiseConfiguration(QDomElement /*componentElement*/) const
{   //  Nothing to do
}

void MemoryBus::deserialiseConfiguration(QDomElement /*componentElement*/)
{   //  Nothing to do
}

ComponentEditor * MemoryBus::createEditor(QWidget * /*parent*/)
{
    return nullptr;
}

MemoryBus::Ui * MemoryBus::createUi()
{
    return nullptr;
}

//////////
//  hadesvm::core::Component (state management)
MemoryBus::State MemoryBus::state() const noexcept
{
    return _state;
}

void MemoryBus::connect() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Constructed)
    {   //  OOPS! Can't
        return;
    }

    //  Locate all "MemoryBlock" components and attach them to this memory bus
    for (MemoryBlock * memoryBlock : this->virtualAppliance()->componentsImplementing<MemoryBlock>())
    {
        attachMemoryBlock(memoryBlock);
    }

    _state = State::Connected;
}

void MemoryBus::initialize() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Initialized;
}

void MemoryBus::start() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Running;
}

void MemoryBus::stop() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Running)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Initialized;
}

void MemoryBus::deinitialize() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Connected;
}

void MemoryBus::disconnect() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    while (_endMappings != _mappings)
    {
        detachMemoryBlock(_mappings->_memoryBlock);
    }

    _state = State::Constructed;
}

//////////
//  Operations
void MemoryBus::attachMemoryBlock(MemoryBlock * memoryBlock) throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(memoryBlock != nullptr);
    Q_ASSERT(memoryBlock->startAddress() % 8 == 0);
    Q_ASSERT(memoryBlock->size().toBytes() != 0);
    Q_ASSERT(memoryBlock->size().toBytes() % 8 == 0);

    uint64_t sizeInBytes = memoryBlock->size().toBytes();
    uint64_t endAddress = memoryBlock->startAddress() + sizeInBytes - 1;

    if (sizeInBytes == 0 ||
        sizeInBytes % 8 != 0 ||
        memoryBlock->startAddress() % 8 != 0 ||
        endAddress < memoryBlock->startAddress())
    {   //  Be defensive in release mode
        throw hadesvm::core::VirtualApplianceException("Inconsistent memory block");
    }

    //  If already mapped, do nothing
    for (_Mapping * existingMapping = _mappings; existingMapping < _endMappings; existingMapping++)
    {
        if (existingMapping->_memoryBlock == memoryBlock)
        {
            return;
        }
    }

    //  Make sure there is no conflict in covered range(s)
    for (_Mapping * existingMapping = _mappings; existingMapping < _endMappings; existingMapping++)
    {
        uint64_t intersectionStartAddress = qMax(existingMapping->_startAddress, memoryBlock->startAddress());
        uint64_t intersectionEndAddress = qMin(existingMapping->_endAddress, endAddress);
        if (intersectionStartAddress <= intersectionEndAddress)
        {   //  OOPS!
            QString range = hadesvm::util::toString(intersectionStartAddress, "%016X") +
                            ".." +
                            hadesvm::util::toString(intersectionEndAddress, "%016X");
            throw hadesvm::core::VirtualApplianceException("Memory blocks conflict in range " + range);
        }
    }

    //  Add a new mapping...
    size_t numMappings = _endMappings - _mappings;
    _Mapping * newMappings = new _Mapping[numMappings + 1];
    for (size_t i = 0; i < numMappings; i++)
    {
        newMappings[i] = _mappings[i];
    }
    newMappings[numMappings]._startAddress = memoryBlock->startAddress();
    newMappings[numMappings]._endAddress = endAddress;
    newMappings[numMappings]._memoryBlock = memoryBlock;
    numMappings++;
    //  ...replacing the mappings list
    delete [] _mappings;
    _mappings = newMappings;
    _endMappings = _mappings + numMappings;
}

void MemoryBus::detachMemoryBlock(MemoryBlock * memoryBlock)
{
    Q_ASSERT(memoryBlock != nullptr);

    size_t index = SIZE_MAX;
    for (_Mapping * existingMapping = _mappings; existingMapping < _endMappings; existingMapping++)
    {
        if (existingMapping->_memoryBlock == memoryBlock)
        {
            index = existingMapping - _mappings;
            break;
        }
    }
    if (index == SIZE_MAX)
    {   //  Not mapped
        return;
    }

    size_t numMappings = _endMappings - _mappings;
    for (size_t i = index; i + 1 < numMappings; i++)
    {
        _mappings[i] = _mappings[i + 1];
    }
    _endMappings--;
}

uint8_t MemoryBus::loadByte(uint64_t address) throws(MemoryAccessError)
{
    if (_Mapping * mapping = _findMapping(address))
    {
        return mapping->_memoryBlock->loadByte(static_cast<size_t>(address - mapping->_startAddress));
    }
    throw MemoryAccessError::InvalidAddress;
}

uint16_t MemoryBus::loadHalfWord(uint64_t address, ByteOrder byteOrder) throws(MemoryAccessError)
{
    if (_Mapping * mapping = _findMapping(address))
    {
        return mapping->_memoryBlock->loadHalfWord(static_cast<size_t>(address - mapping->_startAddress), byteOrder);
    }
    throw MemoryAccessError::InvalidAddress;
}

uint32_t MemoryBus::loadWord(uint64_t address, ByteOrder byteOrder) throws(MemoryAccessError)
{
    if (_Mapping * mapping = _findMapping(address))
    {
        return mapping->_memoryBlock->loadWord(static_cast<size_t>(address - mapping->_startAddress), byteOrder);
    }
    throw MemoryAccessError::InvalidAddress;
}

uint64_t MemoryBus::loadLongWord(uint64_t address, ByteOrder byteOrder) throws(MemoryAccessError)
{
    if (_Mapping * mapping = _findMapping(address))
    {
        return mapping->_memoryBlock->loadLongWord(static_cast<size_t>(address - mapping->_startAddress), byteOrder);
    }
    throw MemoryAccessError::InvalidAddress;
}

void MemoryBus::storeByte(uint64_t address, uint8_t value) throws(MemoryAccessError)
{
    if (_Mapping * mapping = _findMapping(address))
    {
        mapping->_memoryBlock->storeByte(static_cast<size_t>(address - mapping->_startAddress), value);
        return;
    }
    throw MemoryAccessError::InvalidAddress;
}

void MemoryBus::storeHalfWord(uint64_t address, uint16_t value, ByteOrder byteOrder) throws(MemoryAccessError)
{
    if (_Mapping * mapping = _findMapping(address))
    {
        mapping->_memoryBlock->storeHalfWord(static_cast<size_t>(address - mapping->_startAddress), value, byteOrder);
        return;
    }
    throw MemoryAccessError::InvalidAddress;
}

void MemoryBus::storeWord(uint64_t address, uint32_t value, ByteOrder byteOrder) throws(MemoryAccessError)
{
    if (_Mapping * mapping = _findMapping(address))
    {
        mapping->_memoryBlock->storeWord(static_cast<size_t>(address - mapping->_startAddress), value, byteOrder);
        return;
    }
    throw MemoryAccessError::InvalidAddress;
}

void MemoryBus::storeLongWord(uint64_t address, uint64_t value, ByteOrder byteOrder) throws(MemoryAccessError)
{
    if (_Mapping * mapping = _findMapping(address))
    {
        mapping->_memoryBlock->storeLongWord(static_cast<size_t>(address - mapping->_startAddress), value, byteOrder);
        return;
    }
    throw MemoryAccessError::InvalidAddress;
}

//////////
//  hadesvm::cereon::MemoryBus::Type
HADESVM_IMPLEMENT_SINGLETON(MemoryBus::Type)
MemoryBus::Type::Type() {}
MemoryBus::Type::~Type() {}

QString MemoryBus::Type::mnemonic() const
{
    return "CereonMemoryBus";
}

QString MemoryBus::Type::displayName() const
{
    return "Cereon Memory Bus";
}

hadesvm::core::ComponentCategory * MemoryBus::Type::category() const
{
    return StandardComponentCategories::Memory;
}

bool MemoryBus::Type::suspendable() const
{
    return true;
}

bool MemoryBus::Type::isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const
{
    return architecture == CereonWorkstationArchitecture::instance();
}

bool MemoryBus::Type::isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const
{
    return type == hadesvm::core::VirtualMachineType::instance();
}

MemoryBus * MemoryBus::Type::createComponent()
{
    return new MemoryBus();
}

//  End of hadesvm-cereon/MemoryBus.cpp
