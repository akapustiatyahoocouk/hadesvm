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
            //  Types
        public:
            enum class State
            {
                //  Component adaptor has been constructed, but not yet connected to
                //  otther components, has no rintime state and is not running.
                Constructed,

                //  Component adaptor has been constructed and connected to other
                //  components of the VA, but has no runtime state and is not running.
                Connected,

                //  Component adaptor has been constructed, connected to other components
                //  of the VA and has a runtime state, but is not running.
                Initialized,

                //  Component adaptor has been constructed, connected to other components
                //  of the VA, has a runtime state and is running.
                Running
            };

            //////////
            //  Construction/destruction
        public:
            explicit ComponentAdaptor(Component * adaptedComponent);
            virtual ~ComponentAdaptor();

            //////////
            //  Operations (general)
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

            //  Serialises the current configuration of this component adaptor
            //  to an XML element by setting attributes of that XML element.
            //  IMPORTANT: The "Type" attribute must not be touched!
            virtual void        serialiseConfiguration(QDomElement componentElement) = 0;

            //  De-serialises the current configuration of this component adaptor
            //  from an XML element by analysing attributes of that XML element.
            //  IMPORTANT: The "Type" attribute must not be touched!
            virtual void        deserialiseConfiguration(QDomElement componentElement) = 0;

            //////////
            //  Operations (state management)
        public:
            //  The current state of this component adaptor
            virtual State           state() const noexcept = 0;

            //  Performs the Constructed -> Connected state transition.
            //  This connects ComponentAdaptor to other components of the same VA
            //  with which the ComponentAdaptor is expected to collaborate.
            //  Must only be called from the QApplication's main thread
            virtual void            connect() throws(VirtualApplianceException) = 0;

            //  Performs the Connected -> Initialized state transition.
            //  This allocated the "runtime state" of the ComponentAdaptor.
            //  Must only be called from the QApplication's main thread
            virtual void            initialize() throws(VirtualApplianceException) = 0;

            //  Performs the Initialized -> Running state transition.
            //  This starts the ComponentAdaptor.
            //  Must only be called from the QApplication's main thread
            virtual void            start() throws(VirtualApplianceException) = 0;

            //  Performs the Running -> Initialized state transition.
            //  This stops the ComponentAdaptor.
            //  Must only be called from the QApplication's main thread
            virtual void            stop() noexcept = 0;

            //  Performs the Initialized -> Connected state transition.
            //  This drops the "runtime state" of the ComponentAdaptor.
            //  Must only be called from the QApplication's main thread
            virtual void            deinitialize() noexcept = 0;

            //  Performs the Connected -> Constructed state transition.
            //  This disconnects ComponentAdaptor from other components of the same VA.
            //  Must only be called from the QApplication's main thread
            virtual void            disconnect() noexcept = 0;

            //////////
            //  Implementation
        private:
            VirtualAppliance *  _virtualAppliance = nullptr;  //  nullptr == free, else bound
            Component *const    _adaptedComponent;
        };
    }
}

//  End of hadesvm-core/ComponentAdaptor.hpp
