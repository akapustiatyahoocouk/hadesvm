//
//  hadesvm-kernel/Plugins.cpp
//
//  hadesvm-kernel plugins
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

namespace
{
    class Plugin : public hadesvm::util::Plugin
    {
        HADESVM_DECLARE_SINGLETON(Plugin)

        //////////
        //  hadesvm::util::Plugin
    public:
        virtual QString         displayName() const override;
        virtual QVersionNumber  version() const override;
        virtual void            initialise() override;
    };

    //////////
    //  Singleton
    HADESVM_IMPLEMENT_SINGLETON(Plugin)
    Plugin::Plugin() {}
    Plugin::~Plugin() {}

    //////////
    //  hadesvm::util::Plugin
    QString Plugin::displayName() const
    {
        return "HadesVM Kernel";
    }

    QVersionNumber Plugin::version() const
    {
        return QVersionNumber(1, 0, 0);
    }

    void Plugin::initialise()
    {
        hadesvm::core::VirtualArchitecture::register(Architecture::instance());

        hadesvm::core::ComponentType::register(Kernel::Type::instance());
    }
}

HADESVM_BEGIN_PLUGIN_TABLE()
    HADESVM_EXPORT_PLUGIN(Plugin::instance())
HADESVM_END_PLUGIN_TABLE()

//  End of hadesvm-kernel/Plugins.cpp
