//
//  hadesvm-util/API.hpp
//
//  hadesvm-util master header
//
//////////
#pragma once

//////////
//  CRT+QT
#include <QtCompilerDetection>

//  These headers are dirty, so disable some warnings for their processing
#if defined(Q_CC_GNU)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
    #pragma GCC diagnostic ignored "-Wsign-promo"
    #pragma GCC diagnostic ignored "-Wctor-dtor-privacy"
    #pragma GCC diagnostic ignored "-Wswitch-default"
    #pragma GCC diagnostic ignored "-Wconversion"
#elif defined(Q_CC_MSVC)
    #pragma warning(disable:4191)   //  'reinterpret_cast': unsafe conversion from '<T1>' to '<T2>'
    #pragma warning(disable:4702)   //  unreachable code TODO MOC-related?
    #pragma warning(disable:4866)   //  compiler may not enforce left-to-right evaluation order for call to 'C++17 operator'
    #pragma warning(disable:5045)   //  Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified

    #pragma warning(push)
    #pragma warning(disable:4365)   //  '=': conversion from '<T1>' to '<T2>', signed/unsigned mismatch
    #pragma warning(disable:4371)   //  '<CLASS>': layout of class may have changed from a previous version of the compiler due to better packing of member '<MEMBER>'
    #pragma warning(disable:5204)   //  '<CLASS>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
    #pragma warning(disable:5219)   //  implicit conversion from 'const int' to 'float', possible loss of data
#else
    #error Unsupported compiler
#endif

#include <QtCore/qglobal.h>

#include <QApplication>
#include <QAction>
#include <QDialog>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QException>
#include <QFileDialog>
#include <QHostInfo>
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
#include <QUuid>
#include <QVariant>
#include <QVersionNumber>
#include <QWidget>

#if defined(Q_CC_GNU)
    #pragma GCC diagnostic pop
#elif defined(Q_CC_MSVC)
    #pragma warning(pop)
#else
    #error Unsupported compiler
#endif

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
