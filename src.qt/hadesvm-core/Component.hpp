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
            //  Types
        public:
            enum class State
            {
                //  Component has been constructed, but not yet connected to
                //  otther components, has no rintime state and is not running.
                Constructed,

                //  Component has been constructed and connected to other
                //  components of the VA, but has no runtime state and is not running.
                Connected,

                //  Component has been constructed, connected to other components
                //  of the VA and has a runtime state, but is not running.
                Initialized,

                //  Component has been constructed, connected to other components
                //  of the VA, has a runtime state and is running.
                Running
            };

            //////////
            //  Construction/destruction
        public:
            Component();
            virtual ~Component();

            //////////
            //  Operations (general)
        public:
            //  The type of this component
            virtual ComponentType * type() const = 0;

            //  Checks if this component can be "suspended".
            bool                    suspendable() const { return type()->suspendable(); }

            //  The VM to which this component is bound; nullptr if this
            //  is a free component
            VirtualAppliance *      virtualAppliance() const { return _virtualAppliance; }

            //  The short descriptive name of this component
            virtual QString         displayName() const = 0;

            //  Serialises the current configuration of this component
            //  to an XML element by setting attributes of that XML element.
            //  IMPORTANT: The "Type" attribute must not be touched!
            virtual void            serialiseConfiguration(QDomElement componentElement) = 0;

            //  De-serialises the current configuration of this component
            //  from an XML element by analysing attributes of that XML element.
            //  IMPORTANT: The "Type" attribute must not be touched!
            virtual void            deserialiseConfiguration(QDomElement componentElement) = 0;

            //  Creates a new editor widget for viewing and editing the
            //  configuration of this component.
            //  Returns nullptr if this component has no configuration
            //  to view/modify in an editor.
            virtual ComponentEditor*createEditor(QWidget * parent) = 0;

            //////////
            //  Operations (state management)
        public:
            //  The current state of this component
            virtual State           state() const noexcept = 0;

            //  Performs the Constructed -> Connected state transition.
            //  This connects Component to other components of the same VA
            //  with which the Component is expected to collaborate.
            //  Must only be called from the QApplication's main thread
            virtual void            connect() throws(VirtualApplianceException) = 0;

            //  Performs the Connected -> Initialized state transition.
            //  This allocated the "runtime state" of the Component.
            //  Must only be called from the QApplication's main thread
            virtual void            initialize() throws(VirtualApplianceException) = 0;

            //  Performs the Initialized -> Running state transition.
            //  This starts the Component.
            //  Must only be called from the QApplication's main thread
            virtual void            start() throws(VirtualApplianceException) = 0;

            //  Performs the Running -> Initialized state transition.
            //  This stops the Component.
            //  Must only be called from the QApplication's main thread
            virtual void            stop() noexcept = 0;

            //  Performs the Initialized -> Connected state transition.
            //  This drops the "runtime state" of the Component.
            //  Must only be called from the QApplication's main thread
            virtual void            deinitialize() noexcept = 0;

            //  Performs the Connected -> Constructed state transition.
            //  This disconnects Component from other components of the same VA.
            //  Must only be called from the QApplication's main thread
            virtual void            disconnect() noexcept = 0;

            //////////
            //  Implementation
        private:
            VirtualAppliance *      _virtualAppliance = nullptr;  //  nullptr == free, else bound
        };
    }
}

//  End of hadesvm-core/Component.hpp
