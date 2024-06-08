//
//  hadesvm-kernel/NativeThread.SystemCalls.cpp
//
//  hadesvm::kernel::NativeThread::SystemCalls class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

#define HANDLE_TERMINATION_REQUEST()                        \
    if (_nativeThread->_terminationRequested.load())        \
    {                                                       \
        throw Thread::ExitCode::Terminated;                 \
    }

//////////
//  Construction/destruction
NativeThread::SystemCalls::SystemCalls(NativeThread * nativeThread)
    :   _nativeThread(nativeThread)
{
    Q_ASSERT(_nativeThread != nullptr);
}

NativeThread::SystemCalls::~SystemCalls()
{
}

//////////
//  Operations (atoms)
KErrno NativeThread::SystemCalls::getAtom(const QString & name, Oid & atomOid)
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.getAtom(_nativeThread, name, atomOid);
    HANDLE_TERMINATION_REQUEST();
    return result;
}

KErrno NativeThread::SystemCalls::releaseAtom(Oid atomOid)
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.releaseAtom(_nativeThread, atomOid);
    HANDLE_TERMINATION_REQUEST();
    return result;
}

KErrno NativeThread::SystemCalls::getAtomName(Oid atomOid, QString & name)
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.getAtomName(atomOid, name);
    HANDLE_TERMINATION_REQUEST();
    return result;
}

//////////
//  Operations (services and servlets)
KErrno NativeThread::SystemCalls::createService(const QString & name, unsigned int version,
                                                unsigned int maxParameters, unsigned int backlog,
                                                Handle & handle)
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.createService(_nativeThread, name, version, maxParameters, backlog, handle);
    HANDLE_TERMINATION_REQUEST();
    return result;
}

KErrno NativeThread::SystemCalls::openService(const QString & name, unsigned int version,
                                              Handle & handle)
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.openService(_nativeThread, name, version, handle);
    HANDLE_TERMINATION_REQUEST();
    return result;
}

//////////
//  Operations (messages)
KErrno NativeThread::SystemCalls::createMessage(Oid messageTypeAtomOid,
                                                Oid & messageOid)
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.createMessage(_nativeThread, messageTypeAtomOid, messageOid);
    HANDLE_TERMINATION_REQUEST();
    return result;
}

KErrno NativeThread::SystemCalls::createMessage(Oid messageTypeAtomOid,
                                                const Message::Parameter & param0,
                                                Oid & messageOid)
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.createMessage(_nativeThread, messageTypeAtomOid, param0, messageOid);
    HANDLE_TERMINATION_REQUEST();
    return result;
}

KErrno NativeThread::SystemCalls::createMessage(Oid messageTypeAtomOid,
                                                const Message::Parameter & param0,
                                                const Message::Parameter & param1,
                                                Oid & messageOid)
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.createMessage(_nativeThread, messageTypeAtomOid, param0, param1, messageOid);
    HANDLE_TERMINATION_REQUEST();
    return result;
}

KErrno NativeThread::SystemCalls::createMessage(Oid messageTypeAtomOid,
                                                const Message::Parameter & param0,
                                                const Message::Parameter & param1,
                                                const Message::Parameter & param2,
                                                Oid & messageOid)
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.createMessage(_nativeThread, messageTypeAtomOid, param0, param1, param2, messageOid);
    HANDLE_TERMINATION_REQUEST();
    return result;
}

KErrno NativeThread::SystemCalls::createMessage(Oid messageTypeAtomOid,
                                                const Message::Parameter & param0,
                                                const Message::Parameter & param1,
                                                const Message::Parameter & param2,
                                                const Message::Parameter & param3,
                                                Oid & messageOid)
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.createMessage(_nativeThread, messageTypeAtomOid, param0, param1, param2, param3, messageOid);
    HANDLE_TERMINATION_REQUEST();
    return result;
}

KErrno NativeThread::SystemCalls::postMessage(Handle handle, Oid messageOid)
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.postMessage(_nativeThread, handle, messageOid);
    HANDLE_TERMINATION_REQUEST();
    return result;
}

KErrno NativeThread::SystemCalls::getMessage(Handle handle, uint32_t timeoutMs,
                                             Oid & messageOid,
                                             Oid & senderProcessOid, Handle & senderProcessHandle,
                                             Oid & messageTypeAtomOid,
                                             QList<Message::Parameter> & params)
{
    const uint32_t WaitChunkMs = 1000;

    HANDLE_TERMINATION_REQUEST();

    //  Prepare to wait
    Process * process;
    {
        QMutexLocker lock(_nativeThread->_kernel);
        process = _nativeThread->process();
        process->incrementReferenceCount(); //  we'll be keeping the reference to "process" for now
    }

    for (; ; )
    {
        try
        {
            HANDLE_TERMINATION_REQUEST();
        }
        catch (...)
        {
            QMutexLocker lock(_nativeThread->_kernel);
            process->decrementReferenceCount(); //  we've incremenented the ref counter before entering the loop
            throw;
        }

        Server * server;
        {
            QMutexLocker lock(_nativeThread->_kernel);
            //  Is the Process still "live" ?
            if (!process->live())
            {   //  No - release reference and give up
                process->decrementReferenceCount();
                return KErrno::ServerDead;
            }
            server = process->serverForHandle(handle);
            if (server == nullptr)
            {   //  OOPS!  Invalid handle (or not a handle to a Server
                process->decrementReferenceCount();
                return KErrno::InvalidParameter;
            }
            server->incrementReferenceCount();  //  for now
        }
        //  TODO set thread's state to Waiting for the duration of the wait

        //  Wait for a Message to become available
        uint32_t waitChunkMs = qMin(timeoutMs, WaitChunkMs);
        if (server->_messageQueueSize.tryAcquire(1, waitChunkMs))
        {   //  There IS a message in the queue!
            QMutexLocker lock(_nativeThread->_kernel);

            server->decrementReferenceCount();
            process->decrementReferenceCount();
            Q_ASSERT(!server->_messageQueue.isEmpty());
            Message * message = server->_messageQueue.dequeue();
            message->decrementReferenceCount(); //  we've just dropped reference to "message" from "server->_messageQueue"
            message->_state = Message::State::Processing;

            messageOid = message->oid();
            senderProcessOid = message->_senderProcess->oid();
            senderProcessHandle = message->senderHandle();
            messageTypeAtomOid = message->messageTypeAtomOid();
            params = message->parameters();

            return KErrno::OK;
        }
        else
        {   //  No message - must drop the server ref made before we started waiting
            QMutexLocker lock(_nativeThread->_kernel);
            server->decrementReferenceCount();
        }

        //  Keep waiting
        if (timeoutMs != InfiniteTimeout)
        {
            timeoutMs -= waitChunkMs;
        }
        if (timeoutMs == 0)
        {   //  OOPS! Give up.
            QMutexLocker lock(_nativeThread->_kernel);
            process->decrementReferenceCount(); //  we've incremenented the ref counter before entering the loop
            return KErrno::Timeout;
        }
    }
}

KErrno NativeThread::SystemCalls::waitForMessageCompletion(Oid messageOid, uint32_t timeoutMs,
                                KErrno & messageResult,
                                QList<Message::Parameter> & messageOutputs)
{
    const uint32_t WaitChunkMs = 1000;

    HANDLE_TERMINATION_REQUEST();

    //  Prepare to wait
    Kernel * kernel;
    Message * message;
    {
        QMutexLocker lock(_nativeThread->_kernel);
        kernel = _nativeThread->kernel();
        if (!kernel->_liveObjects.contains(messageOid))
        {
            return KErrno::InvalidParameter;
        }
        message = dynamic_cast<Message*>(kernel->_liveObjects[messageOid]);
        if (message == nullptr)
        {
            return KErrno::InvalidParameter;
        }
        message->incrementReferenceCount(); //  we'll be keeping the reference to "message" for now
    }
    //  TODO set thread's state to Waiting for the duration of the wait

    for (; ; )
    {
        try
        {
            HANDLE_TERMINATION_REQUEST();
        }
        catch (...)
        {
            QMutexLocker lock(_nativeThread->_kernel);
            message->decrementReferenceCount(); //  we've incremenented the ref counter before entering the loop
            throw;
        }

        uint32_t waitChunkMs = qMin(timeoutMs, WaitChunkMs);
        if (message->_completionCount.tryAcquire(1, waitChunkMs))
        {   //  Message is Processed...
            QMutexLocker lock(_nativeThread->_kernel);
            //  ...so keep its "processed" status...
            Q_ASSERT(message->state() == Message::State::Processed);
            message->_completionCount.release();    //  go back to 1 for the semaphore
            //  ...store results...
            messageResult = message->result(),
                messageOutputs = message->outputs();
            //  ...and we're done
            message->decrementReferenceCount(); //  we've incremenented the ref counter before entering the loop
            return KErrno::OK;
        }
        //  ...else just keep waiting

        //  Keep waiting
        if (timeoutMs != InfiniteTimeout)
        {
            timeoutMs -= waitChunkMs;
        }
        if (timeoutMs == 0)
        {   //  OOPS! Give up.
            QMutexLocker lock(_nativeThread->_kernel);
            message->decrementReferenceCount(); //  we've incremenented the ref counter before entering the loop
            return KErrno::Timeout;
        }
    }
}

//////////
//  Operations (miscellaneous)
QVersionNumber NativeThread::SystemCalls::getSystemVersion()
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.getSystemVersion();
    HANDLE_TERMINATION_REQUEST();
    return result;
}

KErrno NativeThread::SystemCalls::closeHandle(Handle handle)
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.closeHandle(_nativeThread, handle);
    HANDLE_TERMINATION_REQUEST();
    return result;
}

//  End of hadesvm-kernel/NativeThread.SystemCalls.cpp
