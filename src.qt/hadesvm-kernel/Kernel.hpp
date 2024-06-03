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
        //  A Hades VM kernel
        class HADESVM_KERNEL_PUBLIC Kernel final : public hadesvm::core::Component
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Kernel)

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
            //  Operations (validation)
        public:
            static bool         isValidNodeName(const QString & name);
            static bool         isValidVolumeName(const QString & name);

            //////////
            //  Implementation
        private:
            QRecursiveMutex     _guard;     //  for synchronizing access to all kernel data

            //  Configuration
            QUuid               _nodeUuid;
            QString             _nodeName;
            QMap<QString, QString>  _mountedFolders;    //  volume namd -> full host path

            //  Runtime state
            QMap<Oid, Object*>  _objects;   //  all existing+live kernel objects
        };
    }
}

//  End of hadesvm-kernel/Kernel.hpp
