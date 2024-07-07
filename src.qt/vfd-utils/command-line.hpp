//
//  command-line.hpp - Command line API
//
//////////

//////////
//  The command line
class CommandLine
{
    //////////
    //  Can't assign or copy construct
private:
    CommandLine(const CommandLine &);
    void                operator = (const CommandLine &);

    //////////
    //  Construction/destruction
public:
    CommandLine(int argc, char ** argv);
    ~CommandLine();

    //////////
    //  Operations
public:
    //  Prints the application banner
    static void         printBanner();

    //  Prints the application usage information
    static void         printUsage();

    //  TODO document
    size_t              getActionCount() const { return _actions.size(); }
    Action *            getActionAt(size_t index) const { return _actions[index]; }

    //////////
    //  Implementation
private:
    vector<Action*>     _actions;

    //  Helpers
    static void         _error(const char * format, ...);
    static VfdFormat    _parseVfdFormat(const char * specification);
    bool                _parseCreateVfdAction(int argc, char ** argv, int & scan);
    bool                _parseOpenVfdAction(int argc, char ** argv, int & scan);
    bool                _parseListDirectoryAction(int argc, char ** argv, int & scan);
    bool                _parseCreateDirectoryAction(int argc, char ** argv, int & scan);
    bool                _parsePutAction(int argc, char ** argv, int & scan);
    bool                _parseMakeBootableAction(int argc, char ** argv, int & scan);
    bool                _parseCreateFileAction(int argc, char ** argv, int & scan);
    bool                _parseAppendLineAction(int argc, char ** argv, int & scan);
};

//  End of command-line.hpp
