include(../hadesvm.pri)

TEMPLATE = lib
DEFINES += HADESVM_CEREON_LIBRARY

SOURCES += \
    Alu.cpp \
    Architecture.cpp \
    Plugins.cpp

HEADERS += \
    API.hpp \
    Alu.hpp \
    Architecture.hpp \
    Classes.hpp \
    Linkage.hpp

LIBS += -L$$DESTDIR -lhadesvm-ieee754 -lhadesvm-core -lhadesvm-util
