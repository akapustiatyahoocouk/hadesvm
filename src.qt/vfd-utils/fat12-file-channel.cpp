//
//  fat12-file-channel.cpp - FAT12 file channel
//
//////////
#include "main.hpp"

//////////
//  Construction/destruction
Fat12FileChannel::Fat12FileChannel(Fat12FileSystem * fileSystem,
                                   unsigned int directoryEntryLbaSector,
                                   unsigned int directoryEntryIndex)
    :   _fileSystem(fileSystem),
        _directoryEntryLbaSector(directoryEntryLbaSector),
        _directoryEntryIndex(directoryEntryIndex),
        _currentPosition(0)
{
    assert(_fileSystem != 0);
    assert(_directoryEntryLbaSector >= _fileSystem->_firstRootDirectorySector &&
           _directoryEntryLbaSector < _fileSystem->_totalSectors);
    assert(_directoryEntryIndex < _fileSystem->_directoryEntriesPerSector);
}

Fat12FileChannel::~Fat12FileChannel()
{
    //  Remove this channel from the active channels list
    for (vector<Fat12FileChannel*>::iterator it = _fileSystem->_activeFileChannels.begin();
         it != _fileSystem->_activeFileChannels.end();
         it++)
    {
        Fat12FileChannel * item = *it;
        if (item == this)
        {
            _fileSystem->_activeFileChannels.erase(it);
            break;
        }
    }
}

//////////
//  FileChannel
long Fat12FileChannel::getCurrentPosition()
{
    return _currentPosition;
}

void Fat12FileChannel::setCurrentPosition(long position)
{
    assert(position >= 0);

    _currentPosition = position;
}

long Fat12FileChannel::getSize()
{
    Fat12FileSystem::_DirectoryEntryImpl directoryEntryImpl;
    _fileSystem->_readDirectoryEntry(_directoryEntryLbaSector, _directoryEntryIndex, directoryEntryImpl);
    return static_cast<long>(directoryEntryImpl._size);
}

bool Fat12FileChannel::setSize(long size)
{
    assert(size >= 0);

    return _fileSystem->_setFileSize(_directoryEntryLbaSector, _directoryEntryIndex, size);
}

size_t Fat12FileChannel::read(void * buffer, size_t bytesToRead)
{
    assert(buffer != 0);

    //  Handle special cases
    if (bytesToRead == 0)
    {
        return 0;
    }

    //  General case
    uint8_t * bytes = static_cast<uint8_t *>(buffer);

    //  TODO
    assert(bytes != NULL);
    return 0;
}

size_t Fat12FileChannel::write(const void * buffer, size_t bytesToWrite)
{
    assert(buffer != 0);

    //  Handle special cases
    if (bytesToWrite == 0)
    {
        return 0;
    }

    //  General case
    const uint8_t * bytes = static_cast<const uint8_t *>(buffer);

    //  Make sure the file is big enough to accommodate the write...
    if (!_fileSystem->_setFileSize(_directoryEntryLbaSector, _directoryEntryIndex, (long)(_currentPosition + bytesToWrite)))
    {   //  OOPS! Not enough space! We may still be able to write part of the buffer
        if (bytesToWrite == 1)
        {   //  Failed to write a single byte - we can't split that in two sub-writes
            return 0;
        }
        else
        {   //  Split write into two sub-writes
            size_t halfOfBytesToWrite = bytesToWrite / 2;
            size_t result1 = write(bytes, halfOfBytesToWrite);
            size_t result2 = write(bytes + halfOfBytesToWrite, bytesToWrite - halfOfBytesToWrite);
            return result1 + result2;
        }
    }

    //  Now the file has enough clusters allocated - store bytes
    Fat12FileSystem::_DirectoryEntryImpl directoryEntryImpl;
    _fileSystem->_readDirectoryEntry(_directoryEntryLbaSector, _directoryEntryIndex, directoryEntryImpl);
    _fileSystem->_writeBytes(directoryEntryImpl, _currentPosition, bytes, (unsigned)bytesToWrite);
    _currentPosition += (long)bytesToWrite;

    return bytesToWrite;
}

//  End of fat12-file-channel.cpp
