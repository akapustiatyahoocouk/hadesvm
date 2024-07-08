include(../hadesvm.pri)

SOURCES += \
    AboutDialog.cpp \
    ConfigureVirtualApplianceDialog.cpp \
    FullScreenWindow.cpp \
    Main.cpp \
    MainWindow.cpp \
    NewVirtualApplianceDialog.cpp \
    Preferences.cpp \
    PreferencesDialog.cpp \
    VirtualApplianceWindow.cpp

HEADERS += \
    API.hpp \
    AboutDialog.hpp \
    ConfigureVirtualApplianceDialog.hpp \
    FullScreenWindow.hpp \
    MainWindow.hpp \
    NewVirtualApplianceDialog.hpp \
    Preferences.hpp \
    PreferencesDialog.hpp \
    VirtualApplianceWindow.hpp

FORMS += \
    AboutDialog.ui \
    ConfigureVirtualApplianceDialog.ui \
    MainWindow.ui \
    NewVirtualApplianceDialog.ui \
    PreferencesDialog.ui \
    VirtualApplianceWindow.ui

LIBS += -L$$DESTDIR -lhadesvm-kernel -lhadesvm-core -lhadesvm-util

RESOURCES += \
    Resources.qrc
