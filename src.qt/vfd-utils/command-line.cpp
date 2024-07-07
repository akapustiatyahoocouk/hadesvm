//
//  command-line.cpp - Command line implementation
//
//////////
#include "main.hpp"

//////////
//  Construction/destruction
CommandLine::CommandLine(int argc, char ** argv)
    :   _actions()
{
    for (int scan = 0; scan < argc; )
    {
        if (!_parseCreateVfdAction(argc, argv, scan) &&
            !_parseOpenVfdAction(argc, argv, scan) &&
            !_parseListDirectoryAction(argc, argv, scan) &&
            !_parseCreateDirectoryAction(argc, argv, scan) &&
            !_parsePutAction(argc, argv, scan) &&
            !_parseMakeBootableAction(argc, argv, scan) &&
            !_parseCreateFileAction(argc, argv, scan) &&
            !_parseAppendLineAction(argc, argv, scan))
        {   //  OOPS!
            string unprocessedOptions;
            for (; scan < argc; scan++)
            {
                unprocessedOptions += argv[scan];
                unprocessedOptions += ' ';
            }
            assert(unprocessedOptions.length() != 0);
            unprocessedOptions.resize(unprocessedOptions.length() - 1); //  Chop off tailing space
            _error("Don't know how to process options '%s'", unprocessedOptions.c_str());
        }
    }
}

CommandLine::~CommandLine()
{
    for (size_t i = 0; i < _actions.size(); i++)
    {
        delete _actions[i];
    }
}

//////////
//  Operations
void CommandLine::printBanner()
{
    printf("HADES Virtual Floppy Disk Utilities\n");
    printf("\n");
}

void CommandLine::printUsage()
{
    printf("Usage:\n");
    printf("    vfd-utils <action> [<action>...]\n");
    printf("\n");
    printf("Actions:\n");
    printf("    create <VFD file> <format>\n");
    printf("        Creates a new virtual floppy disk image with empty file system\n");
    printf("        and selects that VFD image as 'current'. Supported formats are:\n");
    printf("            *   1.44M - 3.5' PC floppy with FAT12 file system\n");
    printf("\n");
    printf("    open <VFD file>\n");
    printf("        Selects the specified virtual floppy disk image as 'current'. The\n");
    printf("        file must already exist. Its format and file system are inferred\n");
    printf("        by analyzing the size and contents of the image file.\n");
    printf("\n");
    printf("    dir <absolute path>\n");
    printf("        Lists the contents of the VFD directory with the specified absolute\n");
    printf("        path.\n");
    printf("\n");
    printf("    mkdir <absolute path>\n");
    printf("        Creates a VFD directory with the specified absolute path. An error occurs\n");
    printf("        if the specified directory already exists, if a file already exists at the\n");
    printf("        same absolute path, of if the immediate parent directory for the directory\n");
    printf("        that must be created does not exist.\n");
    printf("\n");
    printf("    put <host file> <absolute path>\n");
    printf("        Copies a file <host file> from the host OS file system to the VFD, placing it\n");
    printf("        at the specified absolute path there. If the VFD already contains a file with\n");
    printf("        the specified absolute path, it is silently overwritten. If VFD already contains\n");
    printf("        a directory at the same absolute path, or the parent directory of the destination\n");
    printf("        file does not exist, an error occurs. If the VFD contains insufficient space to\n");
    printf("        accomodate the entire file, only the part of the file that fits is copied, and then\n");
    printf("        an error occurs.\n");
    printf("\n");
    printf("    make-bootable <platform> <boot loader image file>\n");
    printf("        Makes the VFD bootable by copying the specified boot loader to it as '/bootload',\n");
    printf("        creating the load map for the boot loader as '/loadmap' and setting up the VFD's boot\n");
    printf("        sector to use the load map.\n");
    printf("        The <boot loader image file> must be a raw binary image that can be loaded into a \n");
    printf("        continuous memory block starting at a <platform> - specific address which, when\n");
    printf("        control is transferred to its 1st byte, initiates the boot loading.\n");
    printf("        The <platform> can be:\n");
    printf("            *   ibm-pc      Prepares the IBM PC boot floppy; the boot loaded image will\n");
    printf("                            be loaded at 0x0800:0x0000 segmented 16-bit real mode address\n");
    printf("                            and then executed in 16-bit real mode.\n");
    printf("            *   cereon-be   Prepares the Cereon Workstation (big-endian) boot floppy;\n");
    printf("                            the boot loader will be loaded at linear address 0x00030000\n");
    printf("                            and then executed in real/kernel mode.\n");
    printf("            *   cereon-le   Same as above, but for little-endian Cereon Workstations\n");
    printf("\n");
    printf("    create <absolute path>\n");
    printf("        Creates an empty VFD file with the specified absolute path. An error occurs\n");
    printf("        if the specified file already exists, if a directory already exists at the\n");
    printf("        same absolute path, of if the immediate parent directory for the file that\n");
    printf("        must be created does not exist.\n");
    printf("\n");
    printf("    append-line <absolute path> <line>\n");
    printf("        Appends a single line of text to the end of the specified file and then also\n");
    printf("        adds a newline character. If <line> contains spaces or other specifi characters,\n");
    printf("        then it must be quoted or escape sequences included into it in order for it to\n");
    printf("        appear as a single command line parameter.\n");
    printf("\n");
    printf("NOTE:\n");
    printf("    Absolute paths of files and directories in VFDs must always use a forward\n");
    printf("    slash / as path separator. For exampler the action 'dir /' lists the content\n");
    printf("    of the VFD root directory.\n");
    printf("\n");
}

//////////
//  Implementation helpers
void CommandLine::_error(const char * format, ...)
{
    va_list va;
    va_start(va, format);
    vprintf(format, va);
    printf("\n");
    exit(EXIT_FAILURE);
}

VfdFormat CommandLine::_parseVfdFormat(const char * specification)
{
    if (strcmp(specification, "1.44M") == 0)
    {
        return VfdFormat_144M;
    }
    else
    {
        return VfdFormat_Unknown;
    }
}

bool CommandLine::_parseCreateVfdAction(int argc, char ** argv, int & scan)
{
    if (scan + 3 <= argc && strcmp(argv[scan], "create") == 0)
    {
        VfdFormat vfdFormat = _parseVfdFormat(argv[scan + 2]);
        if (vfdFormat == VfdFormat_Unknown)
        {   //  OOPS!
            _error("Unknown VFD format '%s'", argv[scan + 2]);
        }
        _actions.push_back(new CreateVfdAction(argv[scan + 1], vfdFormat));
        scan += 3;
        return true;
    }
    else
    {
        return false;
    }
}

bool CommandLine::_parseOpenVfdAction(int argc, char ** argv, int & scan)
{
    if (scan + 2 <= argc && strcmp(argv[scan], "open") == 0)
    {
        _actions.push_back(new OpenVfdAction(argv[scan + 1]));
        scan += 2;
        return true;
    }
    else
    {
        return false;
    }
}

bool CommandLine::_parseListDirectoryAction(int argc, char ** argv, int & scan)
{
    if (scan + 2 <= argc && strcmp(argv[scan], "dir") == 0)
    {
        _actions.push_back(new ListDirectoryAction(argv[scan + 1]));
        scan += 2;
        return true;
    }
    else
    {
        return false;
    }
}

bool CommandLine::_parseCreateDirectoryAction(int argc, char ** argv, int & scan)
{
    if (scan + 2 <= argc && strcmp(argv[scan], "mkdir") == 0)
    {
        _actions.push_back(new CreateDirectoryAction(argv[scan + 1]));
        scan += 2;
        return true;
    }
    else
    {
        return false;
    }
}

bool CommandLine::_parsePutAction(int argc, char ** argv, int & scan)
{
    if (scan + 3 <= argc && strcmp(argv[scan], "put") == 0)
    {
        _actions.push_back(new PutAction(argv[scan + 1], argv[scan + 2]));
        scan += 3;
        return true;
    }
    else
    {
        return false;
    }
}

bool CommandLine::_parseMakeBootableAction(int argc, char ** argv, int & scan)
{
    if (scan + 3 <= argc && strcmp(argv[scan], "make-bootable") == 0)
    {
        MakeBootableAction::Platform platform;
        if (strcmp(argv[scan + 1], "ibm-pc") == 0)
        {
            platform = MakeBootableAction::Platform::IbmPc;
        }
        else if (strcmp(argv[scan + 1], "cereon-be") == 0)
        {
            platform = MakeBootableAction::Platform::CereonWorkstationBigEndian;
        }
        else if (strcmp(argv[scan + 1], "cereon-le") == 0)
        {
            platform = MakeBootableAction::Platform::CereonWorkstationLittleEndian;
        }
        else
        {
            return false;
        }
        _actions.push_back(new MakeBootableAction(platform, argv[scan + 2]));
        scan += 3;
        return true;
    }
    else
    {
        return false;
    }
}

bool CommandLine::_parseCreateFileAction(int argc, char ** argv, int & scan)
{
    if (scan + 2 <= argc && strcmp(argv[scan], "create") == 0)
    {
        _actions.push_back(new CreateFileAction(argv[scan + 1]));
        scan += 2;
        return true;
    }
    else
    {
        return false;
    }
}

bool CommandLine::_parseAppendLineAction(int argc, char ** argv, int & scan)
{
    if (scan + 3 <= argc && strcmp(argv[scan], "append-line") == 0)
    {
        _actions.push_back(new AppendLineAction(argv[scan + 1], argv[scan + 2]));
        scan += 3;
        return true;
    }
    else
    {
        return false;
    }
}

//  End of command-line.cpp
