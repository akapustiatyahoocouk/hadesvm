//
//  hadesvm-core/ComponentAspect.hpp
//
//  Component Aspect support
//
//////////

namespace hadesvm
{
    namespace core
    {
        //////////
        //  An "aspect" of an element is a feature or a set of features that this
        //  element implements. Individual aspects will be declared as interfaces
        //  that extend (i.e. virtually & publicly inherit from) the root Aspect
        //  interface. An element that wants to implement one or more of these
        //  Aspects will implement each of the required Aspect sub-interfaces
        //  (again, by virtually & publicly inheriting from it).
        class HADESVM_CORE_PUBLIC IComponentAspect
        {
            //////////
            //  This is an interface
        public:
            virtual ~IComponentAspect() noexcept = default;
        };

        //////////
        //  A "clocked" aspect means that a component (or adapter)
        //  is driven by a periodic "clock" of some fixed frequency.
        class HADESVM_CORE_PUBLIC IClockedComponentAspect : public virtual IComponentAspect
        {
            //////////
            //  Operations
        public:
            //  Returns the configured clock frequency of the clock driving the component
            virtual ClockFrequency  clockFrequency() const = 0;

            //  Called on each clock tick when the VM containing this component runs
            virtual void            onClockTick() = 0;
        };

        //////////
        //  An "clocked" aspect means that a component (or adapter)
        //  is has its own thread of control (normally created/started
        //  by Component's "start()" method and stopped/destroyed by
        //  Component's "stop()" method).
        class HADESVM_CORE_PUBLIC IActiveComponentAspect : public virtual IComponentAspect
        {
        };
    }
}

//  End of hadesvm-core/ComponentAspect.hpp
