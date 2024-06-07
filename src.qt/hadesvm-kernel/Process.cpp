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
        _atomInterests(),
        //  Special atoms
        _handleOpenMethodAtomOid(Oid::Invalid),
        _handleOpenMethodErrno(KErrno::OK),
        _handleClosedMethodAtomOid(Oid::Invalid),
        _handleClosedMethodErrno(KErrno::OK)
{
    Q_ASSERT(kernel->isLockedByCurrentThread());

    if (_parent != nullptr)
    {
        Q_ASSERT(_parent->live() && _parent->kernel() == kernel);
        _parent->incrementReferenceCount(); //  we've just saved a reference in "_parent"
        _parent->_children.insert(this);
        this->incrementReferenceCount(); //  we've just saved a reference to "*this" in "_parent->_children"
    }

    //  Grab special atoms
    _handleOpenMethodErrno = getAtom(SystemInterfaces::IUnknown::HandleOpen, _handleOpenMethodAtomOid);
    _handleClosedMethodErrno = getAtom(SystemInterfaces::IUnknown::HandleClosed, _handleOpenMethodAtomOid);
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

    //  Release special atoms
    releaseAtom(_handleOpenMethodAtomOid);
    releaseAtom(_handleClosedMethodAtomOid);
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

KErrno Process::getAtom(const QString & name, Oid & atomId)
{
    Kernel * kernel = this->kernel();
    Q_ASSERT(kernel->isLockedByCurrentThread());

    //  Do the work
    if (_atomInterests.contains(name))
    {   //  Just incremenent the reference count
        _AtomInterest * atomInterest = _atomInterests[name];
        atomInterest->_interestCount++;
        if (atomInterest->_interestCount == 0)
        {   //  OOPS! Overflow!
            atomInterest->_interestCount--;
            return KErrno::LimitReached;
        }
        return KErrno::OK;
    }
    else
    {   //  Need to find the Atom...
        Atom * atom;
        if (!kernel->_atomsByName.contains(name))
        {   //  Atom does not yet exist - create it now
            atom = new Atom(kernel, name);
            Q_ASSERT(kernel->_atomsByName.contains(name));
        }
        else
        {
            atom = kernel->_atomsByName[name];
        }
        //  Create an interest...
        _AtomInterest * atomInterest = new _AtomInterest();
        atomInterest->_process = this;
        atomInterest->_atom = atom;
        atomInterest->_interestCount = 1;
        atomInterest->_process->incrementReferenceCount();  //  we've just stored a reference to Process
        atomInterest->_atom->incrementReferenceCount();     //  we've just stored a reference to Atom
        //  ...record it
        _atomInterests.insert(name, atomInterest);
        atom->_atomInterests.insert(atomInterest);
        //  ...and we're done
        atomId = atom->oid();
        return KErrno::OK;
    }
}

KErrno Process::releaseAtom(Oid atomOid)
{
    Kernel * kernel = this->kernel();
    Q_ASSERT(kernel->isLockedByCurrentThread());

    if (!kernel->_liveObjects.contains(atomOid))
    {
        return KErrno::InvalidParameter;
    }
    Atom * atom = dynamic_cast<Atom*>(kernel->_liveObjects[atomOid]);
    if (atom == nullptr)
    {
        return KErrno::InvalidParameter;
    }
    QString atomName = atom->name();

    //  If the "process" has no interest in "atom", there's nothing to do
    if (!_atomInterests.contains(atomName))
    {
        return KErrno::InvalidParameter;
    }
    //  Decrement interest count...
    _AtomInterest * atomInterest = _atomInterests[atomName];
    Q_ASSERT(atomInterest->_interestCount > 0);
    atomInterest->_interestCount--;
    //  ...and drop the interest if the interest count drops to zero
    if (atomInterest->_interestCount == 0)
    {
        atom->_atomInterests.remove(atomInterest);
        _atomInterests.remove(atomName);
        this->decrementReferenceCount(); //  we're about to drop a reference to Process from atomInterest
        atom->decrementReferenceCount();    //  we're about to drop a reference to Atom from atomInterest
        delete atomInterest;
    }

    //  TODO can we destroy Atom that no Process has an interest in ?
    //  All done
    return KErrno::OK;
}

KErrno Process::openHandle(Server * server, Handle & handle)
{
    Kernel * kernel = this->kernel();
    Q_ASSERT(kernel->isLockedByCurrentThread());

    //  Validate parameters
    if (server == nullptr || server->kernel() != kernel || !server->live())
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
    server->incrementOpenHandleCount(); //  we've just created a new handle to "server"
    //  ...post the HandleOpen message there...
    Message * message =
        new Message(kernel, this, _handleOpenMethodAtomOid,
                    Message::Parameter(Message::ParameterType::Handle, handle));
    message->_senderHandle = handle;
    server->_messageQueue.enqueue(message);
    server->_messageQueueSize.release();
    message->_state = Message::State::Posted;
    message->incrementReferenceCount(); //  we've just created a new reference to "message"
    //  ...and we're done
    return KErrno::OK;
}

KErrno Process::closeHandle(Handle handle)
{
    Kernel * kernel = this->kernel();
    Q_ASSERT(kernel->isLockedByCurrentThread());

    //  Validate parameters
    int index = static_cast<int>(handle);
    if (index < 0 || index >= _openHandles.count() || _openHandles[index] == nullptr)
    {
        return KErrno::InvalidParameter;
    }

    //  Close...
    Server * server = _openHandles[index];
    _openHandles[index] = nullptr;
    server->decrementReferenceCount();  //  we've just dropped a reference to the "server"
    server->decrementOpenHandleCount(); //  we've just dropped a handle to the "server"
    //  ...notify the server that a handle has been closed...
    Message * message =
        new Message(kernel, this, _handleClosedMethodAtomOid,
                    Message::Parameter(Message::ParameterType::Handle, handle));
    message->_senderHandle = handle;
    server->_messageQueue.enqueue(message);
    server->_messageQueueSize.release();
    message->_state = Message::State::Posted;
    message->incrementReferenceCount(); //  we've just created a new reference to "message"
    //  ...and we're done
    return KErrno::OK;
}

Server * Process::serverForHandle(Handle handle)
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    //  Validate parameters
    int index = static_cast<int>(handle);
    if (index < 0 || index >= _openHandles.count() || _openHandles[index] == nullptr)
    {
        return nullptr;
    }

    return _openHandles[index];
}

//  End of hadesvm-kernel/Process.cpp
