//
//  create-directory-action.cpp - "create directory" action
//
//////////
#include "main.hpp"

//////////
//  Action
bool CreateFileAction::execute(Vfd *& currentVfd)
{
    assert(currentVfd != NULL);

    Fat12FileSystem fileSystem;
    fileSystem.bind(currentVfd);
    fileSystem.loadVfd();

    FileChannel * fileChannel = fileSystem.createFile(_path.c_str());
    if (fileChannel != NULL)
    {   //  Cuccess
        printf("Created file '%s'\n", _path.c_str());
        delete fileChannel;
        return true;
    }
    else
    {
        printf("*** ERROR: Failed to create file '%s'\n", _path.c_str());
        return false;
    }
}

//  End of create-directory-action.cpp
