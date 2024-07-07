//
//  main.cpp - top-level logic
//
//////////
#include "main.hpp"

//////////
//  The program entry point
int main(int argc, char ** argv)
{
    int exitCode = EXIT_SUCCESS;

    //  Help is a special case
    if (argc <= 1)
    {
        CommandLine::printBanner();
        CommandLine::printUsage();
        return 0;
    }

    //  Process the command line; errors are handled internally with abort
    CommandLine commandLine(argc - 1, argv + 1);

    //  Go!
    Vfd * currentVfd = NULL;
    for (size_t i = 0; i < commandLine.getActionCount(); i++)
    {
        Action * action = commandLine.getActionAt(i);
        if (!action->execute(currentVfd))
        {   //  OOPS!
            printf("ABORTING\n");
            exitCode = EXIT_FAILURE;
            break;
        }
    }

    //  Cleanup - just in case the "current" VFD is still open
    delete currentVfd;

    //  Done
    return exitCode;
}

//  End of main.cpp
