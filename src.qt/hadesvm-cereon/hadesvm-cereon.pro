include(../hadesvm.pri)

TEMPLATE = lib
DEFINES += HADESVM_CEREON_LIBRARY

SOURCES += \
    Alu.cpp \
    Architectures.cpp \
    Cereon1P1B.cpp \
    Cmos1.cpp \
    Cmos1Editor.cpp \
    Fdc1Controller.cpp \
    Fdc1ControllerEditor.cpp \
    Fdc1FloppyDrive.cpp \
    Fdc1FloppyDriveEditor.cpp \
    Fdc1FloppyDriveStatusBarWidget.cpp \
    Features.cpp \
    IoBus.cpp \
    IoBusEditor.cpp \
    IoPort.cpp \
    Kis1Controller.cpp \
    Kis1ControllerEditor.cpp \
    Kis1Keyboard.cpp \
    Kis1KeyboardEditor.cpp \
    Kis1KeyboardLayout.cpp \
    Kis1UnitedKingdomExtendedKeyboardLayout.cpp \
    Kis1UnitedKingdomKeyboardLayout.cpp \
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
    Templates.cpp \
    Vds1Controller.Compartment.cpp \
    Vds1Controller.VideoSignal.cpp \
    Vds1Controller.cpp \
    Vds1ControllerEditor.cpp \
    Vds1Display.cpp \
    Vds1DisplayEditor.cpp \
    Vds1DisplayWidget.cpp

HEADERS += \
    API.hpp \
    Alu.hpp \
    Architectures.hpp \
    Classes.hpp \
    Cmos1.hpp \
    Cmos1Editor.hpp \
    Fdc1.hpp \
    Fdc1ControllerEditor.hpp \
    Fdc1FloppyDriveEditor.hpp \
    Fdc1FloppyDriveStatusBarWidget.hpp \
    Io.hpp \
    IoBusEditor.hpp \
    Kis1.hpp \
    Kis1ControllerEditor.hpp \
    Kis1KeyboardEditor.hpp \
    Linkage.hpp \
    Memory.hpp \
    MemoryBusEditor.hpp \
    Processor.hpp \
    ProcessorCore.hpp \
    ProcessorEditor.hpp \
    ResidentRamUnitEditor.hpp \
    ResidentRomUnitEditor.hpp \
    Templates.hpp \
    Vds1.hpp \
    Vds1ControllerEditor.hpp \
    Vds1DisplayEditor.hpp \
    Vds1DisplayWidget.hpp

LIBS += -L$$DESTDIR -lhadesvm-ieee754 -lhadesvm-core -lhadesvm-util

FORMS += \
    Cmos1Editor.ui \
    Fdc1ControllerEditor.ui \
    Fdc1FloppyDriveEditor.ui \
    Fdc1FloppyDriveStatusBarWidget.ui \
    IoBusEditor.ui \
    Kis1ControllerEditor.ui \
    Kis1KeyboardEditor.ui \
    MemoryBusEditor.ui \
    ProcessorEditor.ui \
    ResidentRamUnitEditor.ui \
    ResidentRomUnitEditor.ui \
    Vds1ControllerEditor.ui \
    Vds1DisplayEditor.ui \
    Vds1DisplayWidget.ui

RESOURCES += \
    Resources.qrc
