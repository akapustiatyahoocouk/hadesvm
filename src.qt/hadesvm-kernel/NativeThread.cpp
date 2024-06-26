//
//  hadesvm-kernel/NativeThread.cpp
//
//  hadesvm::kernel::NativeThread class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction
NativeThread::NativeThread(Kernel * kernel, Process * process)
    :   Thread(kernel, process),
        systemCalls(this),
        _terminationRequested(false),
        _runnerThread(nullptr)
{
    Q_ASSERT(kernel->isLockedByCurrentThread());
}

NativeThread::~NativeThread()
{
    Kernel * kernel = this->kernel();
    Q_ASSERT(kernel->isLockedByCurrentThread());

    terminate();            //  ...just in case
    delete _runnerThread;   //  "delete nullptr" is safe
}

//////////
//  Operations
void NativeThread::start()
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    if (state() != State::Constructed)
    {   //  OOPS! Can't!
        return;
    }

    Q_ASSERT(_runnerThread == nullptr);
    _runnerThread = new _RunnerThread(this);
    _state = State::Running;
    _runnerThread->start();
}

void NativeThread::terminate()
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    if (state() == State::Finished)
    {   //  Nothing to do
        return;
    }
    if (state() == State::Constructed)
    {   //  Go straight to Finished state
        _exitCode = ExitCode::Terminated;
        _state = State::Finished;
    }

    //  If we are here, no runner thread has a lock on Kernel!
    Q_ASSERT(_runnerThread != nullptr);
    _terminationRequested = true;

    //  TODO where do we perform this cleanup?
    //_runnerThread->wait();
    //delete _runnerThread;
    //_runnerThread = nullptr;

    _exitCode = ExitCode::Terminated;
    _state = State::Finished;
}

//////////
//  NativeThread::_RunnerThread
NativeThread::_RunnerThread::_RunnerThread(NativeThread * nativeThread)
    :   _nativeThread(nativeThread),
        _kernel(nativeThread->kernel())
{
    setTerminationEnabled();
}

NativeThread::_RunnerThread::~_RunnerThread()
{
}

void NativeThread::_RunnerThread::run()
{
    try
    {
        ExitCode exitCode = _nativeThread->run();

        //  TODO access atomicity ?
        _nativeThread->_exitCode = exitCode;
        _nativeThread->_state = State::Finished;
    }
    catch (ExitCode exitCode)
    {   //  NativeThread::run() has called systemCalls.exitThread();
        //  TODO access atomicity ?
        _nativeThread->_exitCode = exitCode;
        _nativeThread->_state = State::Finished;
    }
    catch (Process::ExitCode /*exitCode*/)
    {   //  NativeThread::run() has called systemCalls.exitProcess();
        failure();    //  TODO implement properly
    }
    catch (...)
    {   //  NativeThread::run() has thrown something
        failure();    //  TODO implement properly
    }
}

//  End of hadesvm-kernel/NativeThread.cpp
