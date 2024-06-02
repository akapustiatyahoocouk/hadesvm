//
//  hadesvm-core/ComponentAdaptor.hpp
//
//  hadesvm-core component adaptor support
//
//////////

namespace hadesvm
{
    namespace core
    {
        //////////
        //  Represents a generic VM component adaptor.
        //
        //  A component adaptor can be in one of two states:
        //  *   A free component adaptor, which is not a part of any VM, or
        //  *   A bound component adaptor, which is assigned to a spefic VM.
        //  All component adaptors are created AUTOMATICALLY as bound component
        //  adaptors as components requiring adaption are added to VAs. Bound
        //  component adaptors can ONLY be destroyed when the VM they are bound
        //  to is destroyed.
        class HADESVM_CORE_PUBLIC ComponentAdaptor
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ComponentAdaptor)

            friend class VirtualAppliance;

            //////////
            //  Construction/destruction
        public:
            explicit ComponentAdaptor(Component * adaptedComponent);
            virtual ~ComponentAdaptor();

            //////////
            //  Operations
        public:
            //  The type of this component adaptor
            virtual ComponentAdaptorType *  type() const = 0;

            //  Checks if this component adaptor can be "suspended".
            bool                suspendable() const { return type()->suspendable(); }

            //  The VM to which this component adaptor is bound;
            //  nullptr if this is a free component adaptor
            VirtualAppliance *  virtualAppliance() const { return _virtualAppliance; }

            //  The component being adapted.
            Component *         adaptedComponent() const { return _adaptedComponent; }

            //////////
            //  Implementation
        private:
            VirtualAppliance *  _virtualAppliance = nullptr;  //  nullptr == free, else bound
            Component *const    _adaptedComponent;
        };
    }
}

//  End of hadesvm-core/ComponentAdaptor.hpp
