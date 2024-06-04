//
//  hadesvm-kernel/Objects.hpp
//
//  hadesvm kernel objects
//
//////////

namespace hadesvm
{
    namespace kernel
    {
        //  An OID of a kernel object - unique per kernel instance
        typedef uint32_t Oid;

        //////////
        //  A common base class for all kernel objects
        class HADESVM_KERNEL_PUBLIC Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Object)

            friend class Kernel;

            //////////
            //  Construction/destruction - from friends only
        protected:
            explicit Object(Kernel * kernel);
            virtual ~Object();

            //////////
            //  Properties
        public:
            //  The Kernel managing this Object.
            Kernel *        kernel() const;

            //  The OID of this Object; unique per Kernel.
            Oid             oid() const;

            //  True if this Object is "live" (and can be used), false
            //  if it is "dead" (and must be recycled).
            bool            live() const;

            //  TODO document
            int             referenceCount() const;
            void            incrementReferenceCount();
            void            decrementReferenceCount();

            //////////
            //  Implementation
        private:
            Kernel *const   _kernel;
            const Oid       _oid;
            bool            _live;
            int             _referenceCount;    //  number of live pointers to this Object
        };

        //////////
        //  Nodes

        //  A "Node" represents a hardware or virtual platform where
        //  a HADES kernel instance is running.
        class HADESVM_KERNEL_PUBLIC Node : public Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Node)

            friend class Device;

            //////////
            //  Construction/destruction - from friends only
        protected:
            Node(Kernel * kernel,
                 const QUuid & uuid, const QString & name);
            virtual ~Node();

            //////////
            //  Operations
        public:
            //  The UUID of this Node - unique worldwide.
            QUuid               uuid() const;

            //  The name of this Node (hopefully unique within cluster).
            QString             name() const;

            //  Returns the Device with the specified name attached to this
            //  Node or nullptr if not found.
            Device *            findDeviceByName(const QString & name) const;

            //////////
            //  Implementation
        private:
            const QUuid         _uuid;
            const QString       _name;
            QMap<QString, Device*>  _devices;   //  name -> Device of devices attached at this Node
        };

        //  A "local" node represents the local hardware unit on which
        //  the HADES kernel runs. There will always be exactly 1 local
        //  Node per a running Kernel instance.
        class HADESVM_KERNEL_PUBLIC LocalNode : public Node
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(LocalNode)

            friend class Kernel;

            //////////
            //  Construction/destruction - from friends only
        protected:
            LocalNode(Kernel * kernel,
                      const QUuid & uuid, const QString & name);
            virtual ~LocalNode();
        };

        //////////
        //  Devices

        //  Represents a physical (Component) or virtual (software-simulated) device.
        class HADESVM_KERNEL_PUBLIC Device : public Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Device)

            //////////
            //  Construction/destruction - from friends only
        protected:
            Device(Kernel * kernel, Node * node, const QString & name);
            virtual ~Device();

            //////////
            //  Operations
        public:
            //  The Node at which this Device is attached.
            Node *              node() const;

            //  The name of this Device. Device names are unique per Node.
            QString             name() const;

            //////////
            //  Implementation
        private:
            Node *const         _node;
            const QString       _name;
        };

        //  Represents a device that represents a hardware Component.
        class HADESVM_KERNEL_PUBLIC PhysicalDevice : public Device
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(PhysicalDevice)

            friend class Kernel;

            //////////
            //  Construction/destruction - from friends only
        protected:
            PhysicalDevice(Kernel * kernel, Node * node, const QString & name,
                           IDeviceComponent * component);
            virtual ~PhysicalDevice();

            //////////
            //  Operations
        public:
            //  Returns the Component represented by this Device.
            IDeviceComponent *  component() const;

            //////////
            //  Implementation
        private:
            IDeviceComponent *const _component;   //  represented by this Device; never nullptr
        };

        //  Represents a device that is implemented in software.
        class HADESVM_KERNEL_PUBLIC VirtualDevice : public Device
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(VirtualDevice)

            //////////
            //  Construction/destruction - from friends only
        protected:
            VirtualDevice(Kernel * kernel, Node * node, const QString & name);
            virtual ~VirtualDevice();

            //////////
            //  Operations
        public:

            //////////
            //  Implementation
        private:
        };

        //////////
        //  Processes and threads

        //  A generic Hades VM kernel process
        class HADESVM_KERNEL_PUBLIC Process : public Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Process)

            friend class Thread;

            //////////
            //  Types
        public:
            //  Process states
            enum class State
            {
                Constructed,    //  process constructed, but not yet started
                Running,        //  process constructed, and started, but not yet finished
                Suspended,      //  process constructed, and started, but is currently suspended
                Finished        //  process has finished; exit code available
            };

            //  Process exit codes
            enum class ExitCode : uint32_t
            {
                Success = 0,
                Unknown = 0xFFFFFFFF    //  e.g. still running (not available)
            };

            //////////
            //  Construction/destruction
        public:
            Process(Kernel * kernel, Process * parent, const QString & name);
            virtual ~Process();

            //////////
            //  Operations
        public:
            //  The name of this process (not necessarily unique!)
            QString             name() const;

            //  The current state of this process
            State               state() const;

            //  The exit code of this process, Unknown if not yet Finished
            ExitCode            exitCode() const;

            //////////
            //  Implementation
        private:
            Process *           _parent;    //  nullptr == none
            QSet<Process*>      _children;

            //  Properties
            QString             _name;
            State               _state;
            int                 _suspendCount;
            QSet<Thread*>       _threads;           //  all threads of this process, dead or alive
            ExitCode            _exitCode;
            Thread *            _mainThread;

            //  Links to other kernel objects
            QSet<Atom*>         _interestingAtoms;      //  Atoms used by this Process
            QSet<Server*>       _implementedServers;    //  Server "implemented by" this Process

            QList<Server*>      _openHandles;   //  index == handle
        };

        //  A generic HADES VM kernel thread
        class HADESVM_KERNEL_PUBLIC Thread : public Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Thread)

            //////////
            //  Types
        public:
            //  Thread states
            enum class State
            {
                Constructed,    //  thread constructed, but not yet started
                Running,        //  thread constructed, and started, but not yet finished
                Waiting,        //  thread is idle-waiting for something to happen
                Suspended,      //  thread constructed, and started, but is currently suspended
                Finished        //  process has finished; exit code available
            };

            //  Thread exit codes
            enum class ExitCode : uint32_t
            {
                Success = 0,
                Unknown = 0xFFFFFFFF    //  e.g. still running (not available)
            };

            //////////
            //  Construction/destruction
        public:
            Thread(Kernel * kernel, Process * process);
            virtual ~Thread();

            //////////
            //  Operations
        public:
            //  The Process to which this thread belongs
            Process *           process() const;

            //  The current state of this thread
            State               state() const;

            //  The exit code of this thread, Unknown if not yet Finished
            ExitCode            exitCode() const;

            //////////
            //  Implementation
        private:
            Process *const      _process;   //  ...to which this thread belongs
            State               _state;
            int                 _suspendCount;
            ExitCode            _exitCode;
            bool                _isDaemon;  //  daemon threads do not keep the process running
        };

        //  A HADES VM kernel thread implemented in C++
        class HADESVM_KERNEL_PUBLIC NativeThread : public Thread
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(NativeThread)

            //////////
            //  Types
        public:
            //  The API for system calls to be made by a running NativeThread.
            class HADESVM_KERNEL_PUBLIC SystemCalls final
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(SystemCalls)

                friend class NativeThread;

                //////////
                //  Construction/destruction - from friends only
            private:
                explicit SystemCalls(NativeThread * nativeThread);
                ~SystemCalls();

                //////////
                //  Operations
            public:
                //  Exits the calling NativeThread immediately
                void            exit(Thread::ExitCode exitCode = Thread::ExitCode::Success);

                //////////
                //  Implementation
            private:
                NativeThread *const _nativeThread;
            };

            //////////
            //  Construction/destruction
        public:
            NativeThread(Kernel * kernel, Process * process);
            virtual ~NativeThread();

            //////////
            //  Properties
        public:
            //  The API for system calls to be made by a running NativeThread.
            SystemCalls         systemCalls;

            //////////
            //  Operations
        public:
            //  Starts the NativeThread by creating a new Qt thread and
            //  executing NativeThread::run() on that Qt thread.
            //  The NativeThread stops (and the corresponding Qt thread
            //  is cleared up) when the run() returns or throws or when
            //  NativeThread::terminate() is called by some other thread.
            void                start();

            //  Terminates the NativeThread immediately, setting its
            //  exitCode. Has no effect if the NativeThread is currently
            //  Constructed or Finished.
            void                terminate(ExitCode exitCode);

            //  Runs the thread to completion.
            virtual void        run() = 0;

            //////////
            //  Implementation
        private:
            class _RunnerThread : public QThread
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_RunnerThread)

                //////////
                //  Construction/destruction - from friends only
            private:
                explicit _RunnerThread(NativeThread * nativeThread);
                virtual ~_RunnerThread();

                //////////
                //  QThread
            public:
                virtual void    run();

                //////////
                //  Implementation
            private:
                NativeThread *const _nativeThread;
            };
            _RunnerThread *     _runnerThread;
        };

        //////////
        //  Servers

        //  A "server" is an object that can be accessed by a Process via
        //  "handles". Processes use "handles" to send messages to the service
        //  and receive responses.
        class HADESVM_KERNEL_PUBLIC Server : public Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Server)

            //////////
            //  Construction/destruction
        public:
            Server(Kernel * kernel, Process * serverProcess);
            virtual ~Server();

            //////////
            //  Operations
        public:
            //  The process that implements this Server (i.e. accepts
            //  incoming Messages, handles them and sends out the responses).
            Process *           serverProcess() const;

            //////////
            //  Implementation
        private:
            Process *const      _serverProcess;
        };

        //  A "service" is a server that has a system-wide-unique name, by
        //  which it can be located by processes which then open "handles" to
        //  the service and use it via those "handles".
        class HADESVM_KERNEL_PUBLIC Service : public Server
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Service)

            //////////
            //  Construction/destruction
        public:
            Service(Kernel * kernel);
            virtual ~Service();

            //////////
            //  Operations
        public:
            //  The name of the service
            QString             name() const { return _name; }
            //  The version of the service
            int                 version() const { return _version; }

            //////////
            //  Implementation
        private:
            const QString       _name;
            const int           _version;   //  > 0
        };

        //  A "servlet" is a server created temporaryly for when a Process
        //  needs it (such as a file access channel, etc.)
        //  Unlike Services, which exist permanently, Servlets are destroyed
        //  when the last handle to them is closed.
        class HADESVM_KERNEL_PUBLIC Servlet : public Server
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Servlet)

            //////////
            //  Construction/destruction
        public:
            Servlet(Kernel * kernel);
            virtual ~Servlet();

            //////////
            //  Operations
        public:

            //////////
            //  Implementation
        private:
        };

        //  A "message" is something that a running Process can send to a
        //  Server, which the Server then processes and responds to.
        class HADESVM_KERNEL_PUBLIC Message : public Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Message)

            //////////
            //  Construction/destruction
        public:
            Message(Kernel * kernel);
            virtual ~Message();

            //////////
            //  Operations
        public:

            //////////
            //  Implementation
        private:
        };

        //////////
        //  Miscellaneous

        //  An "atom" is a system-wide unique string that can be referred
        //  to by any process by its OID.
        class HADESVM_KERNEL_PUBLIC Atom final : public Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Atom)

            //////////
            //  Construction/destruction
        public:
            Atom(Kernel * kernel, const QString & name);
            virtual ~Atom();

            //////////
            //  Operations
        public:
            //  The name of this Atom
            QString             name() const { return _name; }

            //////////
            //  Implementation
        private:
            const QString       _name;
            QSet<Process*>      _interestedProcesses;   //  live Processes that use this Atom
        };
    }
}

//  End of hadesvm-kernel/Objects.hpp
