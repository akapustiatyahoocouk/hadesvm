//
//  hadesvm-core/VirtualAppliance.hpp
//
//  hadesvm-core VA support
//
//////////

namespace hadesvm
{
    namespace core
    {
        //////////
        //  A complete VA.
        class HADESVM_CORE_PUBLIC VirtualAppliance
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(VirtualAppliance)

            //////////
            //  Constants
        public:
            static const QString    PreferredExtension; //  starts with '.'

            //////////
            //  Types
        public:
            enum class State
            {
                Stopped,
                Running,
                Suspended
            };

            //////////
            //  Construction/destruction
        public:
            VirtualAppliance(const QString & name, const QString & location,
                             VirtualArchitecture * architecture);
            virtual ~VirtualAppliance();

            //////////
            //  Operations
        public:
            //  The type of this VA.
            virtual VirtualApplianceType *  type() const = 0;

            //  The current state of this VM; can be called from any thread
            State           state() const;

            //  TODO document
            //  Must only be called from the QApplication's main thread
            static bool     isValidName(const QString & name);
            static bool     isValidLocation(const QString & location);
            QString         name() const;
            void            setName(const QString & name);
            QString         location() const;
            QString         directory() const;
            VirtualArchitecture * architecture() const;

            //  TODO document
            ComponentList   compatibleComponents() const;
            ComponentList   adaptedComponents() const;

            //  An unordered list of all components of this VM, compatible and adapted.
            //  Must only be called from the QApplication's main thread
            ComponentList   components() const;

            //  TODO document
            ComponentAdaptorList    componentAdaptors() const;

            //  Adds the specified component to this VM.
            //  Throws if the component cannot be added to the VM (e.g. it is
            //  incompatible with this VM, etc.)
            //  Must only be called from the QApplication's main thread
            void            addComponent(Component * component) throws(VirtualApplianceException);

            //  Removes the specified component from thid VM.
            //  Has no effect if the component is not part of this VM.
            //  Must only be called from the QApplication's main thread
            void            removeComponent(Component * component);

            //  Starts the VA if Stopped, otherwise has no effect.
            //  Must only be called from the QApplication's main thread
            void            start() throws(VirtualApplianceException);

            //  Stops the VA if Running or Suspended, otherwise has no effect.
            //  Must only be called from the QApplication's main thread
            void            stop() noexcept;

            //  TODO document
            //  Must only be called from the QApplication's main thread
            bool            suspendable() const noexcept;

            //  Suspends the VA if Running, otherwise has no effect.
            //  Must only be called from the QApplication's main thread
            void            suspend() throws(VirtualApplianceException);

            //  Resumes the VA if Suspended, otherwise has no effect.
            //  Must only be called from the QApplication's main thread
            void            resume() throws(VirtualApplianceException);

            //  Saves the current configuration of this VA to its "location"
            //  Throws if a save error occurs.
            void            save() throws(VirtualApplianceException);

            //  Loads a VA from the specified "location".
            //  Throws if a load error occurs.
            static VirtualAppliance *   load(const QString & location);

            //////////
            //  Implementation
        private:
            State                   _state = State::Stopped;
            mutable QMutex          _stateGuard;

            QString                 _name;
            const QString           _location;  //  full path to VM configuration file
            const QString           _directory; //  full path to VM configuration file's containg dir
            VirtualArchitecture *const  _architecture;

            ComponentList           _compatibleComponents = {}; //  ...bound to this VM
            ComponentList           _adaptedComponents = {};    //  ...bound to this VM
            ComponentAdaptorList    _componentAdaptors = {};    //  ...bound to this VM
        };

        //////////
        //  A complete VM.
        class HADESVM_CORE_PUBLIC VirtualMachine : public VirtualAppliance
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(VirtualMachine)

            //////////
            //  Construction/destruction
        public:
            VirtualMachine(const QString & name, const QString & location,
                           VirtualArchitecture * architecture);
            virtual ~VirtualMachine();

            //////////
            //  VirtualAppliance
        public:
            virtual VirtualApplianceType *  type() const override;
        };

        //////////
        //  A complete RT.
        class HADESVM_CORE_PUBLIC RemoteTerminal : public VirtualAppliance
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(RemoteTerminal)

            //////////
            //  Construction/destruction
        public:
            RemoteTerminal(const QString & name, const QString & location,
                           VirtualArchitecture * architecture);
            virtual ~RemoteTerminal();

            //////////
            //  VirtualAppliance
        public:
            virtual VirtualApplianceType *  type() const override;
        };
    }
}

//  End of hadesvm-core/VirtualAppliance.hpp
