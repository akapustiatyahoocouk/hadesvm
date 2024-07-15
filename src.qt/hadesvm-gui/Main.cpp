//
//  hadesvm-gui/Main.cpp
//
//  hadesvm-gui entry point
//
//////////
#include "hadesvm-gui/API.hpp"
using namespace hadesvm::gui;

//////////
//  App entry point
int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("AK");
    QCoreApplication::setOrganizationDomain("www.cybernetic.org/ak");
    QCoreApplication::setApplicationName("HadesVM");

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/hadesvm/gui/images/objects/ComputerLarge.png"));   //  doesn't work on Linux

#if 0   //  TODO what ?
#ifdef Q_OS_WINDOWS
    if (/*AttachConsole(ATTACH_PARENT_PROCESS) ||*/ AllocConsole())
    {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
#endif
#endif

    /*  TODO kill off - this is debug code
    const unsigned Iterations = 1'000'000;

    QMutex m;
    QElapsedTimer et1;
    et1.start();
    for (unsigned i = 0; i < Iterations; i++)
    {
        QMutexLocker lock(&m);
    }
    qint64 ns1 = et1.nsecsElapsed();
    qDebug() << "QMutex: " << ns1 / Iterations << " ns per lock/unlock cycle";

    hadesvm::util::Spinlock s;
    QElapsedTimer et2;
    et2.start();
    for (unsigned i = 0; i < Iterations; i++)
    {
        QMutexLocker lock(&s);
    }
    qint64 ns2 = et2.nsecsElapsed();
    qDebug() << "Spinlock: " << ns2 / Iterations << " ns per lock/unlock cycle";
    */

    hadesvm::util::PluginManager::loadPlugins();

    MainWindow w;
    if (Preferences::startMinimized())
    {
        w.showMinimized();
    }
    else
    {
        w.show();
    }
    return a.exec();
}

//  End of hadesvm-gui/Main.cpp
