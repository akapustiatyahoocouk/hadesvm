//
//  hadesvm-util/PluginManager.cpp
//
//  The vsm::util::PluginManager class implementation
//
//////////
#include "hadesvm-util/API.hpp"
using namespace hadesvm::util;

namespace
{
    QSet<QString> processedDlls;
    QSet<Plugin*> discoveredPlugins;
    QSet<Plugin*> initialisedPlugins;
}

//////////
//  Operations
void PluginManager::loadPlugins()
{
    QString exeDir = QCoreApplication::applicationDirPath();
    QDir directory(exeDir);
    //  Load...
    for (const auto & entry : directory.entryList())
    {
        QFileInfo fileInfo(exeDir, entry);
        QString libPath = fileInfo.absoluteFilePath();
        if (fileInfo.isFile() && !fileInfo.isSymLink() && !processedDlls.contains(libPath))
        {
            processedDlls.insert(libPath);

            QLibrary * lib = new QLibrary(libPath);
            lib->load();
            if (lib->isLoaded())
            {
                PluginExportProc pluginExportProc =
                    reinterpret_cast<PluginExportProc>(lib->resolve("vs_plugin_proc"));
                if (pluginExportProc != nullptr)
                {
                    PluginList newPlugins;
                    (*pluginExportProc)(newPlugins);
                    for (auto newPlugin : newPlugins)
                    {
                        //  TODO kill off qDebug() << fileInfo << ": " << newPlugin;
                        discoveredPlugins.insert(newPlugin);
                    }
                }
                else
                {
                    lib->unload();
                }
            }
        }
    }
    //  ...and initialise
    for (bool keepGoing = true; keepGoing; )
    {
        keepGoing = false;
        for (auto discoveredPlugin : discoveredPlugins)
        {
            if (!initialisedPlugins.contains(discoveredPlugin))
            {
                try
                {
                    discoveredPlugin->initialise();
                    initialisedPlugins.insert(discoveredPlugin);
                    keepGoing = true;
                }
                catch (...)
                {   //  OOPS! Suppress, though
                }
            }
        }
    }
}

//  End of hadesvm-util/PluginManager.cpp
