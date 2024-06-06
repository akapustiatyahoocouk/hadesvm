//
//  hadesvm-kernel/Server.cpp
//
//  hadesvm::kernel::Server class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction
Server::Server(Kernel * kernel, Process * serverProcess,
               unsigned int maxParameters, unsigned int backlog)
    :   Object(kernel),
        _serverProcess(serverProcess),
        _maxParameters(qMax(1u, maxParameters)),
        _backlog(backlog),
        _messageQueue()
{
    Q_ASSERT(kernel->isLockedByCurrentThread());
    Q_ASSERT(_serverProcess != nullptr && _serverProcess->kernel() == kernel && _serverProcess->live());
}

Server::~Server()
{
    Kernel * kernel = this->kernel();
    Q_ASSERT(kernel->isLockedByCurrentThread());

    Q_ASSERT(_messageQueue.isEmpty());
}

//////////
//  Operations

//  End of hadesvm-kernel/Server.cpp
