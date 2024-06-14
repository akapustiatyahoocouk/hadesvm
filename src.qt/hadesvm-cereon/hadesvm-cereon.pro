include(../hadesvm.pri)

TEMPLATE = lib
DEFINES += HADESVM_CEREON_LIBRARY

SOURCES += \
    Alu.cpp \
    Architectures.cpp \
    Cereon1P1B.cpp \
    Cmos1.cpp \
    Features.cpp \
    IoBus.cpp \
    IoBusEditor.cpp \
    IoPort.cpp \
    MemoryBus.cpp \
    MemoryBusEditor.cpp \
    Plugins.cpp \
    Processor.cpp \
    ProcessorCore.Base.cpp \
    ProcessorCore.FloatingPoint.cpp \
    ProcessorCore.cpp \
    ProcessorEditor.cpp \
    ResidentMemoryUnit.cpp \
    ResidentRamUnit.cpp \
    ResidentRamUnitEditor.cpp \
    ResidentRomUnit.cpp \
    ResidentRomUnitEditor.cpp

HEADERS += \
    API.hpp \
    Alu.hpp \
    Architectures.hpp \
    Classes.hpp \
    Cmos1.hpp \
    Io.hpp \
    IoBusEditor.hpp \
    Linkage.hpp \
    Memory.hpp \
    MemoryBusEditor.hpp \
    Processor.hpp \
    ProcessorCore.hpp \
    ProcessorEditor.hpp \
    ResidentRamUnitEditor.hpp \
    ResidentRomUnitEditor.hpp

LIBS += -L$$DESTDIR -lhadesvm-ieee754 -lhadesvm-core -lhadesvm-util

FORMS += \
    IoBusEditor.ui \
    MemoryBusEditor.ui \
    ProcessorEditor.ui \
    ResidentRamUnitEditor.ui \
    ResidentRomUnitEditor.ui
