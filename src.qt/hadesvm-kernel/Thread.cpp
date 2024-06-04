//
//  hadesvm-kernel/Thread.cpp
//
//  hadesvm::kernel::Thread class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction
Thread::Thread(Kernel * kernel, Process * process)
    :   Object(kernel),
        _process(process),
        _state(State::Constructed),
        _suspendCount(0),
        _exitCode(ExitCode::Unknown),
        _isDaemon(false)
{
    Q_ASSERT(_process != nullptr && _process->live() && _process->kernel() == kernel);
    _process->incrementReferenceCount();    //  we've just saved a reference in "_process"

    _process->_threads.insert(this);
    this->incrementReferenceCount(); //  we've just saved a reference to "*this" in "_process->_threads"
    if (_process->_mainThread == nullptr)
    {   //  The 1st Thread of a Process becomes its main thread
        _process->_mainThread = this;
        this->incrementReferenceCount(); //  we've just saved a reference to "*this" in "_process->_mainThread"
    }
}

Thread::~Thread()
{
    _process->decrementReferenceCount();    //  we've just dropped a reference in "_process"

    _process->_threads.remove(this);
    this->decrementReferenceCount(); //  we've just dropped a reference to "*this" in "_process->_threads"
    if (_process->_mainThread == this)
    {
        _process->_mainThread = nullptr;
        this->decrementReferenceCount(); //  we've just dropped a reference to "*this" in "_process->_mainThread"
    }
}

//  End of hadesvm-kernel/Thread.cpp
