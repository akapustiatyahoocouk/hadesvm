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
        //  Links to other kernel objects
        _interestingAtoms(),
        _implementedServers(),
        _openHandles()
{
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
    if (_parent != nullptr)
    {
        _parent->decrementReferenceCount(); //  we've just dropped a reference in "_parent"
        _parent->_children.remove(this);
        this->decrementReferenceCount(); //  we've just dropped a reference to "*this" in "_parent->_children"
    }
}

//  End of hadesvm-kernel/Process.cpp
