//
//  hadesvm-kernel/Message.cpp
//
//  hadesvm::kernel::Message class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction/assignment
Message::Message(Kernel * kernel, Process * senderProcess, Oid messageTypeAtomOid)
    :   Object(kernel),
        _senderProcess(senderProcess),
        _messageTypeAtomOid(messageTypeAtomOid),
        _senderHandle(Handle::Invalid),
        _parameters()
{
    Q_ASSERT(kernel->isLockedByCurrentThread());

    Q_ASSERT(_senderProcess != nullptr && _senderProcess->kernel() == kernel && _senderProcess->live());
    _senderProcess->incrementReferenceCount();  //  we've just created a new reference to "_senderProcess"
}

Message::Message(Kernel * kernel, Process * senderProcess, Oid messageTypeAtomOid,
                 const Parameter & param0)
    :   Object(kernel),
        _senderProcess(senderProcess),
        _messageTypeAtomOid(messageTypeAtomOid),
        _senderHandle(Handle::Invalid),
        _parameters()
{
    Q_ASSERT(kernel->isLockedByCurrentThread());

    Q_ASSERT(_senderProcess != nullptr && _senderProcess->kernel() == kernel && _senderProcess->live());
    _senderProcess->incrementReferenceCount();  //  we've just created a new reference to "_senderProcess"

    _parameters.append(param0);
}

Message::Message(Kernel * kernel, Process * senderProcess, Oid messageTypeAtomOid,
                 const Parameter & param0, const Parameter & param1)
    :   Object(kernel),
        _senderProcess(senderProcess),
        _messageTypeAtomOid(messageTypeAtomOid),
        _senderHandle(Handle::Invalid),
        _parameters()
{
    Q_ASSERT(kernel->isLockedByCurrentThread());

    Q_ASSERT(_senderProcess != nullptr && _senderProcess->kernel() == kernel && _senderProcess->live());
    _senderProcess->incrementReferenceCount();  //  we've just created a new reference to "_senderProcess"

    _parameters.append(param0);
    _parameters.append(param1);
}

Message::Message(Kernel * kernel, Process * senderProcess, Oid messageTypeAtomOid,
                 const Parameter & param0, const Parameter & param1,
                 const Parameter & param2)
    :   Object(kernel),
        _senderProcess(senderProcess),
        _messageTypeAtomOid(messageTypeAtomOid),
        _senderHandle(Handle::Invalid),
        _parameters()
{
    Q_ASSERT(kernel->isLockedByCurrentThread());

    Q_ASSERT(_senderProcess != nullptr && _senderProcess->kernel() == kernel && _senderProcess->live());
    _senderProcess->incrementReferenceCount();  //  we've just created a new reference to "_senderProcess"

    _parameters.append(param0);
    _parameters.append(param1);
    _parameters.append(param2);
}

Message::Message(Kernel * kernel, Process * senderProcess, Oid messageTypeAtomOid,
                 const Parameter & param0, const Parameter & param1,
                 const Parameter & param2, const Parameter & param3)
    :   Object(kernel),
        _senderProcess(senderProcess),
        _messageTypeAtomOid(messageTypeAtomOid),
        _senderHandle(Handle::Invalid),
        _parameters()
{
    Q_ASSERT(kernel->isLockedByCurrentThread());

    Q_ASSERT(_senderProcess != nullptr && _senderProcess->kernel() == kernel && _senderProcess->live());
    _senderProcess->incrementReferenceCount();  //  we've just created a new reference to "_senderProcess"

    _parameters.append(param0);
    _parameters.append(param1);
    _parameters.append(param2);
    _parameters.append(param3);
}

Message::~Message()
{
    Kernel * kernel = this->kernel();
    Q_ASSERT(kernel->isLockedByCurrentThread());
}

//  End of hadesvm-kernel/Message.cpp
