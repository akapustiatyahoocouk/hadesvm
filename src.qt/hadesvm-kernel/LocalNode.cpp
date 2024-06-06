//
//  hadesvm-kernel/LocalNode.cpp
//
//  hadesvm::kernel::LocalNode class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction
LocalNode::LocalNode(Kernel * kernel,
                     const QUuid & uuid, const QString & name)
    :   Node(kernel, uuid, name)
{
    Q_ASSERT(kernel->isLockedByCurrentThread());
}

LocalNode::~LocalNode()
{
    Kernel * kernel = this->kernel();
    Q_ASSERT(kernel->isLockedByCurrentThread());
}

//  End of hadesvm-kernel/LocalNode.cpp
