//
//  hadesvm-kernel/Node.hpp
//
//  hadesvm kernel Nodes
//
//////////

namespace hadesvm
{
    namespace kernel
    {
        //  A "Node" represents a hardware or virtual platform where
        //  a HADES kernel instance is running.
        class HADESVM_KERNEL_PUBLIC Node : public Object
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Node)

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
            QUuid           uuid() const { return _uuid; }

            //  The name of this Node (hopefully unique within cluster).
            QString         name() const { return _name; }

            //////////
            //  Implementation
        private:
            QUuid           _uuid;
            QString         _name;
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
    }
}

//  End of hadesvm-kernel/Process.hpp
