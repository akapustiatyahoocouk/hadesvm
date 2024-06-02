//
//  hadesvm-core/Plugins.cpp
//
//  hadesvm-core plugins
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

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
        return "HadesVM Core";
    }

    QVersionNumber Plugin::version() const
    {
        return QVersionNumber(1, 0, 0);
    }

    void Plugin::initialise()
    {
        VirtualApplianceType::register(VirtualMachineType::instance());
        VirtualApplianceType::register(RemoteTerminalType::instance());

        for (auto cc : StandardComponentCategories::all())
        {
            ComponentCategory::register(cc);
        }
    }
}

HADESVM_BEGIN_PLUGIN_TABLE()
    HADESVM_EXPORT_PLUGIN(Plugin::instance())
HADESVM_END_PLUGIN_TABLE()

//  End of hadesvm-core/Plugins.cpp
