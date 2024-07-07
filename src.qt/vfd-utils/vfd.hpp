//
//  vfd.hpp - a Virtual Floppy Disk ADT
//
//////////

class FileSystem;

//////////
//  VFD formats & geometry
enum VfdFormat
{
    VfdFormat_Unknown   = 0,
    VfdFormat_144M      = 1     //  The 3.5' PC-compatible floppy
};

class VfdGeometry
{
    //////////
    //  Construction/destruction/assignment
public:
    VfdGeometry();
    VfdGeometry(unsigned int cylinders, unsigned int heads, unsigned int sectors, unsigned int sectorSize);
    VfdGeometry(const VfdGeometry & src);
    ~VfdGeometry() {}

    VfdGeometry &       operator = (const VfdGeometry & src);

    //////////
    //  Operations
public:
    //  Returns the number of cylinders, or 0 for "invalid" disk geometry
    unsigned int        getCylinders() const { return _cylinders; }

    //  Returns the number of heads, or 0 for "invalid" disk geometry
    unsigned int        getHeads() const { return _heads; }

    //  Returns the number of sectors per track, or 0 for "invalid" disk geometry
    unsigned int        getSectors() const { return _sectors; }

    //  Returns the sector size, in bytes, or 0 for "invalid" disk geometry
    unsigned int        getSectorSize() const { return _sectorSize; }

    //  Returns the geometry of a specified VFD format
    static VfdGeometry  forFormat(VfdFormat vfdFormat);

    //////////
    //  Implementation
private:
    unsigned int        _cylinders;
    unsigned int        _heads;
    unsigned int        _sectors;
    unsigned int        _sectorSize;
};

//////////
//  A Virtual Floppy Disk image
class Vfd
{
    //////////
    //  Can't assign or copy construct
private:
    Vfd(const Vfd &);
    void                    operator = (const Vfd &);

    //////////
    //  Construction/destruction
public:
    //  Opens an existing VFD image.
    //  Throws std::runtime_error if an error occurs.
    explicit Vfd(const string & fileName);

    //  Creates a new VFD image; overwrites existing files without prompt.
    //  Throws std::runtime_error if an error occurs.
    Vfd(const string & fileName, VfdFormat vfdFormat);

    //  Saves a VFD image if any changes were made to the VFD content since construction.
    //  Throws std::runtime_error if an error occurs.
    ~Vfd();

    //////////
    //  Operations
public:
    //  Returns the file name where this VFD is stored
    string                  getFileName() const { return _fileName; }

    //  Returns the format of this VFD
    VfdFormat               getFormat() const { return _format; }

    //  Returns the geometry of this VFD
    VfdGeometry             getGeometry() const { return _geometry; }

    //  Returns the total number of sectors this VFD has
    unsigned int            getNumberOfSectors() const { return _numberOfSectors; }

    //  Reads a value from the specified sector using the specified byte order
    uint8_t                 readUInt8(unsigned int lbaSector, unsigned int offset) const;
    uint16_t                readUInt16(unsigned int lbaSector, unsigned int offset, ByteOrder byteOrder) const;
    uint32_t                readUInt32(unsigned int lbaSector, unsigned int offset, ByteOrder byteOrder) const;
    void                    readData(unsigned int lbaSector, unsigned int offset, void * data, size_t size);

    //  Writes a value to the specified sector using the specified byte order
    void                    writeUInt8(unsigned int lbaSector, unsigned int offset, uint8_t value);
    void                    writeUInt16(unsigned int lbaSector, unsigned int offset, uint16_t value, ByteOrder byteOrder);
    void                    writeUInt32(unsigned int lbaSector, unsigned int offset, uint32_t value, ByteOrder byteOrder);
    void                    writeData(unsigned int lbaSector, unsigned int offset, const void * data, size_t size);

    //////////
    //  Implementation
private:
    string                  _fileName;
    VfdFormat               _format;
    VfdGeometry             _geometry;
    bool                    _modified;
    FileSystem *            _fileSystem;    //  managing this VFD, a private instance for each VFD

    //  Contents
    class _Sector
    {
    private:
        _Sector(const _Sector &);
        void                operator = (const _Sector &);
    public:
        _Sector(unsigned int size) : _data(new uint8_t[size]) { memset(_data, 0, size); }
        ~_Sector() { delete[] _data; }
        uint8_t *const      _data;  //  Array of _geometry.getSectorSize() bytes
    };
    unsigned int            _numberOfSectors;   //  tracks * heads * sectors
    _Sector **              _sectors;           //  array of _numberOfSectors elements


    //  Helpers
    void                    _allocateSectors();
    void                    _freeSectors();
    void                    _throwReadExceptionFromErrno();
    void                    _throwWriteExceptionFromErrno();
    void                    _readSectorData();
    void                    _writeSectorData();
    _Sector *               _getSector(unsigned int lbaSector) const;
};

//  End of vfd.hpp
