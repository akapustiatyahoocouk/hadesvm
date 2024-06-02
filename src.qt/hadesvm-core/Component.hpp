//
//  hadesvm-core/Component.hpp
//
//  hadesvm-core component support
//
//////////

namespace hadesvm
{
    namespace core
    {
        //////////
        //  Represents a generic VM component.
        //
        //  A component can be in one of two states:
        //  *   A free component, which is not a part of any VM, or
        //  *   A bound component, which is assigned to a spefic VM.
        //  All components are created as free components which can then
        //  be assigned to VMs. Bound components can ONLY be destroyed
        //  when the VM they are bound to is destroyed.
        class HADESVM_CORE_PUBLIC Component
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Component)

            friend class VirtualAppliance;

            //////////
            //  Construction/destruction
        public:
            Component();
            virtual ~Component();

            //////////
            //  Operations
        public:
            //  The type of this component
            virtual ComponentType *     type() const = 0;

            //  Checks if this component can be "suspended".
            bool                suspendable() const { return type()->suspendable(); }

            //  The VM to which this component is bound; nullptr if this
            //  is a free component
            VirtualAppliance *  virtualAppliance() const { return _virtualAppliance; }

            //////////
            //  Implementation
        private:
            VirtualAppliance *  _virtualAppliance = nullptr;  //  nullptr == free, else bound
        };
    }
}

//  End of hadesvm-core/Component.hpp
