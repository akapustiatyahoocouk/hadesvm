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
        _runnerThread(nullptr)
{
}

NativeThread::~NativeThread()
{
}

//////////
//  NativeThread::SystemCalls
NativeThread::SystemCalls::SystemCalls(NativeThread * nativeThread)
    :   _nativeThread(nativeThread)
{
}

NativeThread::SystemCalls::~SystemCalls()
{
}

//  End of hadesvm-kernel/NativeThread.cpp
