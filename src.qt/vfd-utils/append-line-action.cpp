//
//  append-line-action.cpp - "append-line" action
//
//////////
#include "main.hpp"

//////////
//  Action
bool AppendLineAction::execute(Vfd *& currentVfd)
{
    assert(currentVfd != NULL);

    Fat12FileSystem fileSystem;
    fileSystem.bind(currentVfd);
    fileSystem.loadVfd();

    FileChannel * fileChannel = fileSystem.openFile(_path.c_str());
    if (fileChannel != NULL)
    {   //  File open successfully - try appending the line
        fileChannel->setCurrentPosition(fileChannel->getSize());
        string addend = _line + "\n";
        size_t bytesWritten = fileChannel->write(addend.c_str(), addend.length());
        if (bytesWritten == addend.length())
        {
            printf("Appended line '%s' to file '%s'\n", _line.c_str(), _path.c_str());
            delete fileChannel;
            return true;
        }
        else
        {
            printf("Failed to append line '%s' to file '%s' (media full?)\n", _line.c_str(), _path.c_str());
            delete fileChannel;
            return true;
        }
    }
    else
    {
        printf("*** ERROR: Failed to append to file '%s'\n", _path.c_str());
        return false;
    }
}

//  End of append-line-action.cpp
