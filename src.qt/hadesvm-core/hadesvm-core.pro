include(../hadesvm.pri)

TEMPLATE = lib
DEFINES += HADESVM_CORE_LIBRARY

SOURCES += \
    Component.cpp \
    ComponentAdaptor.cpp \
    ComponentAdaptorType.cpp \
    ComponentCategory.cpp \
    ComponentType.cpp \
    Exceptions.cpp \
    Plugins.cpp \
    RemoteTerminal.cpp \
    RemoteTerminalType.cpp \
    StandardComponentCategories.cpp \
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
    ComponentAdaptor.hpp \
    ComponentAdaptorType.hpp \
    ComponentCategory.hpp \
    ComponentType.hpp \
    Exceptions.hpp \
    Linkage.hpp \
    VirtualAppliance.hpp \
    VirtualApplianceTemplate.hpp \
    VirtualApplianceType.hpp \
    VirtualArchitecture.hpp

LIBS += -L$$DESTDIR -lhadesvm-util
