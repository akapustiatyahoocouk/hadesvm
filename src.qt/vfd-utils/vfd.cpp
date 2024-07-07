//
//  vfd.cpp - VFD implementation
//
//////////
#include "main.hpp"

//////////
//  Construction/destruction
Vfd::Vfd(const string & fileName)
    :   _fileName(fileName),
        _format(VfdFormat_Unknown),
        _geometry(),
        _modified(false),
        _fileSystem(NULL),
        _numberOfSectors(0),
        _sectors(NULL)
{
    //  Determine the VFD format & geometry
    struct stat st;
    if (stat(_fileName.c_str(), &st) != 0)
    {
        _throwReadExceptionFromErrno();
    }
    if (st.st_size == 80 * 2 * 18 * 512)
    {   //  Looks like 1.44M
        _format = VfdFormat_144M;
        _geometry = VfdGeometry::forFormat(_format);
    }
    else
    {
        throw runtime_error("The file '" + _fileName + "' has invalid size");
    }
    //  Allocate sectors & read contents
    _allocateSectors();
    _readSectorData();
    //  Determine the file system type
    if (_sectors[0]->_data[510] == 0x55 &&
        _sectors[0]->_data[511] == 0xAA)
    {   //  Boot sector signature correct
        switch (_format)
        {
            case VfdFormat_144M:
                _fileSystem = Fat12FileSystemType::getInstance()->createInstance();
                break;
            case VfdFormat_Unknown:
            default:
                assert(false);
        }
        _fileSystem->bind(this);
        //  Make sure the disk content is compatible with the file system
        //  TODO
    }
    else
    {   //  OOPS! Wrong boot sector signature
        throw runtime_error("The file '" + _fileName + "' has invalid boot sector");
    }
    //  Recognize the disk
    _fileSystem->loadVfd();
}

Vfd::Vfd(const string & fileName, VfdFormat vfdFormat)
    :   _fileName(fileName),
        _format(vfdFormat),
        _geometry(VfdGeometry::forFormat(vfdFormat)),
        _modified(false),
        _fileSystem(NULL),
        _numberOfSectors(0),
        _sectors(NULL)
{
    assert(_format != VfdFormat_Unknown);
    assert(_geometry.getSectorSize() == 512);

    //  Allocate sectors
    _allocateSectors();
    //  Set up an empty file system
    switch (_format)
    {
        case VfdFormat_144M:
            _fileSystem = Fat12FileSystemType::getInstance()->createInstance();
            break;
        case VfdFormat_Unknown:
        default:
            assert(false);
    }
    _fileSystem->bind(this);
    //  Make create initial disk content as required by the file system
    _fileSystem->formatVfd();
    //  Save & we're done
    _writeSectorData();
}

Vfd::~Vfd()
{
    //  Save if necessary
    if (_modified)
    {
        _writeSectorData();
    }
    //  Cleanup
    delete _fileSystem;
    _freeSectors();
}

//////////
//  Operations
uint8_t Vfd::readUInt8(unsigned int lbaSector, unsigned int offset) const
{
    _Sector * sector = _getSector(lbaSector);
    assert(offset < static_cast<size_t>(_geometry.getSectorSize()));

    return sector->_data[offset];
}

uint16_t Vfd::readUInt16(unsigned int lbaSector, unsigned int offset, ByteOrder byteOrder) const
{
    uint16_t b0, b1;

    switch (byteOrder)
    {
        case ByteOrder_BigEndian:
            b0 = static_cast<uint16_t>(readUInt8(lbaSector, offset + 0) << 8);
            b1 = static_cast<uint16_t>(readUInt8(lbaSector, offset + 1) << 0);
            break;
        case ByteOrder_LittleEndian:
            b0 = static_cast<uint16_t>(readUInt8(lbaSector, offset + 0) << 0);
            b1 = static_cast<uint16_t>(readUInt8(lbaSector, offset + 1) << 8);
            break;
        default:
            assert(false);
    }
    return b0 | b1;
}

uint32_t Vfd::readUInt32(unsigned int lbaSector, unsigned int offset, ByteOrder byteOrder) const
{
    uint32_t b0, b1, b2, b3;

    switch (byteOrder)
    {
        case ByteOrder_BigEndian:
            b0 = static_cast<uint32_t>(readUInt8(lbaSector, offset + 0) << 24);
            b1 = static_cast<uint32_t>(readUInt8(lbaSector, offset + 1) << 16);
            b2 = static_cast<uint32_t>(readUInt8(lbaSector, offset + 2) <<  8);
            b3 = static_cast<uint32_t>(readUInt8(lbaSector, offset + 3) <<  0);
            break;
        case ByteOrder_LittleEndian:
            b0 = static_cast<uint32_t>(readUInt8(lbaSector, offset + 0) <<  0);
            b1 = static_cast<uint32_t>(readUInt8(lbaSector, offset + 1) <<  8);
            b2 = static_cast<uint32_t>(readUInt8(lbaSector, offset + 2) << 16);
            b3 = static_cast<uint32_t>(readUInt8(lbaSector, offset + 3) << 24);
            break;
        default:
            assert(false);
    }
    return b0 | b1 | b2 | b3;
}

void Vfd::readData(unsigned int lbaSector, unsigned int offset, void * data, size_t size)
{
    uint8_t * bytes = static_cast<uint8_t *>(data);
    for (unsigned int i = 0; i < size; i++)
    {
        bytes[i] = readUInt8(lbaSector, offset + i);
    }
}

void Vfd::writeUInt8(unsigned int lbaSector, unsigned int offset, uint8_t value)
{
    _Sector * sector = _getSector(lbaSector);
    assert(offset < static_cast<size_t>(_geometry.getSectorSize()));

    if (sector->_data[offset] != value)
    {
        sector->_data[offset] = value;
        _modified = true;
    }
}

void Vfd::writeUInt16(unsigned int lbaSector, unsigned int offset, uint16_t value, ByteOrder byteOrder)
{
    switch (byteOrder)
    {
        case ByteOrder_BigEndian:
            writeUInt8(lbaSector, offset + 0, static_cast<uint8_t>(value >> 8));
            writeUInt8(lbaSector, offset + 1, static_cast<uint8_t>(value >> 0));
            break;
        case ByteOrder_LittleEndian:
            writeUInt8(lbaSector, offset + 0, static_cast<uint8_t>(value >> 0));
            writeUInt8(lbaSector, offset + 1, static_cast<uint8_t>(value >> 8));
            break;
        default:
            assert(false);
    }
}

void Vfd::writeUInt32(unsigned int lbaSector, unsigned int offset, uint32_t value, ByteOrder byteOrder)
{
    switch (byteOrder)
    {
        case ByteOrder_BigEndian:
            writeUInt8(lbaSector, offset + 0, static_cast<uint8_t>(value >> 24));
            writeUInt8(lbaSector, offset + 1, static_cast<uint8_t>(value >> 16));
            writeUInt8(lbaSector, offset + 2, static_cast<uint8_t>(value >>  8));
            writeUInt8(lbaSector, offset + 3, static_cast<uint8_t>(value >>  0));
            break;
        case ByteOrder_LittleEndian:
            writeUInt8(lbaSector, offset + 0, static_cast<uint8_t>(value >>  0));
            writeUInt8(lbaSector, offset + 1, static_cast<uint8_t>(value >>  8));
            writeUInt8(lbaSector, offset + 2, static_cast<uint8_t>(value >> 16));
            writeUInt8(lbaSector, offset + 3, static_cast<uint8_t>(value >> 24));
            break;
        default:
            assert(false);
    }
}

void Vfd::writeData(unsigned int lbaSector, unsigned int offset, const void * data, size_t size)
{
    assert(size <= _geometry.getSectorSize());

    const uint8_t * bytes = static_cast<const uint8_t *>(data);
    for (unsigned int i = 0; i < size; i++)
    {
        writeUInt8(lbaSector, offset + i, bytes[i]);
    }
}

//////////
//  Helpers
void Vfd::_allocateSectors()
{
    assert(_numberOfSectors == 0);
    assert(_sectors == NULL);

    _numberOfSectors = _geometry.getCylinders() * _geometry.getHeads() * _geometry.getSectors();
    assert(_numberOfSectors != 0);
    _sectors = new _Sector*[_numberOfSectors];
    for (size_t i = 0; i < _numberOfSectors; i++)
    {
        _sectors[i] = new _Sector(_geometry.getSectorSize());
    }
}

void Vfd::_freeSectors()
{
    if (_sectors != NULL)
    {
        for (size_t i = 0; i < _numberOfSectors; i++)
        {
            delete _sectors[i];
        }
        delete[] _sectors;
        _sectors = NULL;
        _numberOfSectors = 0;
    }
    assert(_numberOfSectors == 0);
}

void Vfd::_throwReadExceptionFromErrno()
{
    throw runtime_error("Can't read'" + _fileName + "': " + strerror(errno));
}

void Vfd::_throwWriteExceptionFromErrno()
{
    throw runtime_error("Can't write '" + _fileName + "': " + strerror(errno));
}

void Vfd::_readSectorData()
{
    FILE * f = fopen(_fileName.c_str(), "rb");
    if (f == NULL)
    {   //  OOPS!
        _throwReadExceptionFromErrno();
    }
    for (size_t i = 0; i < _numberOfSectors; i++)
    {
        if (fread(_sectors[i]->_data, 1, _geometry.getSectorSize(), f) != _geometry.getSectorSize())
        {   //  OOPS!
            int savedErrno = errno;
            fclose(f);
            errno = savedErrno;
            _throwReadExceptionFromErrno();
        }
    }
    if (fclose(f) != 0)
    {
        _throwReadExceptionFromErrno();
    }
    _modified = false;
}

void Vfd::_writeSectorData()
{
    FILE * f = fopen(_fileName.c_str(), "wb");
    if (f == NULL)
    {   //  OOPS!
        _throwWriteExceptionFromErrno();
    }
    for (size_t i = 0; i < _numberOfSectors; i++)
    {
        if (fwrite(_sectors[i]->_data, 1, _geometry.getSectorSize(), f) != _geometry.getSectorSize())
        {   //  OOPS!
            int savedErrno = errno;
            fclose(f);
            errno = savedErrno;
            _throwWriteExceptionFromErrno();
        }
    }
    if (fclose(f) != 0)
    {
        _throwWriteExceptionFromErrno();
    }
    _modified = false;
}

Vfd::_Sector * Vfd::_getSector(unsigned int lbaSector) const
{
    assert(lbaSector < _numberOfSectors);

    return _sectors[lbaSector];
}

//  End of vfd.cpp
