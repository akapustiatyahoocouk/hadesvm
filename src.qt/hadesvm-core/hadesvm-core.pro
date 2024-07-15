include(../hadesvm.pri)

TEMPLATE = lib
DEFINES += HADESVM_CORE_LIBRARY

SOURCES += \
    ClockFrequency.cpp \
    Component.cpp \
    ComponentAdaptor.cpp \
    ComponentAdaptorType.cpp \
    ComponentCategory.cpp \
    ComponentEditor.cpp \
    ComponentType.cpp \
    DisplayWidget.cpp \
    Exceptions.cpp \
    MemorySize.cpp \
    Plugins.cpp \
    RemoteTerminal.cpp \
    RemoteTerminalType.cpp \
    StandardComponentCategories.cpp \
    StatusBarWidget.cpp \
    TimeInterval.cpp \
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
    ComponentEditor.hpp \
    ComponentType.hpp \
    DisplayWidget.hpp \
    Exceptions.hpp \
    Linkage.hpp \
    StatusBarWidget.hpp \
    Types.hpp \
    VirtualAppliance.hpp \
    VirtualApplianceTemplate.hpp \
    VirtualApplianceType.hpp \
    VirtualArchitecture.hpp

LIBS += -L$$DESTDIR -lhadesvm-util
