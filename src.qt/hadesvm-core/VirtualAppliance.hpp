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
            //  Operations (general)
        public:
            //  The type of this VA.
            virtual VirtualApplianceType *  type() const = 0;

            //  Saves the current configuration of this VA to its "location"
            //  Throws if a save error occurs.
            void                    save() throws(VirtualApplianceException);

            //  Loads a VA from the specified "location".
            //  Throws if a load error occurs.
            static VirtualAppliance *   load(const QString & location);

            //////////
            //  Operations (configuration)
        public:
            //  TODO document
            //  Must only be called from the QApplication's main thread
            static bool             isValidName(const QString & name);
            static bool             isValidLocation(const QString & location);
            QString                 name() const;
            void                    setName(const QString & name);
            QString                 location() const;
            QString                 directory() const;
            VirtualArchitecture *   architecture() const;
            bool                    startAutomatically() const;
            void                    setStartAutomatically(bool startAutomatically);
            bool                    startFullScreen() const;
            void                    setStartFullScreen(bool startFullScreen);
            bool                    startMinimized() const;
            void                    setStartMinimized(bool startMinimized);

            //  TODO document
            //  Must only be called from the QApplication's main thread
            QString                 toRelativePath(const QString & path);
            QString                 toAbsolutePath(const QString & path);

            //////////
            //  Operations (VA components)
        public:
            //  TODO document
            //  Must only be called from the QApplication's main thread
            ComponentList           compatibleComponents() const;
            ComponentList           adaptedComponents() const;

            //  An unordered list of all components of this VM, compatible and adapted.
            //  Must only be called from the QApplication's main thread
            ComponentList           components() const;

            //  TODO document
            ComponentAdaptorList    componentAdaptors() const;

            //  Adds the specified component to this VM.
            //  Throws if the component cannot be added to the VM (e.g. it is
            //  incompatible with this VM, etc.)
            //  Must only be called from the QApplication's main thread
            void                    addComponent(Component * component) throws(VirtualApplianceException);

            //  Removes the specified component from thid VM.
            //  Has no effect if the component is not part of this VM.
            //  Must only be called from the QApplication's main thread
            void                    removeComponent(Component * component);

            //  Returns an unordered list of all components and component
            //  adaptors that implement the specified interface
            //  Must only be called from the QApplication's main thread
            template <class I>
            QList<I*>               componentsImplementing()
            {
                Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

                QList<I*> result;
                for (Component * component : _compatibleComponents)
                {
                    if (I * i = dynamic_cast<I*>(component))
                    {
                        result.append(i);
                    }
                }
                for (Component * component : _adaptedComponents)
                {
                    if (I * i = dynamic_cast<I*>(component))
                    {
                        result.append(i);
                    }
                }
                for (ComponentAdaptor * componentAdaptor : _componentAdaptors)
                {
                    if (I * i = dynamic_cast<I*>(componentAdaptor))
                    {
                        result.append(i);
                    }
                }
                return result;
            }

            //////////
            //  Operations (state management)
        public:
            //  The current state of this VM; can be called from any thread
            State                   state() const;

            //  "Tests" a configuration of a Stopped VA by connecting and
            //  initializing all its components temporarily.
            //  Must only be called from the QApplication's main thread
            virtual void            testConfiguration() throws(VirtualApplianceException);

            //  Starts the VA if Stopped, otherwise has no effect.
            //  Must only be called from the QApplication's main thread
            void                    start() throws(VirtualApplianceException);

            //  Stops the VA if Running or Suspended, otherwise has no effect.
            //  Must only be called from the QApplication's main thread
            void                    stop() noexcept;

            //  Suspends the VA if Running, otherwise has no effect.
            //  Must only be called from the QApplication's main thread
            void                    suspend() throws(VirtualApplianceException);

            //  Resumes the VA if Suspended, otherwise has no effect.
            //  Must only be called from the QApplication's main thread
            void                    resume() throws(VirtualApplianceException);

            //  Performs a live reset of a Running VA; has no effect if the
            //  VA is not in the Running state.
            void                    reset();

            //////////
            //  Operations (runtime state)
        public:
            //  True iff "stop"/"reset" has been requested for this VA.
            bool                    stopRequested() const { return _stopRequested; }
            bool                    resetRequested() const { return _resetRequested; }

            //  Requests this VA to "stop"/"reset" as soon as practicable.
            //  Can be called from any thread, including one of the VA's
            //  own worker threads. This allows a running VA to schedule
            //  its own termination.
            void                    requestStop();
            void                    requestReset();

            //////////
            //  Operations (runtime statistics)
        public:
            //  TODO document
            void                    recordAchievedClockFrequency(IClockedComponent * component, const ClockFrequency & clockFrequency);

            void                    getRuntimeStatistics(QMap<IClockedComponent*, ClockFrequency> & achievedClockFrequencyByClockedComponent);

            //////////
            //  Implementation
        private:
            State                   _state;

            QString                 _name;
            const QString           _location;  //  full path to VM configuration file
            const QString           _directory; //  full path to VM configuration file's containg dir
            VirtualArchitecture *const  _architecture;

            bool                    _startAutomatically;
            bool                    _startFullScreen;
            bool                    _startMinimized;

            //  VA components
            ComponentList           _compatibleComponents;  //  ...bound to this VM
            ComponentList           _adaptedComponents;     //  ...bound to this VM
            ComponentAdaptorList    _componentAdaptors;     //  ...bound to this VM

            //  Runtime state
            std::atomic<bool>       _stopRequested;
            std::atomic<bool>       _resetRequested;

            //  Runtime statistics
            hadesvm::util::Spinlock _statisticsGuard;
            QMap<IClockedComponent*, ClockFrequency>  _achievedClockFrequencyByClockedComponent;

            //  Helpers
            void                    _connectComponents() throws(VirtualApplianceException);
            void                    _initializeComponents() throws(VirtualApplianceException);
            void                    _startComponents() throws(VirtualApplianceException);
            void                    _stopComponents();
            void                    _deinitializeComponents();
            void                    _disconnectComponents();

            void                    _resetStatistics();

            //  Threads
            class _FrequencyDivider final : public virtual IClockedComponent
            {   //  Accepts "input" ticks, performing "output" ticks at a lower frequency
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_FrequencyDivider)

                //////////
                //  Construction/destruction
            public:
                _FrequencyDivider(unsigned inputTicks, unsigned outputTicks, IClockedComponent * drivenComponent);

                //////////
                //  IComponent (general; largely unused)
            public:
                virtual ComponentType * componentType() const override { return _drivenComponent->componentType(); }
                virtual VirtualAppliance *  virtualAppliance() const override { return _drivenComponent->virtualAppliance(); }
                virtual QString         displayName() const override { return _drivenComponent->displayName(); }
                virtual void            serialiseConfiguration(QDomElement /*componentElement*/) const override {}
                virtual void            deserialiseConfiguration(QDomElement /*componentElement*/) override {}
                virtual ComponentEditor*createEditor(QWidget * /*parent*/) override { return nullptr; }
                virtual Ui *            createUi() override { return nullptr; }

                //////////
                //  IComponent (state management)
            public:
                virtual State           state() const noexcept override { return _drivenComponent->state(); }
                virtual void            connect() throws(VirtualApplianceException) override {}
                virtual void            initialize() throws(VirtualApplianceException) override {}
                virtual void            start() throws(VirtualApplianceException) override {}
                virtual void            stop() noexcept override {}
                virtual void            deinitialize() noexcept override {}
                virtual void            disconnect() noexcept override {}

                //////////
                //  IClockedComponent
            public:
                virtual ClockFrequency  clockFrequency() const noexcept override;
                virtual void            onClockTick() noexcept override;

                //////////
                //  Implementation
            private:
                const unsigned      _inputTicks;
                const unsigned      _outputTicks;
                IClockedComponent *const    _drivenComponent;
                //  Worker data for Bresenham's line algorithm
                const unsigned      _dx, _dy;
                const unsigned      _2dx, _2dy;
                int                 _d;
                unsigned            _x, _y;
            };

            class _WorkerThread final : public QThread
            {   //  Drives all "clocked components" that are not themselves "active components"
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_WorkerThread)

                //////////
                //  Construction/destruction
            public:
                explicit _WorkerThread(VirtualAppliance * virtualAppliance);
                virtual ~_WorkerThread();

                //////////
                //  QThread
            protected:
                virtual void    run() override;

                //////////
                //  Operations
            public:
                void            requestStop() { _stopRequested = true; }

                //////////
                //  Implementation
            private:
                VirtualAppliance *const _virtualAppliance;
                std::atomic<bool>   _stopRequested;

                ClockFrequency      _maxClockFrequency;
                QList<_FrequencyDivider*>   _frequencyDividers;
                QList<IClockedComponent*>   _tickTargets;
            };
            _WorkerThread *     _workerThread = nullptr;
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
