//
//  hadesvm-cereon/ResidentMemoryUnit.cpp
//
//  hadesvm::cereon::ResidentMemoryUnit class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Construction/destruction
ResidentMemoryUnit::ResidentMemoryUnit(uint64_t startAddress, const hadesvm::core::MemorySize & size)
    :   _memoryBlocks(),
        //  Configuration
        _startAddress(startAddress),
        _size(size),
        _sizeInBytes(static_cast<size_t>(size.toBytes())),
        _data(nullptr)
{
    _memoryBlocks.append(this);
    Q_ASSERT(_sizeInBytes == _size.toBytes());  //  Avoid overflow!
}

ResidentMemoryUnit::~ResidentMemoryUnit() noexcept
{
    delete [] _data;    //  just in case; "delete nullptr" is safe
}

//////////
//  hadesvm::core::Component
void ResidentMemoryUnit::serialiseConfiguration(QDomElement componentElement) const
{
    componentElement.setAttribute("StartAddress", hadesvm::util::toString(_startAddress, "%016X"));
    componentElement.setAttribute("Size", hadesvm::util::toString(_size));
}

void ResidentMemoryUnit::deserialiseConfiguration(QDomElement componentElement)
{
    uint64_t startAddress = _startAddress;
    if (hadesvm::util::fromString(componentElement.attribute("StartAddress"), "%X", startAddress))
    {   //  TODO validate startAddress - must be a multiple of 8
        _startAddress = startAddress;
    }

    hadesvm::core::MemorySize size = _size;
    if (hadesvm::util::fromString(componentElement.attribute("Size"), size))
    {   //  TODO validate size - must be a multiple of 8 and >0
        _size = size;
    }
}

//////////
//  hadesvm::core::Component (state management)
ResidentMemoryUnit::State ResidentMemoryUnit::state() const noexcept
{
    return _state;
}

void ResidentMemoryUnit::connect() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Constructed)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Connected;
}

void ResidentMemoryUnit::initialize() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    _data = new uint8_t[_sizeInBytes];
    memset(_data, 0, _sizeInBytes);

    _state = State::Initialized;
}

void ResidentMemoryUnit::start() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Running;
}

void ResidentMemoryUnit::stop() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Running)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Initialized;
}

void ResidentMemoryUnit::deinitialize() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    delete[] _data;
    _data = nullptr;

    _state = State::Connected;
}

void ResidentMemoryUnit::disconnect() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Constructed;
}

//////////
//  IMemoryBlock
uint8_t ResidentMemoryUnit::loadByte(size_t offset) throws(MemoryAccessError)
{
#ifndef QT_NO_DEBUG
    //  In release mode the ProcessorCore and MemoryBus take care of alignment/range issues TODO really ?
    if (offset >= _sizeInBytes)
    {
        throw MemoryAccessError::InvalidAddress;
    }
#endif
    return _data[offset];
}

uint16_t ResidentMemoryUnit::loadHalfWord(size_t offset, ByteOrder byteOrder) throws(MemoryAccessError)
{
    Q_ASSERT(byteOrder == ByteOrder::BigEndian || byteOrder == ByteOrder::LittleEndian);

#ifndef QT_NO_DEBUG
    //  In release mode the ProcessorCore and MemoryBus take care of alignment/range issues TODO really ?
    if (offset >= _sizeInBytes)
    {
        throw MemoryAccessError::InvalidAddress;
    }
    if ((offset & 0x01) != 0)
    {
        throw MemoryAccessError::InvalidAlignment;
    }
#endif
    uint16_t value = *reinterpret_cast<uint16_t*>(_data + offset);
    if (byteOrder != ByteOrder::Native)
    {
        value = std::byteswap<uint16_t>(value);
    }
    return value;
}

uint32_t ResidentMemoryUnit::loadWord(size_t offset, ByteOrder byteOrder) throws(MemoryAccessError)
{
    Q_ASSERT(byteOrder == ByteOrder::BigEndian || byteOrder == ByteOrder::LittleEndian);

#ifndef QT_NO_DEBUG
    //  In release mode the ProcessorCore and MemoryBus take care of alignment/range issues TODO really ?
    if (offset >= _sizeInBytes)
    {
        throw MemoryAccessError::InvalidAddress;
    }
    if ((offset & 0x03) != 0)
    {
        throw MemoryAccessError::InvalidAlignment;
    }
#endif
    uint32_t value = *reinterpret_cast<uint32_t*>(_data + offset);
    if (byteOrder != ByteOrder::Native)
    {
        value = std::byteswap<uint32_t>(value);
    }
    return value;
}

uint64_t ResidentMemoryUnit::loadLongWord(size_t offset, ByteOrder byteOrder) throws(MemoryAccessError)
{
    Q_ASSERT(byteOrder == ByteOrder::BigEndian || byteOrder == ByteOrder::LittleEndian);

#ifndef QT_NO_DEBUG
    //  In release mode the ProcessorCore and MemoryBus take care of alignment/range issues TODO really ?
    if (offset >= _sizeInBytes)
    {
        throw MemoryAccessError::InvalidAddress;
    }
    if ((offset & 0x07) != 0)
    {
        throw MemoryAccessError::InvalidAlignment;
    }
#endif
    uint64_t value = *reinterpret_cast<uint64_t*>(_data + offset);
    if (byteOrder != ByteOrder::Native)
    {
        value = std::byteswap<uint64_t>(value);
    }
    return value;
}

void ResidentMemoryUnit::storeByte(size_t offset, uint8_t value) throws(MemoryAccessError)
{
#ifndef QT_NO_DEBUG
    //  In release mode the ProcessorCore and MemoryBus take care of alignment/range issues TODO really ?
    if (offset >= _sizeInBytes)
    {
        throw MemoryAccessError::InvalidAddress;
    }
#endif
    _data[offset] = value;
}

void ResidentMemoryUnit::storeHalfWord(size_t offset, uint16_t value, ByteOrder byteOrder) throws(MemoryAccessError)
{
    Q_ASSERT(byteOrder == ByteOrder::BigEndian || byteOrder == ByteOrder::LittleEndian);

#ifndef QT_NO_DEBUG
    //  In release mode the ProcessorCore and MemoryBus take care of alignment/range issues TODO really ?
    if (offset >= _sizeInBytes)
    {
        throw MemoryAccessError::InvalidAddress;
    }
    if ((offset & 0x01) != 0)
    {
        throw MemoryAccessError::InvalidAlignment;
    }
#endif
    if (byteOrder != ByteOrder::Native)
    {
        value = std::byteswap<uint16_t>(value);
    }
    *reinterpret_cast<uint16_t*>(_data + offset) = value;
}

void ResidentMemoryUnit::storeWord(size_t offset, uint32_t value, ByteOrder byteOrder) throws(MemoryAccessError)
{
    Q_ASSERT(byteOrder == ByteOrder::BigEndian || byteOrder == ByteOrder::LittleEndian);

#ifndef QT_NO_DEBUG
    //  In release mode the ProcessorCore and MemoryBus take care of alignment/range issues TODO really ?
    if (offset >= _sizeInBytes)
    {
        throw MemoryAccessError::InvalidAddress;
    }
    if ((offset & 0x03) != 0)
    {
        throw MemoryAccessError::InvalidAlignment;
    }
#endif
    if (byteOrder != ByteOrder::Native)
    {
        value = std::byteswap<uint32_t>(value);
    }
    *reinterpret_cast<uint32_t*>(_data + offset) = value;
}

void ResidentMemoryUnit::storeLongWord(size_t offset, uint64_t value, ByteOrder byteOrder) throws(MemoryAccessError)
{
    Q_ASSERT(byteOrder == ByteOrder::BigEndian || byteOrder == ByteOrder::LittleEndian);

#ifndef QT_NO_DEBUG
    //  In release mode the ProcessorCore and MemoryBus take care of alignment/range issues TODO really ?
    if (offset >= _sizeInBytes)
    {
        throw MemoryAccessError::InvalidAddress;
    }
    if ((offset & 0x07) != 0)
    {
        throw MemoryAccessError::InvalidAlignment;
    }
#endif
    if (byteOrder != ByteOrder::Native)
    {
        value = std::byteswap<uint64_t>(value);
    }
    *reinterpret_cast<uint64_t*>(_data + offset) = value;
}

//////////
//  Operations (configuration)
void ResidentMemoryUnit::setStartAddress(uint64_t startAddress)
{
    Q_ASSERT(_state == State::Constructed);

    //  TODO validate "startAddress"
    _startAddress = startAddress;
}

void ResidentMemoryUnit::setSize(const hadesvm::core::MemorySize & size)
{
    Q_ASSERT(_state == State::Constructed);

    //  TODO Validate "size"
    _size = size;
    _sizeInBytes = static_cast<size_t>(_size.toBytes());
}

//  End of hadesvm-cereon/ResidentMemoryUnit.cpp
