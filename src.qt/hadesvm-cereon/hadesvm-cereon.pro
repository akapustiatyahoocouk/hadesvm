include(../hadesvm.pri)

TEMPLATE = lib
DEFINES += HADESVM_CEREON_LIBRARY

SOURCES += \
    Alu.cpp \
    Architectures.cpp \
    Cereon1P1B.cpp \
    Cmos1.cpp \
    Cmos1Editor.cpp \
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
    ResidentRomUnitEditor.cpp \
    Vds1Controller.Compartment.cpp \
    Vds1Controller.VideoSignal.cpp \
    Vds1Controller.cpp \
    Vds1ControllerEditor.cpp \
    Vds1Display.cpp \
    Vds1DisplayEditor.cpp

HEADERS += \
    API.hpp \
    Alu.hpp \
    Architectures.hpp \
    Classes.hpp \
    Cmos1.hpp \
    Cmos1Editor.hpp \
    Io.hpp \
    IoBusEditor.hpp \
    Linkage.hpp \
    Memory.hpp \
    MemoryBusEditor.hpp \
    Processor.hpp \
    ProcessorCore.hpp \
    ProcessorEditor.hpp \
    ResidentRamUnitEditor.hpp \
    ResidentRomUnitEditor.hpp \
    Vds1.hpp \
    Vds1ControllerEditor.hpp \
    Vds1DisplayEditor.hpp

LIBS += -L$$DESTDIR -lhadesvm-ieee754 -lhadesvm-core -lhadesvm-util

FORMS += \
    Cmos1Editor.ui \
    IoBusEditor.ui \
    MemoryBusEditor.ui \
    ProcessorEditor.ui \
    ResidentRamUnitEditor.ui \
    ResidentRomUnitEditor.ui \
    Vds1ControllerEditor.ui \
    Vds1DisplayEditor.ui
