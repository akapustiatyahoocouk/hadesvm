include(../hadesvm.pri)

TEMPLATE = lib
DEFINES += HADESVM_CEREON_LIBRARY

SOURCES += \
    Alu.cpp \
    Architectures.cpp \
    Cereon1P1B.cpp \
    Features.cpp \
    IoBus.cpp \
    IoPort.cpp \
    MemoryBus.cpp \
    Plugins.cpp \
    Processor.cpp \
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
    Io.hpp \
    Linkage.hpp \
    Memory.hpp \
    Processor.hpp \
    ProcessorCore.hpp \
    ProcessorEditor.hpp \
    ResidentRamUnitEditor.hpp \
    ResidentRomUnitEditor.hpp

LIBS += -L$$DESTDIR -lhadesvm-ieee754 -lhadesvm-core -lhadesvm-util

FORMS += \
    ProcessorEditor.ui \
    ResidentRamUnitEditor.ui \
    ResidentRomUnitEditor.ui
