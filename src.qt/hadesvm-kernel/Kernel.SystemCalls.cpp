//
//  hadesvm-kernel/Kernel.SystemCalls.cpp
//
//  hadesvm::kernel::Kernel::SystemCalls class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction - from friends only
Kernel::SystemCalls::SystemCalls(Kernel * kernel)
    :   _kernel(kernel)
{
    Q_ASSERT(kernel != nullptr);
}

Kernel::SystemCalls::~SystemCalls()
{
}

//////////
//  Operations (atoms)
KErrno Kernel::SystemCalls::getAtom(Thread * thread, const QString & name, Oid & atomId)
{
    QMutexLocker lock(_kernel);

    if (thread == nullptr || thread->kernel() != _kernel || !thread->live())
    {
        return KErrno::InvalidParameter;
    }
    Process * process = thread->process();

    return process->getAtom(name, atomId);
}

KErrno Kernel::SystemCalls::releaseAtom(Thread * thread, Oid atomOid)
{
    QMutexLocker lock(_kernel);

    if (thread == nullptr || thread->kernel() != _kernel || !thread->live())
    {
        return KErrno::InvalidParameter;
    }
    Process * process = thread->process();

    return process->releaseAtom(atomOid);
}

KErrno Kernel::SystemCalls::getAtomName(Oid /*atomOid*/, QString & /*name*/)
{
    QMutexLocker lock(_kernel);

    Q_ASSERT(false);    //  TODO implement properly
    return KErrno::NotImplemented;
}

//////////
//  Operations (services and servlets)
KErrno Kernel::SystemCalls::createService(Thread * thread,
                                          const QString & name, unsigned int version,
                                          unsigned int maxParameters, unsigned int backlog,
                                          Handle & handle)
{
    QMutexLocker lock(_kernel);

    //  Validate parameters
    if (thread == nullptr || thread->kernel() != _kernel || !thread->live())
    {
        return KErrno::InvalidParameter;
    }
    Process * process = thread->process();

    if (!Kernel::isValidServiceName(name))
    {
        return KErrno::InvalidParameter;
    }
    if (version == 0)
    {
        return KErrno::InvalidParameter;
    }

    //  No duplicates!
    for (Object * obj : _kernel->_liveObjects.values())
    {
        if (Service * srv = dynamic_cast<Service*>(obj))
        {
            if (srv->name() == name && srv->version() == version)
            {   //  OOPS!
                return KErrno::AlreadyExists;
            }
        }
    }

    //  Create a new Service...
    Service * service = new Service(_kernel, process, name, version, maxParameters, backlog);
    //  ...and open a handle to it on behalf of the calling Process
    KErrno err = process->openHandle(service, handle);  //  Posts the "HandleOpen" message to the service
    if (err != KErrno::OK)
    {   //  OOPS! The newly created Service cannot be used!
        delete service;
        return err;
    }

    //  Done
    return KErrno::OK;
}

KErrno Kernel::SystemCalls::openService(Thread * thread,
                                        const QString & name, unsigned int version,
                                        Handle & handle)
{
    QMutexLocker lock(_kernel);

    if (thread == nullptr || thread->kernel() != _kernel || !thread->live())
    {
        return KErrno::InvalidParameter;
    }
    Process * process = thread->process();

    //  Locate the Service...
    Service * service = nullptr;
    for (Object * obj : _kernel->_liveObjects.values())
    {
        if (Service * s = dynamic_cast<Service*>(obj))
        {   //  Match the name/version
            if (s->name() == name)
            {
                if ((version != 0 && version == s->version()) ||
                    (version == 0 && (service == nullptr || s->version() > service->version())))
                {
                    service = s;
                    break;
                }
            }
        }
    }
    if (service == nullptr)
    {
        return KErrno::DoesNotExist;
    }

    //  ...and open a handle to it on behalf of the calling Process
    KErrno err = process->openHandle(service, handle);  //  Posts the "HandleOpen" message to the service
    if (err != KErrno::OK)
    {   //  OOPS!
        return err;
    }

    //  Done
    return KErrno::OK;
}

//////////
//  Operations (messages)
KErrno Kernel::SystemCalls::createMessage(Thread * thread, Oid messageTypeAtomOid,
                                          Oid & messageOid)
{
    return createMessage(thread, messageTypeAtomOid,
                         QList<Message::Parameter>(), messageOid);
}

KErrno Kernel::SystemCalls::createMessage(Thread * thread, Oid messageTypeAtomOid,
                                          const Message::Parameter & param0,
                                          Oid & messageOid)
{
    return createMessage(thread, messageTypeAtomOid,
                         QList<Message::Parameter>{param0}, messageOid);
}

KErrno Kernel::SystemCalls::createMessage(Thread * thread, Oid messageTypeAtomOid,
                                          const Message::Parameter & param0,
                                          const Message::Parameter & param1,
                                          Oid & messageOid)
{
    return createMessage(thread, messageTypeAtomOid,
                         QList<Message::Parameter>{param0, param1}, messageOid);
}

KErrno Kernel::SystemCalls::createMessage(Thread * thread, Oid messageTypeAtomOid,
                                          const Message::Parameter & param0,
                                          const Message::Parameter & param1,
                                          const Message::Parameter & param2,
                                          Oid & messageOid)
{
    return createMessage(thread, messageTypeAtomOid,
                         QList<Message::Parameter>{param0, param1, param2}, messageOid);
}

KErrno Kernel::SystemCalls::createMessage(Thread * thread, Oid messageTypeAtomOid,
                                          const Message::Parameter & param0,
                                          const Message::Parameter & param1,
                                          const Message::Parameter & param2,
                                          const Message::Parameter & param3,
                                          Oid & messageOid)
{
    return createMessage(thread, messageTypeAtomOid,
                         QList<Message::Parameter>{param0, param1, param2, param3}, messageOid);
}

KErrno Kernel::SystemCalls::createMessage(Thread * thread, Oid messageTypeAtomOid,
                                          const QList<Message::Parameter> & params,
                                          Oid & messageOid)
{
    QMutexLocker lock(_kernel);

    if (thread == nullptr || thread->kernel() != _kernel || !thread->live())
    {
        return KErrno::InvalidParameter;
    }
    Kernel * kernel = thread->kernel();
    Process * process = thread->process();

    if (!kernel->_liveObjects.contains(messageTypeAtomOid))
    {
        return KErrno::InvalidParameter;
    }
    Atom * atom = dynamic_cast<Atom*>(kernel->_liveObjects[messageTypeAtomOid]);
    if (atom == nullptr || !process->_atomInterests.contains(atom->name()))
    {   //  Process must getAtom before it can use that Atom
        return KErrno::InvalidParameter;
    }

    //  Create a new Message
    Message * message = new Message(_kernel, process, messageTypeAtomOid, params);
    messageOid = message->oid();
    return KErrno::OK;
}

KErrno Kernel::SystemCalls::postMessage(Thread * thread, Handle handle, Oid messageOid)
{
    QMutexLocker lock(_kernel);

    if (thread == nullptr || thread->kernel() != _kernel || !thread->live())
    {
        return KErrno::InvalidParameter;
    }
    Kernel * kernel = thread->kernel();
    Process * process = thread->process();

    if (!kernel->_liveObjects.contains(messageOid))
    {
        return KErrno::InvalidParameter;
    }
    Message * message = dynamic_cast<Message*>(kernel->_liveObjects[messageOid]);
    if (message == nullptr || message->senderProcess() != process ||
        message->state() != Message::State::Constructed)
    {   //  Process must createMessage before it can post that Message
        return KErrno::InvalidParameter;
    }

    //  Post the message
    Server * server = process->serverForHandle(handle);
    if (server == nullptr)
    {
        return KErrno::InvalidParameter;
    }

    message->_senderHandle = handle;
    message->_server = server;
    server->_messageQueue.enqueue(message);
    server->_messageQueueSize.release();
    message->_state = Message::State::Posted;
    message->incrementReferenceCount(); //  we've just created a new reference to "message"
    return KErrno::OK;
}

KErrno Kernel::SystemCalls::completeMessage(Thread * thread,
                       Oid messageOid, KErrno messageResult,
                       const QList<Message::Parameter> & messageOutputs)
{
    QMutexLocker lock(_kernel);

    if (thread == nullptr || thread->kernel() != _kernel || !thread->live())
    {
        return KErrno::InvalidParameter;
    }
    Kernel * kernel = thread->kernel();
    Process * process = thread->process();

    if (!kernel->_liveObjects.contains(messageOid))
    {
        return KErrno::InvalidParameter;
    }
    Message * message = dynamic_cast<Message*>(kernel->_liveObjects[messageOid]);
    if (message == nullptr || message->state() != Message::State::Processing)
    {   //  Server must fetch Message for processing before it can complete thyat Message
        return KErrno::InvalidParameter;
    }

    Server * server = message->_server;
    if (process != server->serverProcess())
    {   //  Only a process where message was posted to can complete it
        return KErrno::InvalidParameter;
    }

    message->_state = Message::State::Processed;
    message->_result = messageResult;
    message->_outputs = messageOutputs;
    message->_completionCount.release();
    return KErrno::OK;
}

KErrno Kernel::SystemCalls::releaseMessage(Thread * thread, Oid messageOid)
{
    QMutexLocker lock(_kernel);

    if (thread == nullptr || thread->kernel() != _kernel || !thread->live())
    {
        return KErrno::InvalidParameter;
    }
    Kernel * kernel = thread->kernel();
    Process * process = thread->process();

    if (!kernel->_liveObjects.contains(messageOid))
    {
        return KErrno::InvalidParameter;
    }
    Message * message = dynamic_cast<Message*>(kernel->_liveObjects[messageOid]);
    if (message == nullptr || message->senderProcess() != process)
    {   //  Process must createMessage before it can release that Message
        return KErrno::InvalidParameter;
    }
    if (message->_state == Message::State::Completed)
    {   //  Nothing to do
        return KErrno::InvalidParameter;
    }

    if (message->_state == Message::State::Posted)
    {   //  Must remove the Message from the Server's message queue
        Server * server = message->_server;
        Q_ASSERT(server != nullptr && server->_messageQueue.contains(message));
        server->_messageQueue.removeOne(message);
        message->decrementReferenceCount(); //  we've just dropped a reference to "message"
        server->_messageQueueSize.acquire(1);
    }

    message->_state = Message::State::Completed;
    Q_ASSERT(process->_createdMessages.contains(message));
    process->_createdMessages.remove(message);
    return KErrno::OK;
}

//////////
//  Operations (miscellaneous)
QVersionNumber Kernel::SystemCalls::getSystemVersion()
{
    QMutexLocker lock(_kernel);
    return QVersionNumber(1, 0, 0);
}

KErrno Kernel::SystemCalls::closeHandle(Thread * thread, Handle & handle)
{
    QMutexLocker lock(&_kernel->_runtimeStateGuard);

    if (thread == nullptr || thread->kernel() != _kernel || !thread->live())
    {
        return KErrno::InvalidParameter;
    }
    Process * process = thread->process();

    return process->closeHandle(handle);
}

//  End of hadesvm-kernel/Kernel.SystemCalls.cpp
