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
    :   Message(kernel, senderProcess, messageTypeAtomOid,
                QList<Parameter>())
{
}

Message::Message(Kernel * kernel, Process * senderProcess, Oid messageTypeAtomOid,
                 const Parameter & param0)
    :   Message(kernel, senderProcess, messageTypeAtomOid,
                QList<Parameter>{param0})
{
}

Message::Message(Kernel * kernel, Process * senderProcess, Oid messageTypeAtomOid,
                 const Parameter & param0, const Parameter & param1)
    :   Message(kernel, senderProcess, messageTypeAtomOid,
                QList<Parameter>{param0, param1})
{
}

Message::Message(Kernel * kernel, Process * senderProcess, Oid messageTypeAtomOid,
                 const Parameter & param0, const Parameter & param1,
                 const Parameter & param2)
    :   Message(kernel, senderProcess, messageTypeAtomOid,
                QList<Parameter>{param0, param1, param2})
{
}

Message::Message(Kernel * kernel, Process * senderProcess, Oid messageTypeAtomOid,
                 const Parameter & param0, const Parameter & param1,
                 const Parameter & param2, const Parameter & param3)
    :   Message(kernel, senderProcess, messageTypeAtomOid,
                QList<Parameter>{param0, param1, param2, param3})
{
}

Message::Message(Kernel * kernel, Process * senderProcess, Oid messageTypeAtomOid,
                 const QList<Parameter> & params)
    :   Object(kernel),
        _senderProcess(senderProcess),
        _messageTypeAtomOid(messageTypeAtomOid),
        _senderHandle(Handle::Invalid),
        _server(nullptr),
        _parameters(params),
        _completionCount(),
        _result(KErrno::Unknown),
        _outputs()
{
    Q_ASSERT(kernel->isLockedByCurrentThread());

    Q_ASSERT(_senderProcess != nullptr && _senderProcess->kernel() == kernel && _senderProcess->live());
    _senderProcess->incrementReferenceCount();  //  we've just created a new reference to "_senderProcess"
}

Message::~Message()
{
    Kernel * kernel = this->kernel();
    Q_ASSERT(kernel->isLockedByCurrentThread());
}

//////////
//  Operations
Process * Message::senderProcess() const
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    return _senderProcess;
}

Oid Message::messageTypeAtomOid() const
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    return _messageTypeAtomOid;
}

Message::State Message::state() const
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    return _state;
}

Handle Message::senderHandle() const
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    return _senderHandle;
}

QList<Message::Parameter> Message::parameters() const
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    return _parameters;
}

KErrno Message::result() const
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    return _result;
}

QList<Message::Parameter> Message::outputs() const
{
    Q_ASSERT(kernel()->isLockedByCurrentThread());

    return _outputs;
}

//  End of hadesvm-kernel/Message.cpp
