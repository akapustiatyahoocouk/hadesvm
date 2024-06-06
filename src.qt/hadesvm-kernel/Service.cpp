//
//  hadesvm-kernel/Service.cpp
//
//  hadesvm::kernel::Service class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction
Service::Service(Kernel * kernel, Process * serverProcess,
                 const QString & name, unsigned int version,
                 unsigned int maxParameters, unsigned int backlog)
    :   Server(kernel, serverProcess, maxParameters, backlog),
        _name(name),
        _version(version)
{
    Q_ASSERT(kernel->isLockedByCurrentThread());
}

Service::~Service()
{
    Kernel * kernel = this->kernel();
    Q_ASSERT(kernel->isLockedByCurrentThread());
}

//////////
//  Operations
QString Service::name() const
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    return _name;
}

unsigned int Service::version() const
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    return _version;
}

//  End of hadesvm-kernel/Service.cpp
