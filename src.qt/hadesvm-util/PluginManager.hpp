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
        typedef void (*PluginExportProc)(PluginList &);
    }
}

#define HADESVM_BEGIN_PLUGIN_TABLE()                                        \
    extern "C"                                                              \
    Q_DECL_EXPORT void vs_plugin_proc(hadesvm::util::PluginList & plugins); \
    extern "C"                                                              \
    Q_DECL_EXPORT void vs_plugin_proc(hadesvm::util::PluginList & plugins)  \
    {

#define HADESVM_EXPORT_PLUGIN(instance)     plugins.append(instance);

#define HADESVM_END_PLUGIN_TABLE()          }

//  End of hadesvm-util/PluginManager.hpp
