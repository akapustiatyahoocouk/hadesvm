//
//  actions.hpp - Actions
//
//////////

//////////
//  A generic action that can be requested from a command line
class Action
{
    //////////
    //  Can't assign or copy construct
private:
    Action(const Action &);
    void                operator = (const Action &);

    //////////
    //  Construction/destruction
public:
    Action() {}
    virtual ~Action() {}

    //////////
    //  Operations
public:
    //  Executes this action, using and/or affecting the "currentVfd"
    virtual bool        execute(Vfd *& currentVfd) = 0;
};

//////////
//  The "Create a new VFD" action
class CreateVfdAction : public Action
{
    //////////
    //  Can't assign or copy construct
private:
    CreateVfdAction(const CreateVfdAction &);
    void                operator = (const CreateVfdAction &);

    //////////
    //  Construction/destruction
public:
    CreateVfdAction(const string & vfdFileName, VfdFormat vfdFormat)
        :   _vfdFileName(vfdFileName), _vfdFormat(vfdFormat) {}
    virtual ~CreateVfdAction() {}

    //////////
    //  Action
public:
    virtual bool        execute(Vfd *& currentVfd);

    //////////
    //  Implementation
private:
    string              _vfdFileName;
    VfdFormat           _vfdFormat;
};

//////////
//  The "Open an existing VFD" action
class OpenVfdAction : public Action
{
    //////////
    //  Can't assign or copy construct
private:
    OpenVfdAction(const OpenVfdAction &);
    void                operator = (const OpenVfdAction &);

    //////////
    //  Construction/destruction
public:
    explicit OpenVfdAction(const string & vfdFileName) : _vfdFileName(vfdFileName) {}
    virtual ~OpenVfdAction() {}

    //////////
    //  Action
public:
    virtual bool        execute(Vfd *& currentVfd);

    //////////
    //  Implementation
private:
    string              _vfdFileName;
};

//////////
//  The "List directory" action
class ListDirectoryAction : public Action
{
    //////////
    //  Can't assign or copy construct
private:
    ListDirectoryAction(const ListDirectoryAction &);
    void                operator = (const ListDirectoryAction &);

    //////////
    //  Construction/destruction
public:
    explicit ListDirectoryAction(const string & path) : _path(path) {}
    virtual ~ListDirectoryAction() {}

    //////////
    //  Action
public:
    virtual bool        execute(Vfd *& currentVfd);

    //////////
    //  Implementation
private:
    string              _path;
};

//////////
//  The "Create directory" action
class CreateDirectoryAction : public Action
{
    //////////
    //  Can't assign or copy construct
private:
    CreateDirectoryAction(const CreateDirectoryAction &);
    void                operator = (const CreateDirectoryAction &);

    //////////
    //  Construction/destruction
public:
    explicit CreateDirectoryAction(const string & path) : _path(path) {}
    virtual ~CreateDirectoryAction() {}

    //////////
    //  Action
public:
    virtual bool        execute(Vfd *& currentVfd);

    //////////
    //  Implementation
private:
    string              _path;
};

//////////
//  The "Put" action - copies file from host file system to VHD
class PutAction : public Action
{
    //////////
    //  Can't assign or copy construct
private:
    PutAction(const PutAction &);
    void                operator = (const PutAction &);

    //////////
    //  Construction/destruction
public:
    PutAction(const string & hostPath, const string & vfdPath) : _hostPath(hostPath), _vfdPath(vfdPath) {}
    virtual ~PutAction() {}

    //////////
    //  Action
public:
    virtual bool        execute(Vfd *& currentVfd);

    //////////
    //  Implementation
private:
    string              _hostPath;
    string              _vfdPath;
};

//////////
//  The "Make bootable" action - makes a disk bootable by copying boot sector & boot loader
class MakeBootableAction : public Action
{
    //////////
    //  Constants
public:
    enum class Platform
    {
        IbmPc,
        CereonWorkstationBigEndian,
        CereonWorkstationLittleEndian
    };

    //////////
    //  Can't assign or copy construct
private:
    MakeBootableAction(const MakeBootableAction &);
    void                operator = (const MakeBootableAction &);

    //////////
    //  Construction/destruction
public:
    MakeBootableAction(Platform platform, const string & bootLoaderImagePath)
        :   _platform(platform),
            _bootLoaderImagePath(bootLoaderImagePath) {}
    virtual ~MakeBootableAction() {}

    //////////
    //  Action
public:
    virtual bool        execute(Vfd *& currentVfd);

    //////////
    //  Implementation
private:
    static const char * _vfdBootLoaderFileName;
    static const char * _vfdLoadMapFileName;

    Platform            _platform;
    string              _bootLoaderImagePath;

    struct _LoadMapEntry
    {
        _LoadMapEntry()
            :   _sectorsToLoad(), _firstLbaSector(), _pageToLoadAt() {}
        _LoadMapEntry(unsigned int sectorsToLoad, unsigned int firstLbaSector, uint16_t pageToLoadAt)
            :   _sectorsToLoad(sectorsToLoad), _firstLbaSector(firstLbaSector), _pageToLoadAt(pageToLoadAt) {}

        unsigned int    _sectorsToLoad;
        unsigned int    _firstLbaSector;
        uint16_t        _pageToLoadAt;
    };
};

//////////
//  The "Create file" action
class CreateFileAction : public Action
{
    //////////
    //  Can't assign or copy construct
private:
    CreateFileAction(const CreateFileAction &);
    void                operator = (const CreateFileAction &);

    //////////
    //  Construction/destruction
public:
    explicit CreateFileAction(const string & path) : _path(path) {}
    virtual ~CreateFileAction() {}

    //////////
    //  Action
public:
    virtual bool        execute(Vfd *& currentVfd);

    //////////
    //  Implementation
private:
    string              _path;
};

//////////
//  The "Append line" action
class AppendLineAction : public Action
{
    //////////
    //  Can't assign or copy construct
private:
    AppendLineAction(const AppendLineAction &);
    void                operator = (const AppendLineAction &);

    //////////
    //  Construction/destruction
public:
    explicit AppendLineAction(const string & path, const string & line) : _path(path), _line(line) {}
    virtual ~AppendLineAction() {}

    //////////
    //  Action
public:
    virtual bool        execute(Vfd *& currentVfd);

    //////////
    //  Implementation
private:
    string              _path;
    string              _line;
};

//  End of actions.hpp
