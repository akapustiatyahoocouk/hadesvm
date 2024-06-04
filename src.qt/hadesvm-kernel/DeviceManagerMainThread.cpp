//
//  hadesvm-kernel/DeviceManagerMainThread.cpp
//
//  hadesvm::kernel::DeviceManagerMainThread class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction
DeviceManagerMainThread::DeviceManagerMainThread(Kernel * kernel, Process * process)
    :   NativeThread(kernel, process)
{
}

DeviceManagerMainThread::~DeviceManagerMainThread()
{
}

//////////
//  NativeThread
void DeviceManagerMainThread::run()
{
    //  TODO implement
}

//  End of hadesvm-kernel/DeviceManagerMainThread.cpp
