//
//  hadesvm-kernel/MountedFolderDevice.cpp
//
//  hadesvm::kernel::MountedFolderDevice class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction
MountedFolderDevice::MountedFolderDevice(Kernel * kernel, Node * node,
                                         const QString & deviceName, const QString & externalFolderPath)
    :   Device(kernel, node, deviceName),
        _externalFolderPath(externalFolderPath)
{
    Q_ASSERT(kernel->isLockedByCurrentThread());
}

MountedFolderDevice::~MountedFolderDevice()
{
    Kernel * kernel = this->kernel();
    Q_ASSERT(kernel->isLockedByCurrentThread());
}

//////////
//  Operations
QString MountedFolderDevice::externalFolderPath() const
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    return _externalFolderPath;
}

//  End of hadesvm-kernel/MountedFolderDevice.cpp
