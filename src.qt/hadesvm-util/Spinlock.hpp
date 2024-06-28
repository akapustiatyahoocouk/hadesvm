//
//  hadesvm-util/Spinlock.hpp
//
//  Spinlocks for very fast thread synchronization
//
//////////

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
#else
    #error Unsupported OS
#endif

            //////////
            //  Implementation
        private:
            Value       _value = Value::Unlocked;
        };
    }
}

//  End of hadesvm-util/Spinlock.hpp
