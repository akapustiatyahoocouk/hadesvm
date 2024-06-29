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
        //  An abstract interface to a generic VM component.
        //
        //  A component can be in one of two states:
        //  *   A free component, which is not a part of any VM, or
        //  *   A bound component, which is assigned to a spefic VM.
        //  All components are created as free components which can then
        //  be assigned to VMs. Bound components can ONLY be destroyed
        //  when the VM they are bound to is destroyed.
        class HADESVM_CORE_PUBLIC IComponent
        {
            //////////
            //  Types
        public:
            //  Component states
            enum class State
            {
                //  Component has been constructed, but not yet connected to
                //  other components, has no rintime state and is not running.
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
            //  Component UI
            class HADESVM_CORE_PUBLIC Ui
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Ui)

                //////////
                //  Construction/destruction
            public:
                Ui() = default;
                virtual ~Ui() = default;

                //////////
                //  Operations
            public:
                //  Returns the Component to which this UI belongs
                virtual IComponent *    component() const = 0;

                //  Returns the unordered list of all DisplayWidgets.
                //  These initially have no "parent" and cannot be directly "delete"'d
                //  except if the entire Ui instance is deleted.
                virtual DisplayWidgetList   displayWidgets() const = 0;
            };

            //////////
            //  This is an interface
        public:
            virtual ~IComponent() = default;

            //////////
            //  Operations (general)
        public:
            //  The type of this component
            virtual ComponentType * componentType() const = 0;

            //  Checks if this component can be "suspended".
            virtual bool            suspendable() const = 0;

            //  The VM to which this component is bound; nullptr if this
            //  is a free component
            virtual VirtualAppliance *  virtualAppliance() const = 0;

            //  The short descriptive name of this component
            virtual QString         displayName() const = 0;

            //  Serialises the current configuration of this component
            //  to an XML element by setting attributes of that XML element.
            //  IMPORTANT: The "Type" attribute must not be touched!
            virtual void            serialiseConfiguration(QDomElement componentElement) const = 0;

            //  De-serialises the current configuration of this component
            //  from an XML element by analysing attributes of that XML element.
            //  IMPORTANT: The "Type" attribute must not be touched!
            virtual void            deserialiseConfiguration(QDomElement componentElement) = 0;

            //  Creates a new editor widget for viewing and editing the
            //  configuration of this component.
            //  Returns nullptr if this component has no configuration
            //  to view/modify in an editor. Otherwise the caller is responsible
            //  for eventually deleting the returned ComponentEditor.
            virtual ComponentEditor*createEditor(QWidget * parent) = 0;

            //  Creates a new UI for this Component.
            //  Returns nullptr if this component has no UI. Otherwise the
            //  caller is responsible for eventually deleting the returned UI.
            //  Widgets that form the UI are created as parent-less and will
            //  normally be re-parented by the called to fit into the caller's UI.
            virtual Ui *            createUi() = 0;

            //////////
            //  Operations (state management)
        public:
            //  The current state of this component
            virtual State       state() const noexcept = 0;

            //  Performs the Constructed -> Connected state transition.
            //  This connects Component to other components of the same VA
            //  with which the Component is expected to collaborate.
            //  Must only be called from the QApplication's main thread
            virtual void        connect() throws(VirtualApplianceException) = 0;

            //  Performs the Connected -> Initialized state transition.
            //  This allocated the "runtime state" of the Component.
            //  Must only be called from the QApplication's main thread
            virtual void        initialize() throws(VirtualApplianceException) = 0;

            //  Performs the Initialized -> Running state transition.
            //  This starts the Component.
            //  Must only be called from the QApplication's main thread
            virtual void        start() throws(VirtualApplianceException) = 0;

            //  Performs the Running -> Initialized state transition.
            //  This stops the Component.
            //  Must only be called from the QApplication's main thread
            virtual void        stop() noexcept = 0;

            //  Performs the Initialized -> Connected state transition.
            //  This drops the "runtime state" of the Component.
            //  Must only be called from the QApplication's main thread
            virtual void        deinitialize() noexcept = 0;

            //  Performs the Connected -> Constructed state transition.
            //  This disconnects Component from other components of the same VA.
            //  Must only be called from the QApplication's main thread
            virtual void        disconnect() noexcept = 0;
        };

        //////////
        //  A standalone component (i.e. not wrapped by an adaptor)
        class HADESVM_CORE_PUBLIC Component : public virtual IComponent
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Component)

            friend class VirtualAppliance;

            //////////
            //  Construction/destruction
        public:
            Component();
            virtual ~Component();

            //////////
            //  IComponet (general)
        public:
            virtual bool            suspendable() const { return componentType()->suspendable(); }
            virtual VirtualAppliance *virtualAppliance() const override { return _virtualAppliance; }

            //////////
            //  Implementation
        private:
            VirtualAppliance *  _virtualAppliance = nullptr;  //  nullptr == free, else bound
        };

        //////////
        //  Represents a generic VM component adaptor.
        //
        //  All component adaptors are created AUTOMATICALLY as bound component
        //  adaptors as components requiring adaption are added to VAs. Bound
        //  component adaptors can ONLY be destroyed when the VM they are bound
        //  to is destroyed.
        class HADESVM_CORE_PUBLIC ComponentAdaptor : public virtual IComponent
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ComponentAdaptor)

            friend class VirtualAppliance;

            //////////
            //  Construction/destruction
        public:
            explicit ComponentAdaptor(Component * adaptedComponent);
            virtual ~ComponentAdaptor();

            //////////
            //  IComponet (general)
        public:
            virtual VirtualAppliance *virtualAppliance() const override { return _virtualAppliance; }

            //////////
            //  Operations (general)
        public:
            //  The type of this component adaptor
            virtual ComponentAdaptorType *  componentAdaptorType() const = 0;

            //  The component being adapted.
            Component *         adaptedComponent() const { return _adaptedComponent; }

            //////////
            //  Implementation
        private:
            Component *const    _adaptedComponent;
            VirtualAppliance *  _virtualAppliance = nullptr;  //  nullptr == free, else bound
        };

        //////////
        //  A "clocked component (or adapter)" is driven by a periodic
        //  "clock" of some fixed frequency.
        class HADESVM_CORE_PUBLIC IClockedComponent : public virtual IComponent
        {
            //////////
            //  Operations
        public:
            //  Returns the configured clock frequency of the clock driving the component
            virtual ClockFrequency  clockFrequency() const noexcept = 0;

            //  Called on each clock tick when the VM containing this component runs
            virtual void            onClockTick() noexcept = 0;
        };

        //////////
        //  An "active component (or adapter)" has its own thread of control
        //  (normally created/started by IComponent's "start()" method and
        //  stopped/destroyed by IComponent's "stop()" method).
        //  NOTE that an "active component" is NOT necessarily always a "clocked
        //  component". If it is, then the internal worker thread of the "active
        //  component" is expected to call that component's onClockTick() method
        //  at the intervals required by the component's clockFrequency().
        class HADESVM_CORE_PUBLIC IActiveComponent : public virtual IComponent
        {
        };
    }
}

//  End of hadesvm-core/Component.hpp
