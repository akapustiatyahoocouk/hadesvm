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
            //  Operations (Node)
        public:
            //  Find a Node by UUID, returns nullptr if not found
            Node *              findNodeByUuid(const QUuid & uuid) const;

            //////////
            //  Operations (validation)
        public:
            //  TODO document
            static bool         isValidNodeName(const QString & name);
            static bool         isValidVolumeName(const QString & name);
            static bool         isValidDeviceName(const QString & name);

            //////////
            //  Operations (runtime state)
        public:
            //  Returns true if Kernel is "locked" by the current thread, else false.
            bool                isLockedByCurrentThread() const;

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
            LocalNode *         _localNode;
            Process *           _deviceManagerProcess;

            //  Helpers
            Oid                 _generateUniqueOid();
        };
    }
}

//  End of hadesvm-kernel/Kernel.hpp
