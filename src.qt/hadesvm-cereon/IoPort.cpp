//
//  hadesvm-cereon/IoPort.cpp
//
//  hadesvm::cereon::IIoPort class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Operations
void IIoPort::enableInterrupts()
{
    if (!_interruptsEnabled)
    {   //  Enable...
        _interruptsEnabled = true;
        //  ...and inform I/O but that this I/O interrupt is now "ready to handle"
        if (_ioBus != nullptr && _pendingIoInterrupt != nullptr)
        {
            QMutexLocker lock(&_ioBus->_accessGuard);
            Q_ASSERT(!_ioBus->_interruptsReadyToHandle.contains(_pendingIoInterrupt));
            _ioBus->_interruptsReadyToHandle.enqueue(_pendingIoInterrupt);
        }
    }
}

void IIoPort::disableInterrupts()
{
    if (_interruptsEnabled)
    {   //  Disable...
        _interruptsEnabled = false;
        //  ...and inform the I/O bus that the pending interrupt is no longer "ready to handle"
        if (_ioBus != nullptr && _pendingIoInterrupt != nullptr)
        {
            QMutexLocker lock(&_ioBus->_accessGuard);
            Q_ASSERT(_ioBus->_interruptsReadyToHandle.contains(_pendingIoInterrupt));
            _ioBus->_interruptsReadyToHandle.removeOne(_pendingIoInterrupt);
        }
    }
}

void IIoPort::setPendingIoInterrupt(uint16_t interruptStatusCode)
{
    if (_pendingIoInterrupt != nullptr)
    {   //  Delete the old one...
        delete releasePendingIoInterrupt();
    }
    //  Create new I/O interrupt pending in this port
    _pendingIoInterrupt = new IoInterrupt(address(), interruptStatusCode);
    Q_ASSERT(_pendingIoInterrupt->_ioPort == nullptr);
    _pendingIoInterrupt->_ioPort = this;    //  ...because it's pending here
    //  Inform the I/O bus that the new interrupt is "ready to handle"
    if (_ioBus != nullptr && _interruptsEnabled)
    {
        QMutexLocker lock(&_ioBus->_accessGuard);
        _ioBus->_interruptsReadyToHandle.enqueue(_pendingIoInterrupt);
    }
}

IoInterrupt * IIoPort::releasePendingIoInterrupt()
{
    IoInterrupt * result = _pendingIoInterrupt;
    _pendingIoInterrupt = nullptr;
    if (result != nullptr)
    {   //  The I/O interrupt is now "standalone"...
        Q_ASSERT(result->_ioPort == this);
        result->_ioPort = nullptr;
        //  ...and inform I/O bus that it is no longer "ready to handle"
        if (_ioBus != nullptr && _interruptsEnabled)
        {   //  ...informing I/O bus that it is no longer "ready to handle"
            QMutexLocker lock(&_ioBus->_accessGuard);
            Q_ASSERT(_ioBus->_interruptsReadyToHandle.contains(result));
            _ioBus->_interruptsReadyToHandle.removeOne(result);
        }
    }
    return result;
}

const IoInterrupt * IIoPort::pendingIoInterrupt() const
{
    return _pendingIoInterrupt;
}

//  End of hadesvm-cereon/IoPort.cpp
