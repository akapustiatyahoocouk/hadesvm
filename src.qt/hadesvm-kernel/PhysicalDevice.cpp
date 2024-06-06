//
//  hadesvm-kernel/PhysicalDevice.cpp
//
//  hadesvm::kernel::PhysicalDeviceclass implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction - from friends only
PhysicalDevice::PhysicalDevice(Kernel * kernel, Node * node, const QString & name,
                               IDeviceComponent * component)
    :   Device(kernel, node, name),
        _component(component)
{
    Q_ASSERT(kernel->isLockedByCurrentThread());
    Q_ASSERT(_component != nullptr);
}

PhysicalDevice::~PhysicalDevice()
{
    Kernel * kernel = this->kernel();
    Q_ASSERT(kernel->isLockedByCurrentThread());
}

//////////
//  Operations
IDeviceComponent * PhysicalDevice::component() const
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());
    return _component;
}

//  End of hadesvm-kernel/PhysicalDevice.cpp
