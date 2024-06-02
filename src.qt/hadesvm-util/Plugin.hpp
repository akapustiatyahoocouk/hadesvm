//
//  hadesvm-util/Plugin.hpp
//
//  Plugin ADT
//
//////////

namespace hadesvm
{
    namespace util
    {
        class HADESVM_UTIL_PUBLIC Plugin
        {
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Plugin)

            //////////
            //  Construction/destruction
        public:
            Plugin() = default;
            virtual ~Plugin() = default;

            //////////
            //  Operations
        public:
            virtual QString         displayName() const = 0;
            virtual QVersionNumber  version() const = 0;

            //  Called at most once by PluginManager to initialise this plugin,
            //  Returns on success; thros on failure.
            virtual void            initialise() = 0;

            //////////
            //  Implementation
        private:
            bool            _initialised = false;
        };

        using PluginList = QList<Plugin*>;
    }
}

//  End of hadesvm-util/Plugin.hpp
