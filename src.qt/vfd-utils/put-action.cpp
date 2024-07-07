//
//  put-action.cpp - "put" action
//
//////////
#include "main.hpp"

//////////
//  Action
bool PutAction::execute(Vfd *& currentVfd)
{
    assert(currentVfd != NULL);

    Fat12FileSystem fileSystem;
    fileSystem.bind(currentVfd);
    fileSystem.loadVfd();

    printf("Copying host file %s to VFD file %s\n", _hostPath.c_str(), _vfdPath.c_str());
    FileChannel * fileChannel = fileSystem.createFile(_vfdPath.c_str());
    if (fileChannel == NULL)
    {
        printf("*** ERROR: Failed to create VFD file '%s'\n", _vfdPath.c_str());
        return false;
    }

    FILE * hostFile = fopen(_hostPath.c_str(), "rb");
    if (hostFile == NULL)
    {
        printf("*** ERROR: Failed to open host file '%s'\n", _hostPath.c_str());
        return false;
    }

    for (; ; )
    {
        char buffer[1024];

        size_t bytesRead = fread(buffer, sizeof(char), sizeof(buffer) / sizeof(char), hostFile) * sizeof(char);
        if (bytesRead == 0)
        {   //  EOF/error
            break;
        }
        size_t bytesWritten = fileChannel->write(buffer, bytesRead);
        if (bytesWritten != bytesRead)
        {   //  OOPS!
            printf("*** ERROR: Can't write to VFD file '%s' (VFD full?)\n", _vfdPath.c_str());
            return false;
        }
    }
    fclose(hostFile);
    delete fileChannel; //  closes it first

    //  Done
    return true;
}

//  End of put-action.cpp
