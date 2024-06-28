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

    /*  TODO kill off - this is debug code
    const unsigned Iterations = 1'000'000'000;

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

    hadesvm::gui::MainWindow w;
    w.show();
    return a.exec();
}

//  End of hadesvm-gui/Main.cpp
