//
//  hadesvm-util/API.hpp
//
//  hadesvm-util master header
//
//////////
#pragma once

//////////
//  CRT+QT
//  These headers are dirty, so disable some warnings for their processing
#pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
    #pragma GCC diagnostic ignored "-Wsign-promo"
    #pragma GCC diagnostic ignored "-Wctor-dtor-privacy"
    #pragma GCC diagnostic ignored "-Wswitch-default"
    #pragma GCC diagnostic ignored "-Wconversion"

    #include <QtCore/qglobal.h>

    #include <QApplication>
    #include <QAction>
    #include <QDialog>
    #include <QDir>
    #include <QDomDocument>
    #include <QDomElement>
    #include <QException>
    #include <QFileDialog>
    #include <QIcon>
    #include <QLibrary>
    #include <QList>
    #include <QMainWindow>
    #include <QMenu>
    #include <QMenuBar>
    #include <QMessageBox>
    #include <QRecursiveMutex>
    #include <QSet>
    #include <QSettings>
    #include <QStatusBar>
    #include <QTextEdit>
    #include <QThread>
    #include <QTimer>
    #include <QTreeWidget>
    #include <QTreeWidgetItem>
    #include <QVariant>
    #include <QVersionNumber>
    #include <QWidget>
#pragma GCC diagnostic pop

//////////
//  Util components
#include "hadesvm-util/Linkage.hpp"
#include "hadesvm-util/Macros.hpp"

#include "hadesvm-util/Exceptions.hpp"

#include "hadesvm-util/ToString.hpp"
#include "hadesvm-util/FromString.hpp"

#include "hadesvm-util/StockObject.hpp"

#include "hadesvm-util/Plugin.hpp"
#include "hadesvm-util/PluginManager.hpp"

//  End of hadesvm-util/API.hpp
