include(../hadesvm.pri)

SOURCES += \
    Main.cpp \
    MainWindow.cpp \
    NewVirtualApplianceDialog.cpp

HEADERS += \
    API.hpp \
    MainWindow.hpp \
    NewVirtualApplianceDialog.hpp

FORMS += \
    MainWindow.ui \
    NewVirtualApplianceDialog.ui

LIBS += -L$$DESTDIR -lhadesvm-kernel -lhadesvm-core -lhadesvm-util
