//
//  hadesvm-kernel/Kernel.hpp
//
//  hadesvm kernel
//
//////////

namespace hadesvm
{
    namespace kernel
    {
        //  A recursive mutex that keeps track of its locking properties
        class HADESVM_KERNEL_PUBLIC KernelMutex final
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(KernelMutex)

            //////////
            //  Construction/destruction
        public:
            KernelMutex()
                :   _impl(),
                _lockCount(0),
                _lockingThread(nullptr)
            {
            }

            ~KernelMutex()
            {
            }

            //////////
            //  Operations
        public:
            //  Locks this mutex, idle-waiting if necessary
            void                lock()
            {
                _impl.lock();
                Q_ASSERT(_lockCount >= 0);
                _lockCount++;
                _lockingThread = QThread::currentThread();
            }

            //  Unlocks this mutex
            void                unlock()
            {
                _lockCount--;
                Q_ASSERT(_lockCount >= 0);
                _impl.unlock();
            }

            //  The thread that currently holds this mutex locked, nullptr if none.
            QThread *           lockingThread() const
            {
                return (_lockCount == 0) ? nullptr : _lockingThread;
            }

            //////////
            //  Implementation
        private:
            QRecursiveMutex     _impl;
            std::atomic<int>    _lockCount;
            QThread *           _lockingThread;
        };

        //  A Hades VM kernel
        class HADESVM_KERNEL_PUBLIC Kernel final : public hadesvm::core::Component
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Kernel)

            friend class Object;
            friend class Node;
            friend class NativeThread;
            friend class Atom;
            friend class Process;

            //////////
            //  Types
        public:
            //  The type of a Kernel component
            class HADESVM_KERNEL_PUBLIC Type final : public hadesvm::core::ComponentType
            {
                HADESVM_DECLARE_SINGLETON(Type);

                //////////
                //  hadesvm::util::StockObject
            public:
                virtual QString         mnemonic() const override;
                virtual QString         displayName() const override;

                //////////
                //  hadesvn::core::ComponentType
            public:
                virtual hadesvm::core::ComponentCategory * category() const override;
                virtual bool            suspendable() const override;
                virtual bool            isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const override;
                virtual bool            isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const override;
                virtual hadesvm::core::Component * createComponent() override;
            };

            //////////
            //  Types
        public:
            //  The API for system calls to be made by a running NativeThread.
            class HADESVM_KERNEL_PUBLIC SystemCalls final
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(SystemCalls)

                friend class Kernel;

                //////////
                //  Construction/destruction - from friends only
            private:
                explicit SystemCalls(Kernel * kernel);
                ~SystemCalls();

                //////////
                //  Operations (atoms)
            public:
                //  Creates/reuses the Atom with the specified name.
                //  Upon success stores the OID of the Atom in "atomId"
                //  and returns KErrno::OK; the Atom's reference count
                //  for the specified "thread"'s process is incremented by 1.
                //  Upon failure returns the error indicator without affecting
                //  the "atomId" or incrementing anythong.
                KErrno          getAtom(Thread * thread, const QString & name, Oid & atomId);

                //  If the "thread"'s process has "opened" the Atom with the specified
                //  OID (by a previous "getAtom" call), decrements the Atom's reference
                //  count on behalf of the specified "thread"'s process by 1 and returns
                //  KErrno::OK; otherwise returns the error indicator without
                //  decrementing anything. If a process calls "getAtom" with the
                //  same atom name multiple times, it must then call "releaseAtom"
                //  an equal number of times.
                //  When all processes lose interest in an Atom, the kernel is free
                //  to destroy the Atom if it so chooses.
                KErrno          releaseAtom(Thread * thread, Oid atomOid);

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
                KErrno          createService(Thread * thread,
                                              const QString & name, unsigned int version,
                                              unsigned int maxParameters, unsigned int backlog,
                                              Handle & handle);

                //  Opens a service with the specified name and version; if version
                //  is 0 then opens the latest available version of the service with
                //  the specified name. Opon success stores the handle to the service
                //  into "handle" and returns KErrno::OK, upon failure returns the
                //  error indicator without affecting the "handle".
                KErrno          openService(Thread * thread,
                                            const QString & name, unsigned int version,
                                            Handle & handle);

                //////////
                //  Operations (messages)
            public:
                //  Creates a new Message with the specified type (and parameters)
                //  on behalf of the specified Thread's owner Process.
                //  Opun success stores the OID of the newly created Message
                //  into "messageOid" and returns KErrno::OK.
                //  Upon failure returns an error indicator without storing anything.
                KErrno          createMessage(Thread * thread, Oid messageTypeAtomOid,
                                              Oid & messageOid);
                KErrno          createMessage(Thread * thread, Oid messageTypeAtomOid,
                                              const Message::Parameter & param0,
                                              Oid & messageOid);
                KErrno          createMessage(Thread * thread, Oid messageTypeAtomOid,
                                              const Message::Parameter & param0,
                                              const Message::Parameter & param1,
                                              Oid & messageOid);
                KErrno          createMessage(Thread * thread, Oid messageTypeAtomOid,
                                              const Message::Parameter & param0,
                                              const Message::Parameter & param1,
                                              const Message::Parameter & param2,
                                              Oid & messageOid);
                KErrno          createMessage(Thread * thread, Oid messageTypeAtomOid,
                                              const Message::Parameter & param0,
                                              const Message::Parameter & param1,
                                              const Message::Parameter & param2,
                                              const Message::Parameter & param3,
                                              Oid & messageOid);
                KErrno          createMessage(Thread * thread, Oid messageTypeAtomOid,
                                              const QList<Message::Parameter> & params,
                                              Oid & messageOid);

                //  Posts the Message with the specified OID, created by the
                //  specified Thread's owner Process, to a Server to which a
                //  "handle" has been open. Upon success (Message must be in
                //  a Constructed state), the Message becomes Posted and the
                //  KErron::OK is returned. Upon failure an error indicator
                //  is returned without any changes to the state of the Message.
                KErrno          postMessage(Thread * thread, Handle handle, Oid messageOid);

                //////////
                //  Operations (miscellaneous)
            public:
                //  TODO document
                QVersionNumber  getSystemVersion();

                //  TODO document
                KErrno          closeHandle(Thread * thread, Handle & handle);

                //////////
                //  Implementation
            private:
                Kernel *const   _kernel;
            };

            //////////
            //  Construction/destruction
        public:
            Kernel();
            virtual ~Kernel();

            //////////
            //  hadesvm::core::Component
        public:
            virtual Type *      type() const override;
            virtual QString     displayName() const override;
            virtual void        serialiseConfiguration(QDomElement componentElement) override;
            virtual void        deserialiseConfiguration(QDomElement componentElement) override;
            virtual hadesvm::core::ComponentEditor *    createEditor(QWidget * parent) override;

            //////////
            //  hadesvm::core::Component (state management)
        public:
            virtual State       state() const noexcept override;
            virtual void        connect() throws(hadesvm::core::VirtualApplianceException) override;
            virtual void        initialize() throws(hadesvm::core::VirtualApplianceException) override;
            virtual void        start() throws(hadesvm::core::VirtualApplianceException) override;
            virtual void        stop() noexcept override;
            virtual void        deinitialize() noexcept override;
            virtual void        disconnect() noexcept override;

            //////////
            //  Operations (configuration)
        public:
            //  Gets the node UUID for the HADES kernel instance.
            QUuid               nodeUuid() const { return _nodeUuid; }

            //  Gets/sets the node name for the HADES kernel instance.
            QString             nodeName() const { return _nodeName; }
            void                setNodeName(const QString & nodeName);

            //  An unordered list of volume names of all mounted folders.
            QStringList         mountedFolderVolumeNames() const;

            //  Returns the path of the mounted folder with the specified
            //  volume name; returns "" if such mounted folder does not exist.
            QString             mountedFolderPath(const QString & volumeName) const;

            //  Adds the (volumeName, path) mounted folder. If one already
            //  exists for the specified volumeName, replaces it.
            void                setMountedFolderPath(const QString & volumeName, const QString & path);

            //  Removes a mounted folder with the specified volume name.
            //  Has no effect such mounted folder does not exist.
            void                removeMountedFolder(const QString & volumeName);

            //////////
            //  System call gateway
        public:
            SystemCalls         systemCalls;

            //////////
            //  Operations (validation)
        public:
            //  TODO document
            static bool         isValidNodeName(const QString & name);
            static bool         isValidVolumeName(const QString & name);
            static bool         isValidDeviceName(const QString & name);
            static bool         isValidServiceName(const QString & name);

            //////////
            //  Operations (runtime state)
        public:
            //  Returns true if Kernel is "locked" by the current thread, else false.
            bool                isLockedByCurrentThread() const;

            //  TODO document
            void                lock();
            void                unlock();

            //////////
            //  Implementation
        private:
            //  State
            State               _state;

            //  Configuration
            QUuid               _nodeUuid;
            QString             _nodeName;
            QMap<QString, QString>  _mountedFolders;    //  volume namd -> full host path

            //  Runtime state
            KernelMutex         _runtimeStateGuard;

            //  Primary object maps
            QRandomGenerator    _oidGenerator;
            QMap<Oid, Object*>  _liveObjects;   //  all existing+live kernel objects
            QMap<Oid, Object*>  _deadObjects;   //  all existing+dead kernel objects

            //  Secondary object maps
            QMap<QUuid, Node*>  _nodesByUuid; //  node UUID -> node
            QMap<QString, Atom*>_atomsByName; //  all that exist
            LocalNode *         _localNode;
            Process *           _deviceManagerProcess;

            //  Helpers
            Oid                 _generateUniqueOid();
        };
    }
}

//  End of hadesvm-kernel/Kernel.hpp
