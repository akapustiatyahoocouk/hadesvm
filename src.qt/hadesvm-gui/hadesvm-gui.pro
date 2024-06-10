include(../hadesvm.pri)

SOURCES += \
    ConfigureVirtualApplianceDialog.cpp \
    Main.cpp \
    MainWindow.cpp \
    NewVirtualApplianceDialog.cpp \
    VirtualApplianceWindow.cpp

HEADERS += \
    API.hpp \
    ConfigureVirtualApplianceDialog.hpp \
    MainWindow.hpp \
    NewVirtualApplianceDialog.hpp \
    VirtualApplianceWindow.hpp

FORMS += \
    ConfigureVirtualApplianceDialog.ui \
    MainWindow.ui \
    NewVirtualApplianceDialog.ui \
    VirtualApplianceWindow.ui

LIBS += -L$$DESTDIR -lhadesvm-kernel -lhadesvm-core -lhadesvm-util
