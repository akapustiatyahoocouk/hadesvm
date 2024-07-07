//
//  fat12-file-system.cpp - FAT12 implementation
//
//////////
#include "main.hpp"

namespace
{
    #include "data/boot-sector-ibmpc-hades.bin.hxx"
}

//////////
//  File system type
Fat12FileSystemType * Fat12FileSystemType::getInstance()
{
    static Fat12FileSystemType instance;
    return &instance;
}

string Fat12FileSystemType::getName() const
{
    return "FAT12";
}

FileSystem * Fat12FileSystemType::createInstance()
{
    return new Fat12FileSystem();
}

//////////
//  Construction/destruction
Fat12FileSystem::Fat12FileSystem()
    :   _sectorSize(0),
        _totalSectors(0),
        _reservedSectors(0),
        _rootDirectoryEntryCount(0),
        _directoryEntriesPerSector(0),
        _rootDirectorySectors(0),
        _numberOfFatCopies(0),
        _fatEntriesPerSector(0),
        _sectorsPerFat(0),
        _totalFatEntries(0),
        _firstDataAreaSector(0),
        _sectorsPerCluster(0),
        _totalDataSectors(0),
        _totalDataClusters(0),
        _firstRootDirectorySector(0),
        _activeFileChannels()
{
}

Fat12FileSystem::~Fat12FileSystem()
{
    _closeAllFileChannels();
}

//////////
//  FileSystem - Operations
FileSystemType * Fat12FileSystem::getType() const
{
    return Fat12FileSystemType::getInstance();
}

void Fat12FileSystem::bind(Vfd * vfd)
{
    Vfd * oldVfd = getVfd();
    if (vfd == oldVfd)
    {
        return;
    }
    //  The bound VFD is actually being changed.
    //  First of all, we want to close any files still open on the old VFD
    if (oldVfd != NULL)
    {
        _closeAllFileChannels();
    }
    //  Now we can re-bind to another VFD (or un-bind, if "vfd" is NULL)
    FileSystem::bind(vfd);
    //  If the new binding is NULL, drop the file system state
    _sectorSize = 0;
    _totalSectors = 0;
    _reservedSectors = 0;
    _rootDirectoryEntryCount = 0;
    _directoryEntriesPerSector = 0;
    _rootDirectorySectors = 0;
    _numberOfFatCopies = 0;
    _fatEntriesPerSector = 0;
    _sectorsPerFat = 0;
    _totalFatEntries = 0;
    _firstDataAreaSector = 0;
    _sectorsPerCluster = 0;
    _totalDataSectors = 0;
    _totalDataClusters = 0;
    _firstRootDirectorySector = 0;
}

void Fat12FileSystem::formatVfd()
{
    Vfd * vfd = getVfd();
    assert(vfd != NULL);

    unsigned int sectorSize = vfd->getGeometry().getSectorSize();
    assert(sectorSize == 512);

    //  Clear everything
    for (unsigned int lbaSector = 0; lbaSector < vfd->getNumberOfSectors(); lbaSector++)
    {
        for (unsigned int offset = 0; offset < sectorSize; offset++)
        {
            vfd->writeUInt8(lbaSector, offset, 0);
        }
    }

    //  Copy boot sector
    assert(_________src_qt_vfd_utils_data_boot_sector_ibmpc_hades_bin_len == sectorSize);
    for (unsigned int offset = 0; offset < sectorSize; offset++)
    {
        vfd->writeUInt8(0, offset, _________src_qt_vfd_utils_data_boot_sector_ibmpc_hades_bin[offset]);
    }

    //  Write predefined FAT12 values
    vfd->writeData(0, _OemNameOffset, "VFDUTILS", 8);
    vfd->writeUInt16(0, _BytesPerSectorOffset, static_cast<uint16_t>(sectorSize), ByteOrder_LittleEndian);
    vfd->writeUInt8(0, _SectorsPerClusterOffset, 1);
    vfd->writeUInt16(0, _ReservedSectorsOffset, 1, ByteOrder_LittleEndian);
    vfd->writeUInt8(0, _FatCopiesOffset, 2);
    vfd->writeUInt16(0, _RootDirectoryEntriesOffset, 224, ByteOrder_LittleEndian);
    vfd->writeUInt16(0, _TotalSectorsOffset, static_cast<uint16_t>(vfd->getNumberOfSectors()), ByteOrder_LittleEndian);
    switch (vfd->getFormat())
    {
        case VfdFormat_144M:
            vfd->writeUInt8(0, _MediaTypeOffset, _MediaType_144M);
            break;
        case VfdFormat_Unknown:
        default:
            assert(false);
    }
    unsigned int fatEntriesPerSector = (sectorSize * 8) / 12;
    unsigned int fatSectors = (vfd->getNumberOfSectors() + fatEntriesPerSector - 1) / fatEntriesPerSector;
    vfd->writeUInt16(0, _SectorsPerFatOffset, static_cast<uint16_t>(fatSectors), ByteOrder_LittleEndian);
    vfd->writeUInt16(0, _SectorsPerTrackOffset, static_cast<uint16_t>(vfd->getGeometry().getSectors()), ByteOrder_LittleEndian);
    vfd->writeUInt16(0, _HeadsOffset, static_cast<uint16_t>(vfd->getGeometry().getHeads()), ByteOrder_LittleEndian);
    vfd->writeUInt16(0, _HiddenSectorsOffset, 0, ByteOrder_LittleEndian);

    //  Set up this file system to work with the bound disk
    _setupParameters();

    //  Clear all FAT entries and set up the first two
    for (unsigned int i = 0; i < _totalFatEntries; i++)
    {
        _writeFatEntry(i, 0);
    }
    _writeFatEntry(0, 0xF00 | vfd->readUInt8(0, _MediaTypeOffset));
    _writeFatEntry(1, 0xFFF);

    //  Clear the root directory
    _DirectoryEntryImpl emptyDirectoryEntryImpl;
    for (unsigned int sector = 0; sector < _rootDirectorySectors; sector++)
    {
        unsigned int lbaSector = _reservedSectors + _numberOfFatCopies * _sectorsPerFat;
        for (unsigned int index = 0; index < _directoryEntriesPerSector; index++)
        {
            _writeDirectoryEntry(lbaSector, index, emptyDirectoryEntryImpl);
        }
    }
}

void Fat12FileSystem::loadVfd()
{
    Vfd * vfd = getVfd();
    assert(vfd != NULL);

    //  Set up this file system to work with the bound disk
    _setupParameters();
}

bool Fat12FileSystem::exists(const char * fullPath)
{
    assert(fullPath != NULL);
    assert(_isOperational());

    if (strcmp(fullPath, "/") == 0)
    {   //  Root directory is a special case
        return true;
    }
    else
    {   //  General case - find directory entry for the path & examine it
        unsigned int lbaSector, entryIndex;
        _DirectoryEntryImpl directoryEntryImpl;
        return _findDirectoryEntryForPath(fullPath, lbaSector, entryIndex, directoryEntryImpl);
    }
}

bool Fat12FileSystem::isFile(const char * fullPath)
{
    assert(fullPath != NULL);
    assert(_isOperational());

    if (strcmp(fullPath, "/") == 0)
    {   //  Root directory is a special case
        return false;
    }
    else
    {   //  General case - find directory entry for the path & examine it
        unsigned int lbaSector, entryIndex;
        _DirectoryEntryImpl directoryEntryImpl;
        return _findDirectoryEntryForPath(fullPath, lbaSector, entryIndex, directoryEntryImpl) &&
               (directoryEntryImpl._flags & FileAttribute_VolumeLabel) == 0 &&
               (directoryEntryImpl._flags & FileAttribute_Subdirectory) == 0;
    }
}

bool Fat12FileSystem::isDirectory(const char * fullPath)
{
    assert(fullPath != NULL);
    assert(_isOperational());

    if (strcmp(fullPath, "/") == 0)
    {   //  Root directory is a special case
        return false;
    }
    else
    {   //  General case - find directory entry for the path & examine it
        unsigned int lbaSector, entryIndex;
        _DirectoryEntryImpl directoryEntryImpl;
        return _findDirectoryEntryForPath(fullPath, lbaSector, entryIndex, directoryEntryImpl) &&
               (directoryEntryImpl._flags & FileAttribute_VolumeLabel) == 0 &&
               (directoryEntryImpl._flags & FileAttribute_Subdirectory) != 0;
    }
}

bool Fat12FileSystem::listDirectory(const char * fullPath, DirectoryEntries & directoryEntries)
{
    assert(fullPath != NULL);
    assert(_isOperational());

    directoryEntries.clear();

    if (strcmp(fullPath, "/") == 0)
    {   //  Listing the root directory is a special case
        for (unsigned int lbaSector = _firstRootDirectorySector;
             lbaSector < _firstRootDirectorySector + _rootDirectorySectors;
             lbaSector++)
        {
            for (unsigned int entryIndex = 0; entryIndex < _directoryEntriesPerSector; entryIndex++)
            {
                _DirectoryEntryImpl directoryEntryImpl;
                _readDirectoryEntry(lbaSector, entryIndex, directoryEntryImpl);
                if (directoryEntryImpl._name[0] == _DirEntryNamePrefix_NoMore)
                {   //  This entry is unused, and so are all following entries
                    return true;
                }
                else if (directoryEntryImpl._name[0] != _DirEntryNamePrefix_Deleted &&
                         (directoryEntryImpl._flags & FileAttribute_VolumeLabel) == 0)
                {   //  This entry is live, and is not a volume label
                    directoryEntries.push_back(_translateDirectoryEntry(directoryEntryImpl));
                }
            }
        }
        return true;
    }
    else
    {   //  Listing a non-root directory. Find the directory entry for the path
        unsigned int lbaSector, entryIndex;
        _DirectoryEntryImpl directoryEntryImpl;
        if (!_findDirectoryEntryForPath(fullPath, lbaSector, entryIndex, directoryEntryImpl) ||
            (directoryEntryImpl._flags & FileAttribute_Subdirectory) == 0)
        {   //  OOPS! Directory entry for "path" not found or is not a directory
            return false;
        }

        //  The "directoryEntryImpl" is the descriptor of the directory we want to list
        for (unsigned int clusterIndex = directoryEntryImpl._firstCluster;
             _isDataClusterFatEntry(clusterIndex);
             clusterIndex = _readFatEntry(clusterIndex))
        {
            for (unsigned int sectorInCluster = 0; sectorInCluster < _sectorsPerCluster; sectorInCluster++)
            {
                lbaSector = _firstDataAreaSector + (clusterIndex - 2) * _sectorsPerCluster + sectorInCluster;
                for (entryIndex = 0; entryIndex < _directoryEntriesPerSector; entryIndex++)
                {
                    _readDirectoryEntry(lbaSector, entryIndex, directoryEntryImpl);
                    if (directoryEntryImpl._name[0] == _DirEntryNamePrefix_NoMore)
                    {   //  This entry is unused, and so are all following entries in the cluster
                        return true;
                    }
                    else if (directoryEntryImpl._name[0] != _DirEntryNamePrefix_Deleted &&
                             (directoryEntryImpl._flags & FileAttribute_VolumeLabel) == 0)
                    {   //  This entry is live, and is not a volume label
                        directoryEntries.push_back(_translateDirectoryEntry(directoryEntryImpl));
                    }
                    //  ...otherwise the entry is deleted or utility - skip it
                }
            }
        }

        //  Done
        return true;
    }
}

bool Fat12FileSystem::createDirectory(const char * fullPath)
{
    assert(fullPath != NULL);
    assert(_isOperational());

    //  Split path into components
    vector<string> pathComponents;
    if (!_splitAbsolutePath(fullPath, pathComponents))
    {   //  OOPS! Couldn't even process the path!
        return false;
    }
    if (pathComponents.size() == 0)
    {   //  OOPS! Can't create the root directory!
        return false;
    }

    //  Create the directory
    if (pathComponents.size() == 1)
    {   //  Creating a directory in a root directory is a special case
        //  The new directory will require a content cluster for "." and ".." entries
        unsigned int contentClusterIndex = _createContentClusterForNewDirectory(0); //  root directory has no content cluster
        if (contentClusterIndex == 0)
        {   //  OOPS! Disk full!
            return false;
        }

        //  Prepare the directory entry for the new directory
        _DirectoryEntryImpl newDirectoryEntryImpl(pathComponents[pathComponents.size() - 1].c_str(),
                                                  FileAttribute_Subdirectory,
                                                  time(NULL),
                                                  contentClusterIndex);
        if (newDirectoryEntryImpl._name[0] == _DirEntryNamePrefix_NoMore)
        {   //  OOPS! Invalid name! Need to release the content cluster - we won't be using it
            _writeFatEntry(contentClusterIndex, _FatEntry_FreeCluster);
            return false;
        }

        //  Insert entrty into the root directory
        if (!_insertDirectoryEntryIntoRootDirectory(newDirectoryEntryImpl))
        {   //  OOPS! Root directory full! Need to release the content cluster - we won't be using it
            _writeFatEntry(contentClusterIndex, _FatEntry_FreeCluster);
            return false;
        }
        //  Success!
        return true;
    }
    else
    {   //  All path components except the last one must be directories
        unsigned int lbaSector, entryIndex;
        _DirectoryEntryImpl parentDirectoryEntryImpl;

        //  We start with the root...
        if (!_findRootDirectoryEntryByName(pathComponents[0].c_str(), lbaSector, entryIndex, parentDirectoryEntryImpl))
        {   //  OOPS! No such directory!
            return false;
        }
        if ((parentDirectoryEntryImpl._flags & FileAttribute_Subdirectory) == 0)
        {   //  OOPS! Parent is not a directory!
            return false;
        }

        //  ...and go down
        for (size_t i = 1; i < pathComponents.size() - 1; i++)
        {   //  The "parentDirectoryEntryImpl" contains the directory entry for "i-1"-th element of
            //  the path - now we need to search that directory to find the "i"-th element
            _DirectoryEntryImpl childDirectoryEntryImpl;
            if (!_findDirectoryEntryByName(parentDirectoryEntryImpl, pathComponents[i].c_str(), lbaSector, entryIndex, childDirectoryEntryImpl))
            {   //  OOPS! The "i"-th path element not found.
                return false;
            }
            if ((childDirectoryEntryImpl._flags & FileAttribute_Subdirectory) == 0)
            {   //  OOPS! Child is not a directory!
                return false;
            }
            parentDirectoryEntryImpl = childDirectoryEntryImpl;
        }

        //  If we are here, the "parentDirectoryEntryImpl" contains the directory entry
        //  which corresponds the immediate parent directory within which we want to
        //  create a new subdirectory, and the "lbaSector" / "entryIndex" specify the
        //  LBA sector number where this "parentDirectoryEntry" resides and the entry's
        //  index in that LBA sector
        //  The new directory will require a content cluster for "." and ".." entries
        unsigned int contentClusterIndex = _createContentClusterForNewDirectory(parentDirectoryEntryImpl._firstCluster);
        if (contentClusterIndex == 0)
        {   //  OOPS! Disk full!
            return false;
        }

        //  Prepare the directory entry for the new directory
        _DirectoryEntryImpl newDirectoryEntryImpl(pathComponents[pathComponents.size() - 1].c_str(),
                                                  FileAttribute_Subdirectory,
                                                  time(NULL),
                                                  contentClusterIndex);
        if (newDirectoryEntryImpl._name[0] == _DirEntryNamePrefix_NoMore)
        {   //  OOPS! Invalid name! Need to release the content cluster - we won't be using it
            _writeFatEntry(contentClusterIndex, _FatEntry_FreeCluster);
            return false;
        }

        //  Now we need to insert the "directoryEntryImpl" entry into the directory
        //  described by "parentDirectoryEntryImpl"
        if (!_insertDirectoryEntryIntoDirectory(parentDirectoryEntryImpl, newDirectoryEntryImpl))
        {   //  OOPS! Disk full! Need to release the content cluster - we won't be using it
            _writeFatEntry(contentClusterIndex, _FatEntry_FreeCluster);
            return false;
        }
        //  Inserted - the "parentDirectoryEntryImpl"'s time stamp has been modified, and must be saved
        _writeDirectoryEntry(lbaSector, entryIndex, parentDirectoryEntryImpl);
        //  Done
        return true;
    }
}

bool Fat12FileSystem::removeDirectory(const char * fullPath)
{
    assert(fullPath != NULL);
    assert(_isOperational());

    //  TODO implement
    assert(false);
    return false;
}

FileChannel * Fat12FileSystem::createFile(const char * fullPath)
{
    assert(fullPath != NULL);
    assert(_isOperational());

    //  Split path into components
    vector<string> pathComponents;
    if (!_splitAbsolutePath(fullPath, pathComponents))
    {   //  OOPS! Couldn't even process the path!
        return NULL;
    }
    if (pathComponents.size() == 0)
    {   //  OOPS! Can't create the root directory!
        return NULL;
    }

    //  Initially, the new file will be of size 0 and will have
    //  only 1 zeroed-out cluster allocated to it
    unsigned int contentClusterIndex = _grabNewContentClusterForFile();
    if (contentClusterIndex == 0)
    {   //  OOPS! Disk full!
        return NULL;
    }

    //  Prepare the directory entry for the new file
    _DirectoryEntryImpl newDirectoryEntryImpl(pathComponents[pathComponents.size() - 1].c_str(),
                                              0,
                                              time(NULL),
                                              contentClusterIndex);
    if (newDirectoryEntryImpl._name[0] == _DirEntryNamePrefix_NoMore)
    {   //  OOPS! Invalid name! Need to release the content cluster - we won't be using it
        _writeFatEntry(contentClusterIndex, _FatEntry_FreeCluster);
        return NULL;
    }

    //  Create the file
    if (pathComponents.size() == 1)
    {   //  Creating a file in a root directory is a special case
        if (!_insertDirectoryEntryIntoRootDirectory(newDirectoryEntryImpl))
        {   //  OOPS! Root directory full! Need to release the content cluster - we won't be using it
            _writeFatEntry(contentClusterIndex, _FatEntry_FreeCluster);
            return NULL;
        }
    }
    else
    {   //  Inserting into non-root directory - find out into which one...
        unsigned int lbaSector, entryIndex;
        _DirectoryEntryImpl parentDirectoryEntryImpl;
        if (!_findDirectoryEntryForParentOfPath(fullPath, lbaSector, entryIndex, parentDirectoryEntryImpl) ||
            (parentDirectoryEntryImpl._flags & FileAttribute_Subdirectory) == 0)
        {   //  The directory where we want to create a file does not exist (or is not a directory).
            //  Need to release the content cluster - we won't be using it
            _writeFatEntry(contentClusterIndex, _FatEntry_FreeCluster);
            return NULL;
        }

        //  ...and insert the file there
        if (!_insertDirectoryEntryIntoDirectory(parentDirectoryEntryImpl, newDirectoryEntryImpl))
        {   //  OOPS! Disk full! Need to release the content cluster - we won't be using it
            _writeFatEntry(contentClusterIndex, _FatEntry_FreeCluster);
            return NULL;
        }
        //  Inserted - the "parentDirectoryEntryImpl"'s time stamp has been modified, and must be saved
        _writeDirectoryEntry(lbaSector, entryIndex, parentDirectoryEntryImpl);
    }
    //  New file created successfully - now open it
    return openFile(fullPath);
}

FileChannel * Fat12FileSystem::openFile(const char * fullPath)
{
    assert(fullPath != NULL);
    assert(_isOperational());

    if (strcmp(fullPath, "/") == 0)
    {   //  Root directory is a special case
        return NULL;
    }
    else
    {   //  General case - find directory entry for the path & examine it
        unsigned int lbaSector, entryIndex;
        _DirectoryEntryImpl directoryEntryImpl;
        if (!_findDirectoryEntryForPath(fullPath, lbaSector, entryIndex, directoryEntryImpl) ||
            (directoryEntryImpl._flags & FileAttribute_VolumeLabel) != 0 ||
            (directoryEntryImpl._flags & FileAttribute_Subdirectory) != 0)
        {   //  OOPS! Does not exist or is not a file
            return NULL;
        }

        //  Create file channel
        Fat12FileChannel * fileChannel = new Fat12FileChannel(this, lbaSector, entryIndex);
        _activeFileChannels.push_back(fileChannel);
        return fileChannel;
    }
}

bool Fat12FileSystem::deleteFile(const char * fullPath)
{
    assert(fullPath != NULL);
    assert(_isOperational());

    //  TODO implement
    assert(false);
    return false;
}

//////////
//  Operations
bool Fat12FileSystem::getFileClusters(const char * fullPath, vector<uint16_t> & clusterNumbers)
{
    assert(fullPath != NULL);
    assert(_isOperational());

    clusterNumbers.clear();

    if (strcmp(fullPath, "/") == 0)
    {   //  Root directory is a special case
        return NULL;
    }
    else
    {   //  General case - find directory entry for the path & examine it
        unsigned int lbaSector, entryIndex;
        _DirectoryEntryImpl directoryEntryImpl;
        if (!_findDirectoryEntryForPath(fullPath, lbaSector, entryIndex, directoryEntryImpl) ||
            (directoryEntryImpl._flags & FileAttribute_VolumeLabel) != 0 ||
            (directoryEntryImpl._flags & FileAttribute_Subdirectory) != 0)
        {   //  OOPS! Does not exist or is not a file
            return NULL;
        }

        //  Walk the cluster chain
        unsigned int clusterIndex = directoryEntryImpl._firstCluster;
        for (; ; )
        {
            clusterNumbers.push_back(static_cast<uint16_t>(clusterIndex));
            clusterIndex = _readFatEntry(clusterIndex);
            if (!_isDataClusterFatEntry(clusterIndex))
            {
                break;
            }
        }
        return true;
    }
}

uint16_t Fat12FileSystem::getLbaSectorForClusterNumber(uint16_t clusterNumber)
{
    assert(_isOperational());
    assert(_isDataClusterFatEntry(clusterNumber));

    return static_cast<uint16_t>(_firstDataAreaSector + (clusterNumber - 2));
}

bool Fat12FileSystem::setFileAttributes(const char * fullPath, uint8_t attributes)
{
    assert(fullPath != NULL);
    assert(_isOperational());

    if (strcmp(fullPath, "/") == 0)
    {   //  Root directory is a special case
        return false;
    }
    else
    {   //  General case - find directory entry for the path & examine it
        unsigned int lbaSector, entryIndex;
        _DirectoryEntryImpl directoryEntryImpl;
        if (!_findDirectoryEntryForPath(fullPath, lbaSector, entryIndex, directoryEntryImpl))
        {   //  OOPS! Does not exist
            return false;
        }
        //  The "VolumeLabel" and "Subdirectory" attributes of a directory entry cannot be changed
        if ((directoryEntryImpl._flags & FileAttribute_VolumeLabel) != (attributes & FileAttribute_VolumeLabel) ||
            (directoryEntryImpl._flags & FileAttribute_Subdirectory) != (attributes & FileAttribute_Subdirectory))
        {   //  OOPS!
            return false;
        }
        //  Update the directory entry - but NOT the timestamps!!!
        directoryEntryImpl._flags = attributes & FileAttribute_All;
        _writeDirectoryEntry(lbaSector, entryIndex, directoryEntryImpl);
        //  Done
        return true;
    }
}

//////////
//  Implementation helpers
Fat12FileSystem::_DirectoryEntryImpl::_DirectoryEntryImpl()
    :   _flags(0),
        _lastModifiedTime(0),
        _lastModifiedDate(0),
        _firstCluster(0),
        _size(0)
{
    memset(_name, _DirEntryNamePrefix_NoMore, 8);
    memset(_extension, _DirEntryNamePrefix_NoMore, 3);
}

Fat12FileSystem::_DirectoryEntryImpl::_DirectoryEntryImpl(const char * fullName, uint8_t flags, time_t lastModifiedTime, unsigned int firstCluster)
    :   _flags(flags),
        _lastModifiedTime(0),
        _lastModifiedDate(0),
        _firstCluster(static_cast<uint16_t>(firstCluster)),
        _size(0)
{
    assert(_firstCluster == firstCluster);
    _setFullName(fullName);
    Fat12FileSystem::_encodeTime(lastModifiedTime, _lastModifiedDate, _lastModifiedTime);
}

void Fat12FileSystem::_DirectoryEntryImpl::_storeFullName(char fullName[13]) const
{
    int scan = 0;

    for (int i = 0; i < 8 && !isspace(_name[i]); i++)
    {
        if (i == 0 && _name[i] == _DirEntryNamePrefix_E5)
        {
            fullName[scan++] = static_cast<char>(0xE5);
        }
        else
        {
            fullName[scan++] = _name[i];
        }
    }
    if (!isspace(_extension[0]))
    {
        fullName[scan++] = '.';
        for (int i = 0; i < 3 && !isspace(_extension[i]); i++)
        {
            fullName[scan++] = _extension[i];
        }
    }
    fullName[scan] = 0;
}

bool Fat12FileSystem::_DirectoryEntryImpl::_match(const char * fileName) const
{
    char fullName[13];

    //  Build the true filename represented by this entry
    _storeFullName(fullName);
    //  Compare
#if defined _MSC_VER
    return _stricmp(fullName, fileName) == 0;
#else
    return strcasecmp(fullName, fileName) == 0;
#endif
}

bool Fat12FileSystem::_DirectoryEntryImpl::_setFullName(const char * fullName)
{
    char name[8] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
    char extension[3] = { ' ', ' ', ' ' };
    int i, scan;

    if (strcmp(fullName, ".") == 0)
    {
        name[0] = '.';
    }
    else if (strcmp(fullName, "..") == 0)
    {
        name[0] = name[1] = '.';
    }
    else
    {   //  Name
        scan = 0;
        for (i = 0; fullName[i] != 0 && fullName[i] != '.'; i++)
        {
            if (scan == 8)
            {   //  OOPS! Name too long!
                memset(_name, _DirEntryNamePrefix_NoMore, 8);
                memset(_extension, _DirEntryNamePrefix_NoMore, 3);
                return false;
            }
            name[scan++] = static_cast<char>(toupper(static_cast<unsigned char>(fullName[i])));
        }
        //  Extension ?
        if (fullName[i] == '.')
        {
            scan = 0;
            for (i++; fullName[i] != 0 && fullName[i] != '.'; i++)
            {
                if (scan == 3)
                {   //  OOPS! Extension too long!
                    memset(_name, _DirEntryNamePrefix_NoMore, 8);
                    memset(_extension, _DirEntryNamePrefix_NoMore, 3);
                    return false;
                }
                extension[scan++] = static_cast<char>(toupper(static_cast<unsigned char>(fullName[i])));
            }
        }
        //  This must be the end of it
        if (fullName[i] != 0)
        {
            memset(_name, _DirEntryNamePrefix_NoMore, 8);
            memset(_extension, _DirEntryNamePrefix_NoMore, 3);
            return false;
        }
    }
    //  Store
    memcpy(_name, name, 8);
    memcpy(_extension, extension, 3);
    return true;
}

void Fat12FileSystem::_setupParameters()
{
    Vfd * vfd = getVfd();
    assert(vfd != NULL);

    //  Calculate the sector size
    _sectorSize = vfd->readUInt16(0, _BytesPerSectorOffset, ByteOrder_LittleEndian);
    if (_sectorSize != vfd->getGeometry().getSectorSize())
    {   //  OOPS! Invalid value!
        throw runtime_error("Boot sector of '" + vfd->getFileName() + "' contains an invalid sector size");
    }

    //  Calculate the "total sectors" count
    _totalSectors = vfd->readUInt16(0, _TotalSectorsOffset, ByteOrder_LittleEndian);
    if (_totalSectors != vfd->getNumberOfSectors())
    {   //  OOPS! Invalid value!
        throw runtime_error("Boot sector of '" + vfd->getFileName() + "' contains an invalid sector count");
    }

    //  Calculate the "reserved sectors" count
    _reservedSectors = vfd->readUInt16(0, _ReservedSectorsOffset, ByteOrder_LittleEndian);
    if (_reservedSectors < 1 || _reservedSectors > 32)
    {   //  OOPS! Invalid value!
        throw runtime_error("Boot sector of '" + vfd->getFileName() + "' contains an invalid reserved sectors count");
    }

    //  Measure up the root directory
    _rootDirectoryEntryCount = vfd->readUInt16(0, _RootDirectoryEntriesOffset, ByteOrder_LittleEndian);
    if (_rootDirectoryEntryCount == 0)
    {   //  OOPS!
        throw runtime_error("Boot sector of '" + vfd->getFileName() + "' contains an invalid root directory size");
    }
    _directoryEntriesPerSector = _sectorSize / 32;
    _rootDirectorySectors = (_rootDirectoryEntryCount + _directoryEntriesPerSector - 1) / _directoryEntriesPerSector;

    //  Calculate FAT geometry
    _numberOfFatCopies = vfd->readUInt8(0, _FatCopiesOffset);
    if (_numberOfFatCopies == 0)
    {   //  OOPS!
        throw runtime_error("Boot sector of '" + vfd->getFileName() + "' contains an invalid FAT copies count");
    }
    _fatEntriesPerSector = (_sectorSize * 8) / 12;
    _sectorsPerFat = vfd->readUInt16(0, _SectorsPerFatOffset, ByteOrder_LittleEndian);

    _firstDataAreaSector = _reservedSectors +
                           _numberOfFatCopies * _sectorsPerFat +
                           _rootDirectorySectors;
    _sectorsPerCluster = vfd->readUInt8(0, _SectorsPerClusterOffset);
    if (_sectorsPerCluster != 1 && _sectorsPerCluster != 2)
    {
        throw runtime_error("Boot sector of '" + vfd->getFileName() + "' contains an invalid cluster size");
    }
    _totalDataSectors = _totalSectors - _firstDataAreaSector;
    _totalDataClusters = _totalDataSectors / _sectorsPerCluster;
    _totalFatEntries = _totalDataClusters;
    assert((_totalFatEntries + _fatEntriesPerSector - 1) / _fatEntriesPerSector == _sectorsPerFat);

    _firstRootDirectorySector = _reservedSectors + _numberOfFatCopies * _sectorsPerFat;
}

bool Fat12FileSystem::_isOperational() const
{
    return _sectorSize != 0;
}

unsigned int Fat12FileSystem::_readFatEntry(unsigned int index)
{
    assert(index < _totalFatEntries);

    Vfd * vfd = getVfd();
    assert(vfd != NULL);

    unsigned int fatSectorIndex = index / _fatEntriesPerSector;
    unsigned int byteOffset = ((index % _fatEntriesPerSector) * 3) / 2;
    unsigned int bitOffset = ((index % _fatEntriesPerSector) % 2) * 4;

    uint16_t valueContainer = vfd->readUInt16(_reservedSectors + fatSectorIndex, byteOffset, ByteOrder_LittleEndian);
    return (valueContainer >> bitOffset) & 0x0FFF;
}

void Fat12FileSystem::_writeFatEntry(unsigned int index, unsigned int value)
{
    assert(index < _totalFatEntries);
    assert((value & 0x0FFF) == value);

    Vfd * vfd = getVfd();
    assert(vfd != NULL);

    unsigned int fatSectorIndex = index / _fatEntriesPerSector;
    unsigned int byteOffset = ((index % _fatEntriesPerSector) * 3) / 2;
    unsigned int bitOffset = ((index % _fatEntriesPerSector) % 2) * 4;
    uint16_t valueOutMask = static_cast<uint16_t>(~(0x0FFF << bitOffset));

    for (unsigned int fatCopy = 0; fatCopy < _numberOfFatCopies; fatCopy++)
    {
        uint16_t valueContainer = vfd->readUInt16(_reservedSectors + fatCopy * _sectorsPerFat + fatSectorIndex, byteOffset, ByteOrder_LittleEndian);
        valueContainer &= valueOutMask;                              //  remove old value
        valueContainer |= static_cast<uint16_t>(value << bitOffset); //  insert new value
        vfd->writeUInt16(_reservedSectors + fatCopy * _sectorsPerFat + fatSectorIndex, byteOffset, valueContainer, ByteOrder_LittleEndian);
    }
}

bool Fat12FileSystem::_isFreeClusterFatEntry(unsigned int entry)
{
    assert((entry & 0x0FFF) == entry);

    return entry == 0x0000;
}

bool Fat12FileSystem::_isReservedClusterFatEntry(unsigned int entry)
{
    assert((entry & 0x0FFF) == entry);

    return entry == 0x0001 ||
           (entry >= 0x0FF1 && entry <= 0x0FF5) ||
           entry == 0x0FF6;
}

bool Fat12FileSystem::_isBadClusterFatEntry(unsigned int entry)
{
    assert((entry & 0x0FFF) == entry);

    return entry == 0x0FF7;
}

bool Fat12FileSystem::_isLastClusterFatEntry(unsigned int entry)
{
    assert((entry & 0x0FFF) == entry);

    return entry == 0x0FF0 ||
           (entry >= 0x0FF8 && entry <= 0x0FFF);
}

bool Fat12FileSystem::_isDataClusterFatEntry(unsigned int entry)
{
    assert((entry & 0x0FFF) == entry);

    return entry >= 0x0002 && entry <= 0x0FEF;
}

unsigned int Fat12FileSystem::_findUnusedCluster()
{
    assert(getVfd() != NULL);

    for (unsigned int i = 0; i < _totalFatEntries; i++)
    {
        if (_isFreeClusterFatEntry(_readFatEntry(i)))
        {
            return i;
        }
    }
    return 0;
}

void Fat12FileSystem::_clearCluster(unsigned int clusterIndex)
{
    assert(clusterIndex >= 2 && clusterIndex - 2 < _totalFatEntries);

    Vfd * vfd = getVfd();
    for (unsigned int sector = 0; sector < _sectorsPerCluster; sector++)
    {
        unsigned int lbaSector = (clusterIndex - 2) * _sectorsPerCluster + _firstDataAreaSector + sector;
        for (unsigned int offset = 0; offset < _sectorSize; offset++)
        {
            vfd->writeUInt8(lbaSector, offset, 0);
        }
    }
}

void Fat12FileSystem::_encodeTime(time_t t, uint16_t & date, uint16_t & time)
{
    struct tm tm;

    tm = *gmtime(&t);
    date = static_cast<uint16_t>(((tm.tm_year - 80) << 9) | ((tm.tm_mon + 1) << 5) | (tm.tm_mday));
    time = static_cast<uint16_t>((tm.tm_hour << 11) | (tm.tm_min << 5) | (tm.tm_sec / 2));
}

time_t Fat12FileSystem::_decodeTime(uint16_t date, uint16_t time)
{
    struct tm tm;

    tm.tm_year = ((date >> 9) & 0x007F) + 80;
    tm.tm_mon = ((date >> 5) & 0x000F) - 1;
    tm.tm_mday = (date & 0x001F);
    tm.tm_hour = ((time >> 11) & 0x001F);
    tm.tm_min = ((time >> 5) & 0x003F);
    tm.tm_sec = (time & 0x001F) * 2;
    tm.tm_isdst = 0;
    return mktime(&tm);
}

void Fat12FileSystem::_readDirectoryEntry(unsigned int lbaSector, unsigned int entryIndex, _DirectoryEntryImpl & directoryEntryImpl)
{
    Vfd * vfd = getVfd();
    assert(vfd != NULL);
    assert(lbaSector < _totalSectors);
    assert(entryIndex < _directoryEntriesPerSector);

    unsigned int offset = entryIndex * 32;
    vfd->readData(lbaSector, offset, directoryEntryImpl._name, 8);
    vfd->readData(lbaSector, offset + 8, directoryEntryImpl._extension, 3);
    directoryEntryImpl._flags = vfd->readUInt8(lbaSector, offset + 11);
    directoryEntryImpl._lastModifiedTime = vfd->readUInt16(lbaSector, offset + 22, ByteOrder_LittleEndian);
    directoryEntryImpl._lastModifiedDate = vfd->readUInt16(lbaSector, offset + 24, ByteOrder_LittleEndian);
    directoryEntryImpl._firstCluster = vfd->readUInt16(lbaSector, offset + 26, ByteOrder_LittleEndian);
    directoryEntryImpl._size = vfd->readUInt32(lbaSector, offset + 28, ByteOrder_LittleEndian);
}

void Fat12FileSystem::_writeDirectoryEntry(unsigned int lbaSector, unsigned int entryIndex, const _DirectoryEntryImpl & directoryEntryImpl)
{
    Vfd * vfd = getVfd();
    assert(vfd != NULL);
    assert(lbaSector < _totalSectors);
    assert(entryIndex < _directoryEntriesPerSector);

    unsigned int offset = entryIndex * 32;
    vfd->writeData(lbaSector, offset, directoryEntryImpl._name, 8);
    vfd->writeData(lbaSector, offset + 8, directoryEntryImpl._extension, 3);
    vfd->writeUInt8(lbaSector, offset + 11, directoryEntryImpl._flags);
    for (unsigned int i = 12; i < 22; i++)
    {   //  Clear up unused bytes
        vfd->writeUInt8(lbaSector, offset + i, 0);
    }
    vfd->writeUInt16(lbaSector, offset + 22, directoryEntryImpl._lastModifiedTime, ByteOrder_LittleEndian);
    vfd->writeUInt16(lbaSector, offset + 24, directoryEntryImpl._lastModifiedDate, ByteOrder_LittleEndian);
    vfd->writeUInt16(lbaSector, offset + 26, directoryEntryImpl._firstCluster, ByteOrder_LittleEndian);
    vfd->writeUInt32(lbaSector, offset + 28, directoryEntryImpl._size, ByteOrder_LittleEndian);
}

bool Fat12FileSystem::_splitAbsolutePath(const char * path, vector<string> & pathComponents)
{
    pathComponents.clear();
    if (path[0] != '/')
    {   //  Not an absolute path!
        return false;
    }
    //  "/" is a special case
    if (path[0] == '/' && path[1] == 0)
    {
        return true;
    }
    //  General case
    size_t scan = 1;
    for (; ; )
    {   //  Everything until the closest '/' or EOS is a path component
        size_t prescan;
        for (prescan = scan; path[prescan] != 0 && path[prescan] != '/'; prescan++)
        {   //  All work is done in the loop header
        }
        pathComponents.push_back(string(path + scan, prescan - scan));
        //  Advance
        scan = prescan;
        if (path[scan] != 0)
        {
            assert(path[scan] == '/');
            scan++;
        }
        else
        {
            break;
        }
    }
    //  Done
    return true;
}

const char * Fat12FileSystem::_findLastPathComponent(const char * path)
{
    const char * result = NULL;
    for (; *path != 0; path++)
    {
        if (path[0] == '/' && path[1] != 0)
        {
            result = path + 1;
        }
    }
    return result;
}

bool Fat12FileSystem::_findRootDirectoryEntryByName(const char * name, unsigned int & lbaSector, unsigned int & entryIndex, _DirectoryEntryImpl & directoryEntryImpl)
{
    for (lbaSector = _firstRootDirectorySector;
         lbaSector < _firstRootDirectorySector + _rootDirectorySectors;
         lbaSector++)
    {
        for (entryIndex = 0; entryIndex < _directoryEntriesPerSector; entryIndex++)
        {
            _readDirectoryEntry(lbaSector, entryIndex, directoryEntryImpl);
            if (directoryEntryImpl._name[0] == _DirEntryNamePrefix_NoMore)
            {   //  This entry is unused, and all following entries are unused too
                return false;   //  Not found
            }
            else if (directoryEntryImpl._name[0] != _DirEntryNamePrefix_Deleted &&
                     (directoryEntryImpl._flags & FileAttribute_VolumeLabel) == 0)
            {   //  This entry represents a live entity which is not a volume label
                if (directoryEntryImpl._match(name))
                {   //  Got it !
                    return true;
                }
            }
        }
    }
    return false;   //  not found!
}

bool Fat12FileSystem::_findDirectoryEntryByName(const _DirectoryEntryImpl & parentDirectoryEntryImpl, const char * name, unsigned int & lbaSector, unsigned int & entryIndex, _DirectoryEntryImpl & directoryEntryImpl)
{
    assert(&parentDirectoryEntryImpl != &directoryEntryImpl);

    unsigned int clusterIndex = parentDirectoryEntryImpl._firstCluster;
    //  Search this cluster
SearchCluster:
    for (unsigned int sectorInCluster = 0; sectorInCluster < _sectorsPerCluster; sectorInCluster++)
    {
        lbaSector = _firstDataAreaSector + (clusterIndex - 2) * _sectorsPerCluster + sectorInCluster;
        for (entryIndex = 0; entryIndex < _directoryEntriesPerSector; entryIndex++)
        {
            _readDirectoryEntry(lbaSector, entryIndex, directoryEntryImpl);
            if (directoryEntryImpl._name[0] == _DirEntryNamePrefix_NoMore)
            {   //  This entry is dead - and so are all the remaining ones.
                return false;
            }
            else if (directoryEntryImpl._name[0] != _DirEntryNamePrefix_Deleted &&
                     (directoryEntryImpl._flags & FileAttribute_VolumeLabel) == 0)
            {   //  This entry is live - does if match the name we look for ?
                if (directoryEntryImpl._match(name))
                {   //  Yes!!!
                    return true;
                }
            }
            //  ...otherwise it's a deleted entry - skip it & keep looking.
        }
    }
    //  OOPS! The element we seek was not found in the cluster "clusterIndex".
    //  The FAT tells us what the next cluster is, though
    clusterIndex = _readFatEntry(clusterIndex);
    if (_isLastClusterFatEntry(clusterIndex))
    {   //  OOPS! No more clusters in the directory - the search failed
        return false;
    }
    else if (_isDataClusterFatEntry(clusterIndex))
    {   //  Next cluster of the directory has been found - keep searching there
        goto SearchCluster;
    }
    else
    {   //  OOPS! Something wrong!
        return false;
    }
}

bool Fat12FileSystem::_findDirectoryEntryForPath(const char * fullPath, unsigned int & lbaSector, unsigned int & entryIndex, _DirectoryEntryImpl & directoryEntryImpl)
{
    //  Split path into components
    vector<string> pathComponents;
    if (!_splitAbsolutePath(fullPath, pathComponents))
    {   //  OOPS! Couldn't even process the path!
        return false;
    }
    if (pathComponents.size() == 0)
    {   //  OOPS! Root directory has no directory entry - it's fixed!
        return false;
    }

    //  We start from the root directory...
    if (!_findRootDirectoryEntryByName(pathComponents[0].c_str(), lbaSector, entryIndex, directoryEntryImpl))
    {   //  OOPS! The 0th path component does not exist in the root directory
        return false;
    }

    //  ...and go down
    for (size_t i = 1; i < pathComponents.size(); i++)
    {   //  The "directoryEntryImpl" contains the directory entry for "i-1"-th element of
        //  the path - now we need to search that directory to find the "i"-th element
        if ((directoryEntryImpl._flags & FileAttribute_Subdirectory) == 0)
        {   //  OOPS! Parent is not a directory!
            return false;
        }
        //  Find the child
        _DirectoryEntryImpl childDirectoryEntryImpl;
        if (!_findDirectoryEntryByName(directoryEntryImpl, pathComponents[i].c_str(), lbaSector, entryIndex, childDirectoryEntryImpl))
        {   //  OOPS! The "i"-th path element not found.
            return false;
        }
        directoryEntryImpl = childDirectoryEntryImpl;
    }
    //  Done!
    return true;
}

bool Fat12FileSystem::_findDirectoryEntryForParentOfPath(const char * fullPath, unsigned int & lbaSector, unsigned int & entryIndex, _DirectoryEntryImpl & directoryEntryImpl)
{
    //  Split path into components
    vector<string> pathComponents;
    if (!_splitAbsolutePath(fullPath, pathComponents))
    {   //  OOPS! Couldn't even process the path!
        return false;
    }
    if (pathComponents.size() == 0)
    {   //  OOPS! Root directory has no parent - hence we can't find a directory entry for it
        return false;
    }

    //  We start from the root directory...
    if (!_findRootDirectoryEntryByName(pathComponents[0].c_str(), lbaSector, entryIndex, directoryEntryImpl))
    {   //  OOPS! The 0th path component does not exist in the root directory
        return false;
    }

    //  ...and go down
    for (size_t i = 1; i < pathComponents.size() - 1; i++)
    {   //  The "directoryEntryImpl" contains the directory entry for "i-1"-th element of
        //  the path - now we need to search that directory to find the "i"-th element
        if ((directoryEntryImpl._flags & FileAttribute_Subdirectory) == 0)
        {   //  OOPS! Parent is not a directory!
            return false;
        }
        //  Find the child
        _DirectoryEntryImpl childDirectoryEntryImpl;
        if (!_findDirectoryEntryByName(directoryEntryImpl, pathComponents[i].c_str(), lbaSector, entryIndex, childDirectoryEntryImpl))
        {   //  OOPS! The "i"-th path element not found.
            return false;
        }
        directoryEntryImpl = childDirectoryEntryImpl;
    }
    //  Done!
    return true;
}

void Fat12FileSystem::_closeAllFileChannels()
{
    while (!_activeFileChannels.empty())
    {
        delete _activeFileChannels[0];
    }
}

unsigned int Fat12FileSystem::_createContentClusterForNewDirectory(unsigned int parentDirectoryClusterIndex)
{
    //  The new directory will always contain "." and ".." entries - so we need
    //  a cluster where these will be held
    unsigned int contentClusterIndex = _findUnusedCluster();
    if (contentClusterIndex == 0)
    {   //  OOPS! Disk full!
        return 0;
    }
    _DirectoryEntryImpl thisDirectoryEntryImpl(".", FileAttribute_Subdirectory, time(NULL), contentClusterIndex);
    _DirectoryEntryImpl parentDirectoryEntryImpl("..", FileAttribute_Subdirectory, time(NULL), parentDirectoryClusterIndex);

    _clearCluster(contentClusterIndex);
    _writeDirectoryEntry(_firstDataAreaSector + (contentClusterIndex - 2) * _sectorsPerCluster, 0, thisDirectoryEntryImpl);
    _writeDirectoryEntry(_firstDataAreaSector + (contentClusterIndex - 2) * _sectorsPerCluster, 1, parentDirectoryEntryImpl);
    _writeFatEntry(contentClusterIndex, _FatEntry_LastCluster);

    return contentClusterIndex;
}

unsigned int Fat12FileSystem::_grabNewContentClusterForFile()
{
    //  The new directory will always contain "." and ".." entries - so we need
    //  a cluster where these will be held
    unsigned int contentClusterIndex = _findUnusedCluster();
    if (contentClusterIndex == 0)
    {   //  OOPS! Disk full!
        return 0;
    }

    _clearCluster(contentClusterIndex);
    _writeFatEntry(contentClusterIndex, _FatEntry_LastCluster);

    return contentClusterIndex;
}

bool Fat12FileSystem::_insertDirectoryEntryIntoRootDirectory(const _DirectoryEntryImpl & newDirectoryEntryImpl)
{
    //  Where can we insert this directory entry ?
    for (unsigned int lbaSector = _firstRootDirectorySector;
         lbaSector < _firstRootDirectorySector + _rootDirectorySectors;
         lbaSector++)
    {
        for (unsigned int entryIndex = 0; entryIndex < _directoryEntriesPerSector; entryIndex++)
        {
            _DirectoryEntryImpl existingDirectoryEntryImpl;
            _readDirectoryEntry(lbaSector, entryIndex, existingDirectoryEntryImpl);
            if (existingDirectoryEntryImpl._name[0] == _DirEntryNamePrefix_NoMore ||
                existingDirectoryEntryImpl._name[0] == _DirEntryNamePrefix_Deleted)
            {   //  The existing entry is dead or deleted - we can (re)use it
                _writeDirectoryEntry(lbaSector, entryIndex, newDirectoryEntryImpl);
                //  Success!
                return true;
            }
        }
    }
    //  OOPS! Root directory full!
    return false;
}

bool Fat12FileSystem::_insertDirectoryEntryIntoDirectory(_DirectoryEntryImpl & parentDirectoryEntryImpl, const _DirectoryEntryImpl & newDirectoryEntryImpl)
{
    unsigned int lbaSector, entryIndex;
    unsigned int clusterIndex = parentDirectoryEntryImpl._firstCluster;
    //  Search this cluster
SearchCluster:
    for (unsigned int sectorInCluster = 0; sectorInCluster < _sectorsPerCluster; sectorInCluster++)
    {
        lbaSector = _firstDataAreaSector + (clusterIndex - 2) * _sectorsPerCluster + sectorInCluster;
        for (entryIndex = 0; entryIndex < _directoryEntriesPerSector; entryIndex++)
        {
            _DirectoryEntryImpl existingDirectoryEntryImpl;
            _readDirectoryEntry(lbaSector, entryIndex, existingDirectoryEntryImpl);
            if (existingDirectoryEntryImpl._name[0] == _DirEntryNamePrefix_NoMore ||
                existingDirectoryEntryImpl._name[0] == _DirEntryNamePrefix_Deleted)
            {   //  This entry is dead or deleted - so we can (re)use it
                _writeDirectoryEntry(lbaSector, entryIndex, newDirectoryEntryImpl);
                _encodeTime(time(NULL), parentDirectoryEntryImpl._lastModifiedDate, parentDirectoryEntryImpl._lastModifiedTime);
                return true;
            }
            else if (memcmp(existingDirectoryEntryImpl._name, newDirectoryEntryImpl._name, 8) == 0 &&
                     memcmp(existingDirectoryEntryImpl._extension, newDirectoryEntryImpl._extension, 3) == 0)
            {   //  OOPS! Directory entry with the same name already exists - we can't create a new one
                return false;
            }
            //  else we're looking at a live directory entry with a name different from the one
            //  we're trying to insert - skip it & keep looking
        }
    }
    //  OOPS! The element we seek was not found in the cluster "clusterIndex".
    //  The FAT tells us what the next cluster is, though
    unsigned int nextClusterIndex = _readFatEntry(clusterIndex);
    if (_isLastClusterFatEntry(nextClusterIndex))
    {   //  OOPS! No more clusters in the directory - we want to extend the directory by one
        //  more cluster
        nextClusterIndex = _findUnusedCluster();
        _writeFatEntry(clusterIndex, nextClusterIndex);
        _writeFatEntry(nextClusterIndex, _FatEntry_LastCluster);
        _clearCluster(nextClusterIndex);
        //  Directory extended by one more all-zero cluster - keep searching
        clusterIndex = nextClusterIndex;
        goto SearchCluster;
    }
    else if (_isDataClusterFatEntry(nextClusterIndex))
    {   //  Next cluster of the directory has been found - keep searching there
        clusterIndex = nextClusterIndex;
        goto SearchCluster;
    }
    else
    {   //  OOPS! Something wrong! Need to also release the content cluster - we won't be using it
        return false;
    }
}

DirectoryEntry Fat12FileSystem::_translateDirectoryEntry(const _DirectoryEntryImpl & directoryEntryImpl)
{
    DirectoryEntry directoryEntry;

    directoryEntryImpl._storeFullName(directoryEntry.name);
    directoryEntry.creationTime = 0;
    directoryEntry.lastAccessTime = 0;
    directoryEntry.lastModificationTime = _decodeTime(directoryEntryImpl._lastModifiedDate,
                                                      directoryEntryImpl._lastModifiedTime);
    directoryEntry.size = directoryEntryImpl._size;
    directoryEntry.isDirectory = (directoryEntryImpl._flags & FileAttribute_Subdirectory) != 0;
    return directoryEntry;
}

unsigned int Fat12FileSystem::_getUnusedClustersCount()
{
    unsigned int result = 0;
    for (unsigned int clusterIndex = 0; clusterIndex < _totalDataClusters; clusterIndex++)
    {
        if (_isFreeClusterFatEntry(_readFatEntry(clusterIndex)))
        {
            result++;
        }
    }
    return result;
}

bool Fat12FileSystem::_setFileSize(unsigned int lbaSector, unsigned int entryIndex, long size)
{
    //  Reload the directory entry
    Fat12FileSystem::_DirectoryEntryImpl directoryEntryImpl;
    _readDirectoryEntry(lbaSector, entryIndex, directoryEntryImpl);
    unsigned int oldClusterCount = _getClusterCountForFileSize(directoryEntryImpl._size);
    unsigned int newClusterCount = _getClusterCountForFileSize(size);

    if (newClusterCount == oldClusterCount)
    {   //  If we don't need to change cluster count, things are easy.
        //  Just update the file size
        directoryEntryImpl._size = size;
        _writeDirectoryEntry(lbaSector, entryIndex, directoryEntryImpl);
        return true;
    }
    else if (newClusterCount < oldClusterCount)
    {   //  If we're reducing the cluster count, we can always succeed - all we do is
        //  chop off some clusters at the end of the cluster chain.
        //  Shortening the cluster chain - reduce the file size first, then
        //  mark extra clusters "free". This order ensures we can recover if
        //  an operation encounters a failure (e.g. floppy forced out)
        directoryEntryImpl._size = size;
        _writeDirectoryEntry(lbaSector, entryIndex, directoryEntryImpl);

        unsigned int clusterIndex = directoryEntryImpl._firstCluster;
        unsigned int clusterNumber;
        //  We need to skip the first "newClusterCount - 1" clusters
        for (clusterNumber = 1; clusterNumber < newClusterCount; clusterNumber++)
        {
            clusterIndex = _readFatEntry(clusterIndex);
            assert(_isDataClusterFatEntry(clusterIndex));
        }
        //  The "clusterIndex" will now become the last cluster of the file - and
        //  all subsequent clusters must be freed
        unsigned int nextClusterIndex = _readFatEntry(clusterIndex);
        _writeFatEntry(clusterIndex, _FatEntry_LastCluster);    //  this marks the new "last" cluster
        for (; ; )
        {
            clusterIndex = nextClusterIndex;
            nextClusterIndex = _readFatEntry(clusterIndex);
            _writeFatEntry(clusterIndex, _FatEntry_FreeCluster);
            if (_isLastClusterFatEntry(nextClusterIndex))
            {
                break;
            }
        }
        //  Done
        return true;
    }
    else
    {   //  If we're here, we need to assign more clusters to the file. We know both how many
        //  extra clusters we need, and how many free clusters the file system has
        unsigned int extraClustersCount = newClusterCount - oldClusterCount;
        if (extraClustersCount > _getUnusedClustersCount())
        {   //  OOPS! Not enough free clusters on disk!
            return false;
        }
        unsigned int clusterIndex = directoryEntryImpl._firstCluster;
        unsigned int nextClusterIndex = _readFatEntry(clusterIndex);
        //  Seek to the end of the existing cluster chain...
        while (!_isLastClusterFatEntry(nextClusterIndex))
        {
            clusterIndex = nextClusterIndex;
            nextClusterIndex = _readFatEntry(clusterIndex);
        }
        //  ...and create as many new clusters as needed
        for (unsigned int i = 0; i < extraClustersCount; i++)
        {   //  The cluster "clusterIndex" is the last cluster currently assigned to
            //  the file - we need to allocate one more cluster
            nextClusterIndex = _grabNewContentClusterForFile();
            _writeFatEntry(nextClusterIndex, _FatEntry_LastCluster);
            _writeFatEntry(clusterIndex, nextClusterIndex);
            clusterIndex = nextClusterIndex;
            nextClusterIndex = _readFatEntry(clusterIndex);
        }
        //  Now we can update the file size - the cluster chain is long enough to accomodate it
        directoryEntryImpl._size = size;
        _writeDirectoryEntry(lbaSector, entryIndex, directoryEntryImpl);
        //  Done
        return true;
    }
}

unsigned int Fat12FileSystem::_getClusterCountForFileSize(long size)
{
    unsigned int bytesPerCluster = _sectorsPerCluster * _sectorSize;
    return std::max<unsigned int>(1, static_cast<unsigned int>((size + (bytesPerCluster - 1)) / bytesPerCluster));
}

void Fat12FileSystem::_writeBytes(const _DirectoryEntryImpl & directoryEntryImpl, uint32_t offset, const uint8_t * bytes, uint32_t numBytes)
{
    assert(offset <= directoryEntryImpl._size);
    assert(numBytes > 0 && numBytes <= directoryEntryImpl._size);
    assert(offset + numBytes <= directoryEntryImpl._size);

    unsigned int bytesPerCluster = _sectorsPerCluster * _sectorSize;

    //  If the write affects several clusters, break it into several writes, each confined to one cluster
    unsigned int firstCluster = offset / bytesPerCluster;
    unsigned int lastCluster = (offset + numBytes - 1) / bytesPerCluster;
    if (firstCluster != lastCluster)
    {
        uint32_t secondClusterOffset = (firstCluster + 1) * bytesPerCluster;
        size_t firstClusterByteCount = secondClusterOffset - offset;
        _writeBytes(directoryEntryImpl, offset, bytes, (uint32_t)firstClusterByteCount);
        _writeBytes(directoryEntryImpl, secondClusterOffset, bytes + firstClusterByteCount, (uint32_t)(numBytes - firstClusterByteCount));
        return;
    }

    //  It's guaranteed that all bytes are written to the same cluster
    //  We need to locate the cluster where this byte resides - by traversing the cluster chain
    unsigned int clusterNumber = firstCluster;
    unsigned int offsetInCluster = offset % bytesPerCluster;

    unsigned int clusterIndex = directoryEntryImpl._firstCluster;
    for (unsigned int i = 0; i < clusterNumber; i++)
    {
        assert(_isDataClusterFatEntry(clusterIndex));
        clusterIndex = _readFatEntry(clusterIndex);
    }

    //  We're within a single cluster - but we must work with sectors
    unsigned int firstSectorInCluster = offsetInCluster / _sectorSize;
    unsigned int lastSectorInCluster = (offsetInCluster + numBytes - 1) / _sectorSize;
    if (firstSectorInCluster != lastSectorInCluster)
    {   //  Need to break the write at sector boundaries
        assert(false);  //  TODO implement
    }

    //  The write is now confined to a single sector
    unsigned int lbaSector = _firstDataAreaSector + (clusterIndex - 2) * _sectorsPerCluster + offsetInCluster / _sectorSize;
    unsigned int offsetInSector = offset % _sectorSize;
    getVfd()->writeData(lbaSector, offsetInSector, bytes, numBytes);
}

//  End of fat12-file-system.cpp
