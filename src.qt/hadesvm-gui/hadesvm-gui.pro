include(../hadesvm.pri)

SOURCES += \
    AboutDialog.cpp \
    ConfigureVirtualApplianceDialog.cpp \
    FullScreenWindow.cpp \
    Main.cpp \
    MainWindow.cpp \
    NewVirtualApplianceDialog.cpp \
    VirtualApplianceWindow.cpp

HEADERS += \
    API.hpp \
    AboutDialog.hpp \
    ConfigureVirtualApplianceDialog.hpp \
    FullScreenWindow.hpp \
    MainWindow.hpp \
    NewVirtualApplianceDialog.hpp \
    VirtualApplianceWindow.hpp

FORMS += \
    AboutDialog.ui \
    ConfigureVirtualApplianceDialog.ui \
    MainWindow.ui \
    NewVirtualApplianceDialog.ui \
    VirtualApplianceWindow.ui

LIBS += -L$$DESTDIR -lhadesvm-kernel -lhadesvm-core -lhadesvm-util

RESOURCES += \
    Resources.qrc
