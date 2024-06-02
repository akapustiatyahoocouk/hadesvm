include(../hadesvm.pri)

SOURCES += \
    ConfigureVirtualApplianceDialog.cpp \
    Main.cpp \
    MainWindow.cpp \
    NewVirtualApplianceDialog.cpp

HEADERS += \
    API.hpp \
    ConfigureVirtualApplianceDialog.hpp \
    MainWindow.hpp \
    NewVirtualApplianceDialog.hpp

FORMS += \
    ConfigureVirtualApplianceDialog.ui \
    MainWindow.ui \
    NewVirtualApplianceDialog.ui

LIBS += -L$$DESTDIR -lhadesvm-kernel -lhadesvm-core -lhadesvm-util
