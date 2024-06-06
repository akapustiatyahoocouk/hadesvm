//
//  hadesvm-kernel/Process.cpp
//
//  hadesvm::kernel::Process class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction
Process::Process(Kernel * kernel, Process * parent, const QString & name)
    :   Object(kernel),
        _parent(parent),
        _children(),
        //  Properties
        _name(name),
        _state(State::Constructed),
        _suspendCount(0),
        _threads(),
        _exitCode(ExitCode::Unknown),
        _mainThread(nullptr),
        //  Open handle table
        _openHandles(),
        //  Links to other kernel objects
        _implementedServers(),
        _atomInterests()
{
    Q_ASSERT(kernel->isLockedByCurrentThread());

    if (_parent != nullptr)
    {
        Q_ASSERT(_parent->live() && _parent->kernel() == kernel);
        _parent->incrementReferenceCount(); //  we've just saved a reference in "_parent"
        _parent->_children.insert(this);
        this->incrementReferenceCount(); //  we've just saved a reference to "*this" in "_parent->_children"
    }
}

Process::~Process()
{
    Kernel * kernel = this->kernel();
    Q_ASSERT(kernel->isLockedByCurrentThread());

    if (_parent != nullptr)
    {
        _parent->decrementReferenceCount(); //  we've just dropped a reference in "_parent"
        _parent->_children.remove(this);
        this->decrementReferenceCount(); //  we've just dropped a reference to "*this" in "_parent->_children"
    }
}

//////////
//  Operations
QString Process::name() const
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    return _name;
}

Process::State Process::state() const
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    return _state;
}

Process::ExitCode Process::exitCode() const
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    return _exitCode;
}

KErrno Process::openHandle(Server * server, Handle & handle)
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    //  Validate parameters
    if (server == nullptr || server->kernel() != this->kernel() || !server->live())
    {
        return KErrno::InvalidParameter;
    }
    //  Prepare the "open handles" index - it will become "handle" upon success
    int unusedIndex = -1;
    for (int i = 0; i < _openHandles.count(); i++)
    {
        if (_openHandles[i] == nullptr)
        {   //  Use this one
            unusedIndex = i;
            break;
        }
    }
    if (unusedIndex == -1)
    {   //  Need a new open handles table entry
        unusedIndex = static_cast<int>(_openHandles.count());
        _openHandles.append(nullptr);
    }

    //  Open the handle...
    Q_ASSERT(_openHandles[unusedIndex] == nullptr);
    _openHandles[unusedIndex] = server;
    server->incrementReferenceCount();  //  we've just created a new reference to "server"
    handle = static_cast<Handle>(unusedIndex);
    //  ...post the HandleOpen message there...
    Oid messageTypeAtomOid = kernel()->getAtom(SystemInterfaces::IUnknown::HandleOpen);
    Message * message =
        new Message(kernel(), this, messageTypeAtomOid,
                    Message::Parameter(Message::ParameterType::Handle, handle));
    server->_messageQueue.enqueue(message);
    message->incrementReferenceCount(); //  we've just created a new reference to "handleOpenMessage"
    //  ...and we're done
    return KErrno::OK;
}

//  End of hadesvm-kernel/Process.cpp
