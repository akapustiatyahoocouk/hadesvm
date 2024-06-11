include(../hadesvm.pri)

TEMPLATE = lib
DEFINES += HADESVM_CEREON_LIBRARY

SOURCES += \
    Alu.cpp \
    Architectures.cpp \
    MemoryBus.cpp \
    Plugins.cpp \
    ResidentMemoryBlock.cpp \
    ResidentRamBlock.cpp \
    ResidentRamBlockEditor.cpp

HEADERS += \
    API.hpp \
    Alu.hpp \
    Architectures.hpp \
    Classes.hpp \
    Linkage.hpp \
    Memory.hpp \
    ProcessorCore.hpp \
    ResidentRamBlockEditor.hpp

LIBS += -L$$DESTDIR -lhadesvm-ieee754 -lhadesvm-core -lhadesvm-util

FORMS += \
    ResidentRamBlockEditor.ui
