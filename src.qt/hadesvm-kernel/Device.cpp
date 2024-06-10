//
//  hadesvm-kernel/Device.cpp
//
//  hadesvm::kernel::Deviceclass implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction - from friends only
Device::Device(Kernel * kernel, Node * node, const QString & name)
    :   Object(kernel),
        _node(node),
        _name(name)
{
    Q_ASSERT(kernel->isLockedByCurrentThread());
    _node->incrementReferenceCount();   //  we've just created a new reference to "_node"

    //  Register with the Node
    Q_ASSERT(node->live() && kernel == node->kernel());
    Q_ASSERT(node->findDeviceByName(name) == nullptr);
    node->_devicesByName[name] = this;
    this->incrementReferenceCount();    //  we've just created a new reference to this Device
}

Device::~Device()
{
    Kernel * kernel = this->kernel();
    Q_ASSERT(kernel->isLockedByCurrentThread());

    Q_ASSERT(_node->findDeviceByName(_name) == this);
    _node->_devicesByName.remove(_name);
    this->decrementReferenceCount();    //  we've just dropped a reference to this Device
}

//  End of hadesvm-kernel/Device.cpp
