//
//  create-directory-action.cpp - "create directory" action
//
//////////
#include "main.hpp"

//////////
//  Action
bool CreateDirectoryAction::execute(Vfd *& currentVfd)
{
    assert(currentVfd != NULL);

    Fat12FileSystem fileSystem;
    fileSystem.bind(currentVfd);
    fileSystem.loadVfd();

    if (fileSystem.createDirectory(_path.c_str()))
    {
        printf("Created directory '%s'\n", _path.c_str());
        return true;
    }
    else
    {
        printf("*** ERROR: Failed to create directory '%s'\n", _path.c_str());
        return false;
    }
}

//  End of create-directory-action.cpp
