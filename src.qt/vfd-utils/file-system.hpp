//
//  file-system.hpp - file system API
//
//////////

class FileSystem;

//////////
//  A generic file system type
class FileSystemType
{
    //////////
    //  Can't assign or copy construct
private:
    FileSystemType(const FileSystemType &);
    void                        operator = (const FileSystemType &);

    //////////
    //  Construction/destruction
public:
    FileSystemType() {}
    virtual ~FileSystemType() {}

    //////////
    //  Operations
public:
    //  The user-readable name of this file system type
    virtual string              getName() const = 0;

    //  Creates a new instance of this file system, not bound to any VFD
    virtual FileSystem *        createInstance() = 0;
};

//////////
//  A generic "file channel" - a channel through which a file
//  residing in a file system can be accessed
class FileChannel
{
    //////////
    //  Can't assign or copy construct
private:
    FileChannel(const FileChannel &);
    void                        operator = (const FileChannel &);

    //////////
    //  Construction (from derived classes only)/destruction
protected:
    FileChannel() {}
public:
    virtual ~FileChannel() {}

    //////////
    //  Operations
public:
    //  TODO document
    virtual long                getCurrentPosition() = 0;
    virtual void                setCurrentPosition(long position) = 0;
    virtual long                getSize() = 0;
    virtual bool                setSize(long size) = 0;
    virtual size_t              read(void * buffer, size_t bytesToRead) = 0;
    virtual size_t              write(const void * buffer, size_t bytesToWrite) = 0;
};

//////////
//  A directory entry in a file system
struct DirectoryEntry
{
    char                        name[13];               //  "name" or "name.ext" or "."/".."
    time_t                      creationTime;           //  date + time, UTC, 0 if not known
    time_t                      lastAccessTime;         //  date + time, UTC, 0 if not known
    time_t                      lastModificationTime;   //  date + time, UTC, 0 if not known
    uint32_t                    size;                   //  in bytes
    bool                        isDirectory;            //  true == directory
};
typedef vector<DirectoryEntry> DirectoryEntries;

//////////
//  A generic file system
class FileSystem
{
    //////////
    //  Can't assign or copy construct
private:
    FileSystem(const FileSystem &);
    void                        operator = (const FileSystem &);

    //////////
    //  Construction/destruction
public:
    FileSystem() : _vfd(NULL) {}
    virtual ~FileSystem() {}

    //////////
    //  Operations
public:
    //  Returns the type of this file system
    virtual FileSystemType *    getType() const = 0;

    //  Binds this file system to the specified VFD; NULL to un-bind
    virtual void                bind(Vfd * vfd);

    //  Returns the VFD to which this file system instance is bound, or NULL if none
    Vfd *                       getVfd() const { return _vfd; }

    //  Formats the bound VFD
    virtual void                formatVfd() = 0;

    //  "Loads" the bound VFD, preparing this file system for using it
    virtual void                loadVfd() = 0;

    //  TODO document
    virtual bool                exists(const char * fullPath) = 0;
    virtual bool                isFile(const char * fullPath) = 0;
    virtual bool                isDirectory(const char * fullPath) = 0;
    virtual bool                listDirectory(const char * fullPath, DirectoryEntries & directoryEntries) = 0;
    virtual bool                createDirectory(const char * fullPath) = 0;
    virtual bool                removeDirectory(const char * fullPath) = 0;
    virtual FileChannel *       createFile(const char * fullPath) = 0;
    virtual FileChannel *       openFile(const char * fullPath) = 0;
    virtual bool                deleteFile(const char * fullPath) = 0;

    //////////
    //  Implementation
private:
    Vfd *                       _vfd;   //  The VFD managed by this file system, NULL = none
};

//////////
//  The FAT12 file system type
class Fat12FileSystemType : public FileSystemType
{
    //////////
    //  Can't assign or copy construct
private:
    Fat12FileSystemType(const Fat12FileSystemType &);
    void                        operator = (const Fat12FileSystemType &);

    //////////
    //  Singleton
private:
    Fat12FileSystemType() {}
    virtual ~Fat12FileSystemType() {}
public:
    static Fat12FileSystemType *getInstance();

    //////////
    //  FileSystemType - Operations
public:
    virtual string              getName() const;
    virtual FileSystem *        createInstance();
};

//////////
//  A FAT12 file system
class Fat12FileChannel;
class Fat12FileSystem : public FileSystem
{
    friend class Fat12FileChannel;

    //////////
    //  Can't assign or copy construct
private:
    Fat12FileSystem(const Fat12FileSystem &);
    void                        operator = (const Fat12FileSystem &);

    //////////
    //  Constants
public:
    static const uint8_t        FileAttribute_ReadOnly = 0x01;
    static const uint8_t        FileAttribute_Hidden = 0x02;
    static const uint8_t        FileAttribute_System = 0x04;
    static const uint8_t        FileAttribute_VolumeLabel = 0x08;
    static const uint8_t        FileAttribute_Subdirectory = 0x10;
    static const uint8_t        FileAttribute_Archive = 0x20;

    static const uint8_t        FileAttribute_None = 0x00;
    static const uint8_t        FileAttribute_All = 0x3F;

    //////////
    //  Construction/destruction
public:
    Fat12FileSystem();
    virtual ~Fat12FileSystem();

    //////////
    //  FileSystem - Operations
public:
    virtual FileSystemType *    getType() const;
    virtual void                bind(Vfd * vfd);
    virtual void                formatVfd();
    virtual void                loadVfd();
    virtual bool                exists(const char * fullPath);
    virtual bool                isFile(const char * fullPath);
    virtual bool                isDirectory(const char * fullPath);
    virtual bool                listDirectory(const char * fullPath, DirectoryEntries & directoryEntries);
    virtual bool                createDirectory(const char * fullPath);
    virtual bool                removeDirectory(const char * fullPath);
    virtual FileChannel *       createFile(const char * fullPath);
    virtual FileChannel *       openFile(const char * fullPath);
    virtual bool                deleteFile(const char * fullPath);

    //////////
    //  Operations
public:
    bool                        getFileClusters(const char * fullPath, vector<uint16_t> & clusterNumbers);
    uint16_t                    getLbaSectorForClusterNumber(uint16_t clusterNumber);
    bool                        setFileAttributes(const char * fullPath, uint8_t attributes);

    //////////
    //  Implementation
private:
    //  Constants
    static const unsigned int   _OemNameOffset = 3;
    static const unsigned int   _BytesPerSectorOffset = 11;
    static const unsigned int   _SectorsPerClusterOffset = 13;
    static const unsigned int   _ReservedSectorsOffset = 14;
    static const unsigned int   _FatCopiesOffset = 16;
    static const unsigned int   _RootDirectoryEntriesOffset = 17;
    static const unsigned int   _TotalSectorsOffset = 19;
    static const unsigned int   _MediaTypeOffset = 21;
    static const unsigned int   _SectorsPerFatOffset = 22;
    static const unsigned int   _SectorsPerTrackOffset = 24;
    static const unsigned int   _HeadsOffset = 26;
    static const unsigned int   _HiddenSectorsOffset = 28;
    static const unsigned int   _BootSignatureOffset = 38;
    static const unsigned int   _VolumeIdOffset = 39;
    static const unsigned int   _VolumeLabelOffset = 43;
    static const unsigned int   _FileSystemTypeOffset = 54;

    static const uint8_t        _MediaType_144M = 0xF0;

    static const char           _DirEntryNamePrefix_NoMore = 0x00;
    static const char           _DirEntryNamePrefix_E5 = 0x05;
    static const char           _DirEntryNamePrefix_Deleted = static_cast<char>(0xE5);

    //  FAT entry classification
    static const unsigned int   _FatEntry_FreeCluster = 0x0000; //  canonical value
    static const unsigned int   _FatEntry_BadCluster = 0x0FF7;  //  canonical value
    static const unsigned int   _FatEntry_LastCluster = 0x0FFF; //  canonical value

    //  Types
    struct _DirectoryEntryImpl
    {
        char                    _name[8];           //  right-padded with spaces
        char                    _extension[3];      //  right-padded with spaces
        uint8_t                 _flags;
        uint16_t                _lastModifiedTime;  //  in MS-DOS encoding
        uint16_t                _lastModifiedDate;  //  in MS-DOS encoding
        uint16_t                _firstCluster;      //  0 for ".." entry of a root directory's subdirectory
        uint32_t                _size;

        //////////
        //  Construction.

        //  Sets everything to 0's, including "_name" and "_extension"
        _DirectoryEntryImpl();

        //  Creates a live entry. "_name" ane "_extension" are all-0 if "fullName" is e.g. too long, etc.
        _DirectoryEntryImpl(const char * fullName, uint8_t flags, time_t lastModifiedTime, unsigned int firstCluster);

        //////////
        //  Operations
        void                    _storeFullName(char fullName[13]) const;    //  stores "name.extension" into "fullName"
        bool                    _match(const char * fileName) const;
        bool                    _setFullName(const char * fullName);        //  stores "fullName" into "_name" and "_extension"
    };

    //  File system state - only used when file system is bound to a VFD, otherwise are all 0
    unsigned int                _sectorSize;
    unsigned int                _totalSectors;          //  on the VFD
    unsigned int                _reservedSectors;       //  at the beginning of the VFD
    unsigned int                _rootDirectoryEntryCount;
    unsigned int                _directoryEntriesPerSector;
    unsigned int                _rootDirectorySectors;
    unsigned int                _numberOfFatCopies;     //  >= 1
    unsigned int                _fatEntriesPerSector;
    unsigned int                _sectorsPerFat;         //  sectors needed for a single FAT copy
    unsigned int                _totalFatEntries;       //  needed for the entire data area of the disk
    unsigned int                _firstDataAreaSector;   //  LBA sector number of the 1st sector of data area
    unsigned int                _sectorsPerCluster;
    unsigned int                _totalDataSectors;
    unsigned int                _totalDataClusters;
    unsigned int                _firstRootDirectorySector;

    vector<Fat12FileChannel*>   _activeFileChannels;

    //  Helpers
    void                        _setupParameters(); //  Can throw runtime_error
    bool                        _isOperational() const;

    unsigned int                _readFatEntry(unsigned int index);
    void                        _writeFatEntry(unsigned int index, unsigned int value);
    static bool                 _isFreeClusterFatEntry(unsigned int entry);
    static bool                 _isReservedClusterFatEntry(unsigned int entry);
    static bool                 _isBadClusterFatEntry(unsigned int entry);
    static bool                 _isLastClusterFatEntry(unsigned int entry);
    static bool                 _isDataClusterFatEntry(unsigned int entry);
    unsigned int                _findUnusedCluster();   //  returns 0 if not found
    void                        _clearCluster(unsigned int clusterIndex);

    static void                 _encodeTime(time_t t, uint16_t & date, uint16_t & time);
    static time_t               _decodeTime(uint16_t date, uint16_t time);
    void                        _readDirectoryEntry(unsigned int lbaSector, unsigned int entryIndex, _DirectoryEntryImpl & directoryEntryImpl);
    void                        _writeDirectoryEntry(unsigned int lbaSector, unsigned int entryIndex, const _DirectoryEntryImpl & directoryEntryImpl);

    bool                        _splitAbsolutePath(const char * path, vector<string> & pathComponents);
    const char *                _findLastPathComponent(const char * path);
    bool                        _findRootDirectoryEntryByName(const char * name, unsigned int & lbaSector, unsigned int & entryIndex, _DirectoryEntryImpl & directoryEntryImpl);
    bool                        _findDirectoryEntryByName(const _DirectoryEntryImpl & parentDirectoryEntryImpl, const char * name, unsigned int & lbaSector, unsigned int & entryIndex, _DirectoryEntryImpl & directoryEntryImpl);
    bool                        _findDirectoryEntryForPath(const char * fullPath, unsigned int & lbaSector, unsigned int & entryIndex, _DirectoryEntryImpl & directoryEntryImpl);
    bool                        _findDirectoryEntryForParentOfPath(const char * fullPath, unsigned int & lbaSector, unsigned int & entryIndex, _DirectoryEntryImpl & directoryEntryImpl);

    void                        _closeAllFileChannels();

    unsigned int                _createContentClusterForNewDirectory(unsigned int parentDirectoryClusterIndex); //  returns cluster index (>=2), or 0 on failure
    unsigned int                _grabNewContentClusterForFile(); //  returns cluster index (>=2), or 0 on failure
    bool                        _insertDirectoryEntryIntoRootDirectory(const _DirectoryEntryImpl & newDirectoryEntryImpl);
    bool                        _insertDirectoryEntryIntoDirectory(_DirectoryEntryImpl & parentDirectoryEntryImpl, const _DirectoryEntryImpl & newDirectoryEntryImpl);
    DirectoryEntry              _translateDirectoryEntry(const _DirectoryEntryImpl & directoryEntryImpl);

    unsigned int                _getUnusedClustersCount();
    bool                        _setFileSize(unsigned int lbaSector, unsigned int entryIndex, long size);
    unsigned int                _getClusterCountForFileSize(long size);

    void                        _writeBytes(const _DirectoryEntryImpl & directoryEntryImpl, uint32_t offset, const uint8_t * bytes, uint32_t numBytes);
};

//////////
//  A FAT12 "file channel" - a channel to a FAT12 file
class Fat12FileChannel : public FileChannel
{
    friend class Fat12FileSystem;

    //////////
    //  Can't assign or copy construct
private:
    Fat12FileChannel(const Fat12FileChannel &);
    void                        operator = (const Fat12FileChannel &);

    //////////
    //  Construction (from friend classes only)/destruction
private:
    Fat12FileChannel(Fat12FileSystem * fileSystem,
                     unsigned int directoryEntryLbaSector,
                     unsigned int directoryEntryIndex);
public:
    virtual ~Fat12FileChannel();

    //////////
    //  FileChannel
public:
    virtual long                getCurrentPosition();
    virtual void                setCurrentPosition(long position);
    virtual long                getSize();
    virtual bool                setSize(long size);
    virtual size_t              read(void * buffer, size_t bytesToRead);
    virtual size_t              write(const void * buffer, size_t bytesToWrite);

    //////////
    //  Implementation
private:
    Fat12FileSystem *           _fileSystem;
    unsigned int                _directoryEntryLbaSector;
    unsigned int                _directoryEntryIndex;   //  in the above sector
    long                        _currentPosition;       //  for this clannel, always >= 0, may be beyond EOF
};

//  End of file-system.hpp
