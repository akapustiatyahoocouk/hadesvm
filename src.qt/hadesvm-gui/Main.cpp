//
//  hadesvm-gui/Main.cpp
//
//  hadesvm-gui entry point
//
//////////
#include "hadesvm-gui/API.hpp"

//////////
//  App entry point
int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("AK");
    QCoreApplication::setOrganizationDomain("www.cybernetic.org/ak");
    QCoreApplication::setApplicationName("HadesVM");

    QApplication a(argc, argv);

    hadesvm::util::PluginManager::loadPlugins();

    hadesvm::gui::MainWindow w;
    w.show();
    return a.exec();
}

//  End of hadesvm-gui/Main.cpp
