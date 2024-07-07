//
//  list-directory-action.cpp - "list directory" action
//
//////////
#include "main.hpp"

//////////
//  Action
bool ListDirectoryAction::execute(Vfd *& currentVfd)
{
    assert(currentVfd != NULL);

    Fat12FileSystem fileSystem;
    fileSystem.bind(currentVfd);
    fileSystem.loadVfd();

    DirectoryEntries directoryEntries;
    fileSystem.listDirectory(_path.c_str(), directoryEntries);

    printf("Contents of %s\n", _path.c_str());
    for (size_t i = 0; i < directoryEntries.size(); i++)
    {
        DirectoryEntry & de = directoryEntries[i];

        struct tm timeinfo;
        timeinfo = *(localtime(&(de.lastModificationTime)));
        char timeString[64];
        strcpy(timeString, asctime(&timeinfo));
        if (timeString[strlen(timeString) - 1] == '\n')
        {
            timeString[strlen(timeString) - 1] = 0;
        }

        string name = de.name;
        if (de.isDirectory)
        {
            name += '/';
        }
        printf("    %-14s  %s  %lu\n",
               name.c_str(),
               timeString,
               static_cast<unsigned long>(de.size));
    }

    return true;    //  TODO always ?
}

//  End of list-directory-action.cpp
