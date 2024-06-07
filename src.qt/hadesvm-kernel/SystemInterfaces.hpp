//
//  hadesvm-kernel/SystemInterfaces.hpp
//
//  hadesvm-kernel system interface and method names
//
//////////

namespace hadesvm
{
    namespace kernel
    {
        class HADESVM_KERNEL_PUBLIC SystemInterfaces final
        {
            HADESVM_UTILITY_CLASS(SystemInterfaces)

            //  IUnknown - interface and methods implemented by all Servers
        public:
            class HADESVM_KERNEL_PUBLIC IUnknown final
            {
                HADESVM_UTILITY_CLASS(IUnknown)

                //////////
                //  Properties
            public:
                //  The name of the interface
                static const QString    Name;

                //  A message with 1 "Oid" parameter.
                //  If the Server implements the interface with the name given
                //  by the Atom with the specified Oid, it will return KErrno::OK;
                //  otherwise it will return KErrno::NotImplemented.
                static const QString    QueryInterface;

                //  A message with 1 "Handle" parameter.
                //  Informs the Server that some process opened a Handle to that
                //  Server.
                //  HandleOpen messages are generated by the Kernel and posted to
                //  the server message queue DISREGARDING the queue backlog limit.
                //  Always returns KErrno::OK. As these messages are not explicitly
                //  sent by any Process, there's no wait-until-message-processed.
                static const QString    HandleOpen;

                //  A message with 1 "Handle" parameter.
                //  Informs the Server that some process closed a Handle to that
                //  Server (whether voluntarily or not; e.g. when a Process exits
                //  all handles it holds open are force-closed).
                //  HandleClosed messages are generated by the Kernel and posted to
                //  the server message queue DISREGARDING the queue backlog limit.
                //  Always returns KErrno::OK. As these messages are not explicitly
                //  sent by any Process, there's no wait-until-message-processed.
                static const QString    HandleClosed;
            };
        };
    }
}

//  End of hadesvm-kernel/SystemInterfaces.hpp
