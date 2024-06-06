//
//  hadesvm-kernel/Kernel.SystemCalls.cpp
//
//  hadesvm::kernel::Kernel::SystemCalls class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction - from friends only
Kernel::SystemCalls::SystemCalls(Kernel * kernel)
    :   _kernel(kernel)
{
    Q_ASSERT(kernel != nullptr);
}

Kernel::SystemCalls::~SystemCalls()
{
}

//////////
//  Operations (miscellaneous)
QVersionNumber Kernel::SystemCalls::getSystemVersion()
{
    QMutexLocker lock(&_kernel->_runtimeStateGuard);
    return QVersionNumber(1, 0, 0);
}

//  End of hadesvm-kernel/Kernel.SystemCalls.cpp
