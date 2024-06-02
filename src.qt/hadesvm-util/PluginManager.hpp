//
//  hadesvm-util/PluginManager.hpp
//
//  hadesvm plugin Manager
//
//////////

namespace hadesvm
{
    namespace util
    {
        class HADESVM_UTIL_PUBLIC PluginManager
        {
            HADESVM_UTILITY_CLASS(PluginManager)

            //////////
            //  Operations
        public:
            static void         loadPlugins();
        };

        extern "C"
        typedef PluginList (*PluginExportProc)(void);
    }
}

#define HADESVM_BEGIN_PLUGIN_TABLE()                            \
    extern "C"                                                  \
    Q_DECL_EXPORT hadesvm::util::PluginList vs_plugin_proc();   \
    extern "C"                                                  \
    Q_DECL_EXPORT hadesvm::util::PluginList vs_plugin_proc()    \
    {                                                           \
        static hadesvm::util::PluginList result;                \
        if (result.empty())                                     \
        {

#define HADESVM_EXPORT_PLUGIN(instance)                         \
            result.append(instance);

#define HADESVM_END_PLUGIN_TABLE()                              \
        }                                                       \
        return result;                                          \
    }

//  End of hadesvm-util/PluginManager.hpp
