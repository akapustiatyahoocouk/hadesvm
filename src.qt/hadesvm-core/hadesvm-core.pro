include(../hadesvm.pri)

TEMPLATE = lib
DEFINES += HADESVM_CORE_LIBRARY

SOURCES += \
    Component.cpp \
    ComponentCategory.cpp \
    ComponentType.cpp \
    Exceptions.cpp \
    Plugins.cpp \
    RemoteTerminal.cpp \
    RemoteTerminalType.cpp \
    VirtualAppliance.cpp \
    VirtualApplianceTemplate.cpp \
    VirtualApplianceType.cpp \
    VirtualArchitecture.cpp \
    VirtualMachine.cpp \
    VirtualMachineType.cpp

HEADERS += \
    API.hpp \
    Classes.hpp \
    Component.hpp \
    ComponentCategory.hpp \
    ComponentType.hpp \
    Exceptions.hpp \
    Linkage.hpp \
    VirtualAppliance.hpp \
    VirtualApplianceTemplate.hpp \
    VirtualApplianceType.hpp \
    VirtualArchitecture.hpp

LIBS += -L$$DESTDIR -lhadesvm-util
