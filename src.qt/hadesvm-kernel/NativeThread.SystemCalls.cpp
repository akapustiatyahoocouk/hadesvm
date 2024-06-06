//
//  hadesvm-kernel/NativeThread.SystemCalls.cpp
//
//  hadesvm::kernel::NativeThread::SystemCalls class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction
NativeThread::SystemCalls::SystemCalls(NativeThread * nativeThread)
    :   _nativeThread(nativeThread)
{
    Q_ASSERT(_nativeThread != nullptr);
}

NativeThread::SystemCalls::~SystemCalls()
{
}

//////////
//  Operations (miscellaneous)
#define HANDLE_TERMINATION_REQUEST()                        \
    if (_nativeThread->_terminationRequested.load())        \
    {                                                       \
        throw _nativeThread->_terminationExitCode.load();   \
    }

int NativeThread::SystemCalls::getSystemVersion()
{
    HANDLE_TERMINATION_REQUEST();

    //  TODO delegate to Kernel::SystemCalls
    int result = 1;

    HANDLE_TERMINATION_REQUEST();
    return result;
}

//  End of hadesvm-kernel/NativeThread.SystemCalls.cpp
