include(../hadesvm.pri)

TEMPLATE = lib
DEFINES += HADESVM_IEEE754_LIBRARY

SOURCES += \
    Environment.cpp \
    FpValue.cpp \
    Real21.cpp \
    Real32.cpp \
    Real64.cpp

HEADERS += \
    API.hpp \
    Classes.hpp \
    Environment.hpp \
    FpValue.hxx \
    Linkage.hpp \
    Real21.hpp \
    Real32.hpp \
    Real64.hpp

LIBS += -L$$DESTDIR -lhadesvm-util
