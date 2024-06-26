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
        //////////
        //  Basic types TODO move into a separate header

        //  An OID of a kernel object - unique per kernel instance
        enum class Oid : uint32_t
        {
            Invalid = 0
        };

        //  A handle to an server opened by a process - unique per process
        enum class Handle : uint32_t
        {
            Invalid = 0xFFFFFFFF    //  invalid handle value
        };

        //  TODO document TODO namespace ?
        static const uint32_t NoTimeout = 0x00000000;
        static const uint32_t InfiniteTimeout = 0xFFFFFFFF;

        //////////
        //  A common base class for all kernel objects
        class HADESVM_KERNEL_PUBLIC Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Object)

            friend class Kernel;
            friend class NativeThread;

            //////////
            //  Construction/destruction - in "kernel locked" state only
        public:
            explicit Object(Kernel * kernel);
            virtual ~Object();

            //////////
            //  Properties - in "kernel locked" state only
        public:
            //  The Kernel managing this Object.
            Kernel *        kernel() const;

            //  The OID of this Object; unique per Kernel.
            Oid             oid() const;

            //  True if this Object is "live" (and can be used), false
            //  if it is "dead" (and must be recycled).
            bool            live() const;

            //  TODO document
            unsigned int    referenceCount() const;
            void            incrementReferenceCount();
            void            decrementReferenceCount();

            //////////
            //  Implementation
        private:
            Kernel *const   _kernel;
            const Oid       _oid;
            bool            _live;
            unsigned int    _referenceCount;    //  number of live pointers to this Object
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
            //  Construction/destruction - in "kernel locked" state only
        public:
            Node(Kernel * kernel,
                 const QUuid & uuid, const QString & name);
            virtual ~Node();

            //////////
            //  Operations - in "kernel locked" state only
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
            QMap<QString, Device*>  _devicesByName; //  name -> Device of devices attached at this Node
        };

        //  A "local" node represents the local hardware unit on which
        //  the HADES kernel runs. There will always be exactly 1 local
        //  Node per a running Kernel instance.
        class HADESVM_KERNEL_PUBLIC LocalNode : public Node
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(LocalNode)

            //////////
            //  Construction/destruction - in "kernel locked" state only
        public:
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
            //  Construction/destruction - in "kernel locked" state only
        public:
            Device(Kernel * kernel, Node * node, const QString & name);
            virtual ~Device();

            //////////
            //  Operations - in "kernel locked" state only
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

            //////////
            //  Construction/destruction - in "kernel locked" state only
        public:
            PhysicalDevice(Kernel * kernel, Node * node, const QString & name,
                           IDeviceComponent * component);
            virtual ~PhysicalDevice();

            //////////
            //  Operations - in "kernel locked" state only
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
            //  Construction/destruction - in "kernel locked" state only
        public:
            VirtualDevice(Kernel * kernel, Node * node, const QString & name);
            virtual ~VirtualDevice();

            //////////
            //  Implementation
        private:
        };

        //////////
        //  Servers

        //  A "server" is an object that can be accessed by a Process via
        //  "handles". Processes use "handles" to send messages to the service
        //  and receive responses.
        class HADESVM_KERNEL_PUBLIC Server : public Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Server)

            friend class Kernel;
            friend class Process;
            friend class NativeThread;

            //////////
            //  Construction/destruction - in "kernel locked" state only
        public:
            Server(Kernel * kernel, Process * serverProcess,
                   unsigned int maxParameters, unsigned int backlog);
            virtual ~Server();

            //////////
            //  Operations - in "kernel locked" state only
        public:
            //  The process that implements this Server (i.e. accepts
            //  incoming Messages, handles them and sends out the responses).
            Process *           serverProcess() const;

            //  Returns the count of handles open to this Server by Processes.
            unsigned int        openHandleCount() const;

            //  Increments the count of handles open to this Server by Processes by 1.
            void                incrementOpenHandleCount();

            //  Decrements the count of handles open to this Server by Processes by 1.
            void                decrementOpenHandleCount();

            //////////
            //  Implementation
        private:
            Process *const      _serverProcess;
            const unsigned int  _maxParameters;
            const unsigned int  _backlog;

            unsigned int        _openHandleCount;

            QQueue<Message*>    _messageQueue;
            QSemaphore          _messageQueueSize;
        };

        //  A "service" is a server that has a system-wide-unique name, by
        //  which it can be located by processes which then open "handles" to
        //  the service and use it via those "handles".
        class HADESVM_KERNEL_PUBLIC Service : public Server
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Service)

            //////////
            //  Construction/destruction - in "kernel locked" state only
        public:
            Service(Kernel * kernel, Process * serverProcess,
                    const QString & name, unsigned int version,
                    unsigned int maxParameters, unsigned int backlog);
            virtual ~Service();

            //////////
            //  Operations - in "kernel locked" state only
        public:
            //  The name of the service
            QString             name() const;
            //  The version of the service
            unsigned int        version() const;

            //////////
            //  Implementation
        private:
            const QString       _name;
            const unsigned int  _version;   //  > 0
        };

        //  A "servlet" is a server created temporaryly for when a Process
        //  needs it (such as a file access channel, etc.)
        //  Unlike Services, which exist permanently, Servlets are destroyed
        //  when the last handle to them is closed.
        class HADESVM_KERNEL_PUBLIC Servlet : public Server
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Servlet)

            //////////
            //  Construction/destruction - in "kernel locked" state only
        public:
            Servlet(Kernel * kernel);
            virtual ~Servlet();

            //////////
            //  Implementation
        private:
        };

        //  A "message" is something that a running Process can send to a
        //  Server, which the Server then processes and responds to.
        class HADESVM_KERNEL_PUBLIC Message : public Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Message)

            friend class Kernel;
            friend class Process;
            friend class NativeThread;

            //////////
            //  Types
        public:
            //  The data type of a single message parameter
            enum class ParameterType
            {
                Bool,
                UInt8,
                Int8,
                UInt16,
                Int16,
                UInt32,
                Int32,
                UInt64,
                Int64,
                Real32,
                Real64,
                Oid,
                Handle
            };

            //  A single parameter of a message
            class HADESVM_KERNEL_PUBLIC Parameter final
            {
                //////////
                //  Construction/destruction/assignment
            public:
                Parameter(ParameterType type, bool value)
                    :   _type(type), _value() { _setValue(value); }
                Parameter(ParameterType type, uint8_t value)
                    :   _type(type), _value() { _setValue(value); }
                Parameter(ParameterType type, int8_t value)
                    :   _type(type), _value() { _setValue(value); }
                Parameter(ParameterType type, uint16_t value)
                    :   _type(type), _value() { _setValue(value); }
                Parameter(ParameterType type, int16_t value)
                    :   _type(type), _value() { _setValue(value); }
                Parameter(ParameterType type, uint32_t value)
                    :   _type(type), _value() { _setValue(value); }
                Parameter(ParameterType type, int32_t value)
                    :   _type(type), _value() { _setValue(value); }
                Parameter(ParameterType type, uint64_t value)
                    :   _type(type), _value() { _setValue(value); }
                Parameter(ParameterType type, int64_t value)
                    :   _type(type), _value() { _setValue(value); }
                Parameter(ParameterType type, float value)
                    :   _type(type), _value() { _setValue(value); }
                Parameter(ParameterType type, double value)
                    :   _type(type), _value() { _setValue(value); }
                Parameter(ParameterType type, Oid value)
                    :   _type(type), _value() { _setValue(value); }
                Parameter(ParameterType type, Handle value)
                    :   _type(type), _value() { _setValue(value); }

                //////////
                //  Implementation
            private:
                const ParameterType _type;
                union
                {
                    bool        boolean;
                    uint8_t     uint8;
                    int8_t      int8;
                    uint16_t    uint16;
                    int16_t     int16;
                    uint32_t    uint32;
                    int32_t     int32;
                    uint64_t    uint64;
                    int64_t     int64;
                    float       real32;
                    double      real64;
                    Oid         oid;
                    Handle      handle;
                } _value;

                //  Helpers
                template <class T>
                void            _setValue(T value)
                {
                    switch (_type)
                    {
                        case ParameterType::Bool:
                            _value.boolean = static_cast<bool>(value);
                            break;
                        case ParameterType::UInt8:
                            _value.uint8 = static_cast<uint8_t>(value);
                            break;
                        case ParameterType::Int8:
                            _value.int8 = static_cast<int8_t>(value);
                            break;
                        case ParameterType::UInt16:
                            _value.uint16 = static_cast<uint16_t>(value);
                            break;
                        case ParameterType::Int16:
                            _value.int16 = static_cast<int16_t>(value);
                            break;
                        case ParameterType::UInt32:
                            _value.uint32 = static_cast<uint32_t>(value);
                            break;
                        case ParameterType::Int32:
                            _value.int32 = static_cast<int32_t>(value);
                            break;
                        case ParameterType::UInt64:
                            _value.uint64 = static_cast<uint64_t>(value);
                            break;
                        case ParameterType::Int64:
                            _value.int64 = static_cast<int64_t>(value);
                            break;
                        case ParameterType::Real32:
                            _value.real32 = static_cast<float>(value);
                            break;
                        case ParameterType::Real64:
                            _value.real64 = static_cast<double>(value);
                            break;
                        case ParameterType::Oid:
                            _value.oid = static_cast<Oid>(value);
                            break;
                        case ParameterType::Handle:
                            _value.handle = static_cast<Handle>(value);
                            break;
                        default:
                            failure();
                    }
                }
            };

            //  Message states
            enum class State
            {
                Constructed,    //  message has just been consructed
                Posted,         //  message has been added to the sermer's message queue
                Processing,     //  server has retrieved message from message queue and is processing it
                Processed,      //  server has signalled the end of message' processing
                Completed       //  the client has called releaseMessage on a message
            };

            //////////
            //  Construction/destruction - in "kernel locked" state only
        public:
            Message(Kernel * kernel, Process * senderProcess, Oid messageTypeAtomOid);
            Message(Kernel * kernel, Process * senderProcess, Oid messageTypeAtomOid,
                    const Parameter & param0);
            Message(Kernel * kernel, Process * senderProcess, Oid messageTypeAtomOid,
                    const Parameter & param0, const Parameter & param1);
            Message(Kernel * kernel, Process * senderProcess, Oid messageTypeAtomOid,
                    const Parameter & param0, const Parameter & param1,
                    const Parameter & param2);
            Message(Kernel * kernel, Process * senderProcess, Oid messageTypeAtomOid,
                    const Parameter & param0, const Parameter & param1,
                    const Parameter & param2, const Parameter & param3);
            Message(Kernel * kernel, Process * senderProcess, Oid messageTypeAtomOid,
                    const QList<Parameter> & params);
            virtual ~Message();

            //////////
            //  Operations - in "kernel locked" state only
        public:
            //  Returns the process that has created the message.
            Process *           senderProcess() const;

            //  Returns the OID of an Atom that represents message type.
            Oid                 messageTypeAtomOid() const;

            //  Returns the current state of the message
            State               state() const;

            //  Returns the Handle within the senderProcess that was used to post
            //  the message; Handle::Invalid if message has not yet been Posted
            Handle              senderHandle() const;

            //  The parameters of the message
            QList<Parameter>    parameters() const;

            //  TODO document
            KErrno              result() const;
            QList<Parameter>    outputs() const;

            //////////
            //  Implementation
        private:
            Process *const      _senderProcess; //  the Process that has created the message
            const Oid           _messageTypeAtomOid;
            State               _state;
            Handle              _senderHandle;  //  the Handle within the _senderProcess that was used to post the message
            Server *            _server;        //  in whose message queue a Posted message resides, else nullptr

            QList<Parameter>    _parameters;

            QSemaphore          _completionCount;   //  starts off as 0, becomes 1 when Processed
            KErrno              _result;    //  set when message becomes Processed
            QList<Parameter>    _outputs;   //  set when message becomes Processed
        };

        //////////
        //  Processes and threads

        //  A generic Hades VM kernel process
        class HADESVM_KERNEL_PUBLIC Process final : public Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Process)

            friend class Kernel;
            friend class Atom;
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
                Success     = 0,
                Terminated  = 0xFFFFFFFE,   //  process terminated from outside instead if exisint voluntarily
                Unknown     = 0xFFFFFFFF    //  e.g. still running (not available)
            };

            //////////
            //  Construction/destruction - in "kernel locked" state only
        public:
            Process(Kernel * kernel, Process * parent, const QString & name);
            virtual ~Process();

            //////////
            //  Operations - in "kernel locked" state only
        public:
            //  The name of this process (not necessarily unique!)
            QString             name() const;

            //  The current state of this process
            State               state() const;

            //  The exit code of this process, Unknown if not yet Finished
            ExitCode            exitCode() const;

            //  Registers a single interest of this Proicess in an Atom with
            //  the specified name.
            KErrno              getAtom(const QString & name, Oid & atomId);

            //  Un-registers a single interest of this Proicess in an Atom with
            //  the specified name.
            KErrno              releaseAtom(Oid atomOid);

            //  Opens a new handle to the specified Server.
            //  Upon success stores the "handle" and returns KErrno::OK.
            //  Upon failure returns error indicator without storing anything.
            KErrno              openHandle(Server * server, Handle & handle);

            //  Closes the existing handle of this process.
            KErrno              closeHandle(Handle handle);

            //  TODO document
            Server *            serverForHandle(Handle handle);

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

            //  Open handle table
            QList<Server*>      _openHandles;   //  index == handle

            //  Links to other kernel objects
            QSet<Server*>       _implementedServers;    //  Server "implemented by" this Process
            QSet<Message*>      _createdMessages;       //  Messages created by this Process

            struct _AtomInterest
            {   //  represents an interest of a Process in an Atom
                Process *       _process;       //  interested in an Atom
                Atom *          _atom;          //  in which a Process is interested
                unsigned int    _interestCount; //  the number of times Process is interested in Atom
            };
            QMap<QString, _AtomInterest*>   _atomInterests; //  ...that involve this Process, name -> interest

            //  Special atoms
            Oid                 _handleOpenMethodAtomOid;
            KErrno              _handleOpenMethodErrno;
            Oid                 _handleClosedMethodAtomOid;
            KErrno              _handleClosedMethodErrno;
        };

        //  A generic HADES VM kernel thread
        class HADESVM_KERNEL_PUBLIC Thread : public Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Thread)

            friend class NativeThread;

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
                Success     = 0,
                Terminated  = 0xFFFFFFFE,   //  thread terminated from outside instead if exisint voluntarily
                Unknown     = 0xFFFFFFFF    //  e.g. still running (not available)
            };

            //////////
            //  Construction/destruction - in "kernel locked" state only
        public:
            Thread(Kernel * kernel, Process * process);
            virtual ~Thread();

            //////////
            //  Operations - in "kernel locked" state only
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

            friend class Kernel;

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
                //  Operations (atoms)
            public:
                //  Creates/reuses the Atom with the specified name.
                //  Upon success stores the OID of the Atom in "atomId"
                //  and returns KErrno::OK; the Atom's reference count
                //  for the current process is incremented by 1.
                //  Upon failure returns the error indicator without affecting
                //  the "atomId" or incrementing anythong.
                KErrno          getAtom(const QString & name, Oid & atomId);

                //  If the process has "opened" the Atom with the specified OID
                //  (by a previous "getAtom" call), decrements the Atom's reference
                //  count on behalf of the calling process by 1 and returns
                //  KErrno::OK; otherwise returns the error indicator without
                //  decrementing anything. If a process calls "getAtom" with the
                //  same atom name multiple times, it must then call "releaseAtom"
                //  an equal number of times.
                //  When all processes lose interest in an Atom, the kernel is free
                //  to destroy the Atom if it so chooses.
                KErrno          releaseAtom(Oid atomOid);

                //  If an Atom with the specified OID exists, stores its name
                //  into "name" and returns KErrno::OK; otherwise returns
                //  KErrno::InvalidParameter without storing anything into "name".
                KErrno          getAtomName(Oid atomOid, QString & name);

                //////////
                //  Operations (services and servlets)
            public:
                //  Creates a new Service with the specified name+version (!= 0).
                //  Messages sent to the service can have at most "maxParameters"
                //  typed parameters (but can have less than that, including 0).
                //  The "backlog", unless 0, specifies the maximum number of
                //  messages that can be posted into the server's incoming message
                //  queue before the queue is considered "full" (in which case
                //  an attempt to post one more message into that queue results in
                //  KErrno::QueueFull, until such time that the server handles at
                //  least 1 message in the queue, thus making space there for a
                //  new message; typical client reaction to a KErrno::QueueFull
                //  would be yield the thread, then retry posting the message).
                //  Upon success, stores the Handle to the newly created Service
                //  on behalf of the current process, stores it to "handle" and
                //  returns KErrno::OK. Upon failure returns the error code and
                //  does not modify the "handle".
                KErrno          createService(const QString & name, unsigned int version,
                                              unsigned int maxParameters, unsigned int backlog,
                                              Handle & handle);

                //  Opens a service with the specified name and version; if version
                //  is 0 then opens the latest available version of the service with
                //  the specified name. Opon success stores the handle to the service
                //  into "handle" and returns KErrno::OK, upon failure returns the
                //  error indicator without affecting the "handle".
                KErrno          openService(const QString & name, unsigned int version,
                                            Handle & handle);

                //////////
                //  Operations (messages)
            public:
                //  Creates a new Message with the specified type (and parameters)
                //  on behalf of the calling Thread's owner Process.
                //  Opun success stores the OID of the newly created Message
                //  into "messageOid" and returns KErrno::OK.
                //  Upon failure returns an error indicator without storing anything.
                KErrno          createMessage(Oid messageTypeAtomOid,
                                              Oid & messageOid);
                KErrno          createMessage(Oid messageTypeAtomOid,
                                              const Message::Parameter & param0,
                                              Oid & messageOid);
                KErrno          createMessage(Oid messageTypeAtomOid,
                                              const Message::Parameter & param0,
                                              const Message::Parameter & param1,
                                              Oid & messageOid);
                KErrno          createMessage(Oid messageTypeAtomOid,
                                              const Message::Parameter & param0,
                                              const Message::Parameter & param1,
                                              const Message::Parameter & param2,
                                              Oid & messageOid);
                KErrno          createMessage(Oid messageTypeAtomOid,
                                              const Message::Parameter & param0,
                                              const Message::Parameter & param1,
                                              const Message::Parameter & param2,
                                              const Message::Parameter & param3,
                                              Oid & messageOid);

                //  Posts the Message with the specified OID, created by the
                //  current Thread's owner Process, to a Server to which a
                //  "handle" has been open. Upon success (Message must be in
                //  a Constructed state), the Message becomes Posted and the
                //  KErron::OK is returned. Upon failure an error indicator
                //  is returned without any changes to the state of the Message.
                KErrno          postMessage(Handle handle, Oid messageOid);

                //  When called by a process that implements a Server, retrieves
                //  the next incoming Message from the Server's message queue.
                //  Upon success, stores message properties into the specified
                //  variables and returns KErrno::OK. Upon failure returns the
                //  error indicator without storing anything.
                //  The "handle" is the handle to the Server that is obtained when
                //  the Process that implements the Server makes a "createService"
                //  call.
                //  The "timeoutMs" specifies the maximum time, in milliseconds,
                //  to idle-wait before giving up. "NoTimeout" here means "don't
                //  wait", i.e. retrieve message if it is immediately available,
                //  else give up. The "InfiniteTimeout" means wait forever.
                KErrno          getMessage(Handle handle, uint32_t timeoutMs,
                                           Oid & messageOid,
                                           Oid & senderProcessOid, Handle & senderProcessHandle,
                                           Oid & messageTypeAtomOid,
                                           QList<Message::Parameter> & params);

                //  Performs idle=wait until the processing of the message with
                //  the specified "messageOid" has been completed (and the Message
                //  is in the Processed state). The Message must have been previously
                //  posted to a Server by the current Process via a "postMessage".
                //  Upon success, stores the "messageResult" and "messageOutputs"
                //  and returns KErrno::OK. Upon failure returns the error indicator
                //  without storing anything.
                //  The "timeoutMs" specifies the maximum time, in milliseconds,
                //  to idle-wait before giving up. "NoTimeout" here means "don't
                //  wait", i.e. retrieve message completion status if it is immediately
                //  available, else give up. The "InfiniteTimeout" means wait forever.
                KErrno          waitForMessage(Oid messageOid, uint32_t timeoutMs,
                                            KErrno & messageResult,
                                            QList<Message::Parameter> & messageOutputs);

                //  "completes" the processing of the specified Message by storing
                //  its result and outputs. The Message must be obtained earlier by
                //  an earlier call to getMessage(). It is an error to attempr to
                //  complete a Message that is not in a Processing state or to
                //  complete it from a Process other than that implementing the Server
                //  through whose message queue the message was obtained by getMessage.
                //  When a Server dies (because it is destroyed, or its implementing
                //  Process dies, etc.) all messages pending in the Server's message
                //  queue are automatically "completed" with result Kerrno::ServerDead.
                KErrno          completeMessage(Oid messageOid, KErrno messageResult,
                                                const QList<Message::Parameter> & messageOutputs);

                //  The client signals that it no longer needs a Mesage it had created
                //  by an earlier call to createMessage.
                KErrno          releaseMessage(Oid messageOid);

                //////////
                //  Operations (miscellaneous)
            public:
                //  TODO document
                QVersionNumber  getSystemVersion();

                //  Exits the calling process immediately; including all threads
                void            exitProcess(Process::ExitCode exitCode = Process::ExitCode::Success);

                //  Exits the calling NativeThread immediately
                void            exitThread(Thread::ExitCode exitCode = Thread::ExitCode::Success);

                //  Closes the specified "handle" of the current process.
                //  Returns the success/failure indicator.
                KErrno          closeHandle(Handle handle);

                //////////
                //  Implementation
            private:
                NativeThread *const _nativeThread;
            };

            //////////
            //  Construction/destruction - in "kernel locked" state only
        public:
            NativeThread(Kernel * kernel, Process * process);
            virtual ~NativeThread();

            //////////
            //  Properties
        public:
            //  The API for system calls to be made by a running NativeThread.
            SystemCalls         systemCalls;

            //////////
            //  Operations - in "kernel locked" state only
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
            void                terminate();

            //  Runs the thread to completion; teturns thread exit code.
            virtual ExitCode    run() = 0;

            //////////
            //  Implementation
        private:
            std::atomic<bool>       _terminationRequested;

            //  Threads
            class _RunnerThread : public QThread
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_RunnerThread)

                friend class Kernel;
                friend class NativeThread;

                //////////
                //  Construction/destruction - from friends only
            private:
                explicit _RunnerThread(NativeThread * nativeThread);
                virtual ~_RunnerThread();

                //////////
                //  QThread
            public:
                virtual void    run() override;

                //////////
                //  Implementation
            private:
                NativeThread *const _nativeThread;
                Kernel *const   _kernel;
            };
            _RunnerThread *     _runnerThread;
        };

        //////////
        //  Miscellaneous

        //  An "atom" is a system-wide unique string that can be referred
        //  to by any process by its OID.
        class HADESVM_KERNEL_PUBLIC Atom final : public Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Atom)

            friend class Process;

            //////////
            //  Construction/destruction - in "kernel locked" state only
        public:
            Atom(Kernel * kernel, const QString & name);
            virtual ~Atom();

            //////////
            //  Operations - in "kernel locked" state only
        public:
            //  The name of this Atom
            QString             name() const;

            //////////
            //  Implementation
        private:
            const QString       _name;
            using _AtomInterest = Process::_AtomInterest;
            QSet<_AtomInterest*>    _atomInterests;   //  ...that involve this Atom
        };
    }
}

//  End of hadesvm-kernel/Objects.hpp
