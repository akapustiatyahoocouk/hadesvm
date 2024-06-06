//
//  hadesvm-kernel/Node.cpp
//
//  hadesvm::kernel::Node class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction
Node::Node(Kernel * kernel,
           const QUuid & uuid, const QString & name)
    :   Object(kernel),
        _uuid(uuid),
        _name(name),
        _devices()
{
    Q_ASSERT(kernel->isLockedByCurrentThread());
    Q_ASSERT(!kernel->_nodesByUuid.contains(uuid));

    //  Register the new node
    kernel->_nodesByUuid[uuid] = this;
    this->incrementReferenceCount();    //  we've just created a new reference to this Node
}

Node::~Node()
{
    Kernel * kernel = this->kernel();
    Q_ASSERT(kernel->isLockedByCurrentThread());
}

//////////
//  Operations
QUuid Node::uuid() const
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    return _uuid;
}

QString Node::name() const
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    return _name;
}

Device * Node::findDeviceByName(const QString & name) const
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    return _devices.contains(name) ? _devices[name] : nullptr;;
}

//  End of hadesvm-kernel/Node.cpp
