//
//  hadesvm-cereon/Plugins.cpp
//
//  hadesvm-cereon plugins
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

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
        return "Cereon";
    }

    QVersionNumber Plugin::version() const
    {
        return QVersionNumber(1, 0, 0);
    }

    void Plugin::initialise()
    {
        hadesvm::core::VirtualArchitecture::register(CereonWorkstationArchitecture::instance());

        hadesvm::core::ComponentType::register(MemoryBus::Type::instance());
        hadesvm::core::ComponentType::register(ResidentRamUnit::Type::instance());
        hadesvm::core::ComponentType::register(ResidentRomUnit::Type::instance());
        hadesvm::core::ComponentType::register(IoBus::Type::instance());
        hadesvm::core::ComponentType::register(Cereon1P1B::Type::instance());
    }
}

HADESVM_BEGIN_PLUGIN_TABLE()
    HADESVM_EXPORT_PLUGIN(Plugin::instance())
HADESVM_END_PLUGIN_TABLE()

//  End of hadesvm-cereon/Plugins.cpp
