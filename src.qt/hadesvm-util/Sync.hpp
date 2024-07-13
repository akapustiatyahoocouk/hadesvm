//
//  hadesvm-util/Sync.hpp
//
//  Thread synchronization
//
//////////

#if defined(Q_CC_MSVC) && defined(Q_OS_WINDOWS)
    #include <Windows.h>
#endif

namespace hadesvm
{
    namespace util
    {
        //////////
        //  A non-recursive lock for very fast thread synchronization
        class HADESVM_UTIL_PUBLIC Spinlock final
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Spinlock)

            //////////
            //  Types
        public:
#if defined(Q_CC_GNU) && defined(Q_OS_LINUX)
            enum class Value : int
            {
                Unlocked = 0,
                Locked = 1
            };
#elif defined(Q_CC_MSVC) && defined(Q_OS_WINDOWS)
            enum class Value : long
            {
                Unlocked = 0,
                Locked = 1
            };
#else
    #error Unsupported OS
#endif

            //////////
            //  Operations ()
        public:
            Spinlock() = default;
            ~Spinlock() = default;

            //////////
            //  Operations
        public:
#if defined(Q_CC_GNU) && defined(Q_OS_LINUX)
            void                lock()
            {
                for (; ; )
                {
                    //  Try grabbing the spinlock
                    Value oldValue = __sync_lock_test_and_set(&_value, Value::Locked);
                    if (oldValue == Value::Unlocked)
                    {   //  The spinlock was NOT locked before the call
                        return;
                    }
                    //  Failed to grab the spinlock - give some other thread the chance to release it
                    sched_yield();
                }
            }

            void                unlock()
            {
                __sync_lock_release(&_value);
            }
#elif defined(Q_CC_MSVC) && defined(Q_OS_WINDOWS)
            void                lock()
            {
                for (; ; )
                {
                    //  Try grabbing the spinlock
                    Value oldValue = static_cast<Value>(InterlockedExchange(reinterpret_cast<long*>(&_value), static_cast<long>(Value::Locked)));
                    if (oldValue == Value::Unlocked)
                    {   //  The spinlock was NOT locked before the call
                        return;
                    }
                    //  Failed to grab the spinlock - give some other thread the chance to release it
                    Yield();
                }
            }

            void                unlock()
            {
                InterlockedExchange(reinterpret_cast<long*>(&_value), static_cast<long>(Value::Unlocked));
            }
#else
    #error Unsupported OS
#endif

            //////////
            //  Implementation
        private:
            Value       _value = Value::Unlocked;
        };

        //////////
        //  A thread-0safe queue that allows multiple producers and multiple consumers
        template <class T>
        class InterthreadQueue
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(InterthreadQueue)

            //////////
            //  Construction/destruction
        public:
            InterthreadQueue()
                :   _data(), _dataGuard(), _datasReady() {}
            ~InterthreadQueue() = default;

            //////////
            //  Operations
        public:
            //  Add the spoecified value to the end of the queue
            void        enqueue(const T & value);

            //  Stores the value at the head of the queue, removing it from
            //  the queue. If there is no such value, will idle-wait until there is.
            void        dequeue(T & value);

            //  Stores the value at the head of the queue, removing it from
            //  the queue and returning true. If there is no such value, will
            //  idle-wait until there is one, but only for timeoutMs. If within
            //  that timeout there's still no value to dequeue, returns false
            //  without storing anything.
            bool        tryDequeue(int timeoutMs, T & value);

            //////////
            //  Implementation
        private:
            QQueue<T>   _data;
            Spinlock    _dataGuard;
            QSemaphore  _datasReady; //  value == _data.count
        };

        template <class T>
        void InterthreadQueue<T>::enqueue(const T & value)
        {
            QMutexLocker lock(&_dataGuard);
            _data.enqueue(value);
            _datasReady.release();
        }

        template <class T>
        void InterthreadQueue<T>::dequeue(T & value)
        {
            _datasReady.acquire();
            QMutexLocker lock(&_dataGuard);
            value = _data.dequeue();
        }

        template <class T>
        bool InterthreadQueue<T>::tryDequeue(int timeoutMs, T & value)
        {
            if (_datasReady.tryAcquire(1, timeoutMs))
            {
                QMutexLocker lock(&_dataGuard);
                value = _data.dequeue();
                return true;
            }
            return false;
        }
    }
}

//  End of hadesvm-util/Sync.hpp
