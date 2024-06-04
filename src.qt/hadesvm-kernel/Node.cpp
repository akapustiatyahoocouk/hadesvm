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
        _name(name)
{
}

Node::~Node()
{
}

//  End of hadesvm-kernel/Node.cpp
