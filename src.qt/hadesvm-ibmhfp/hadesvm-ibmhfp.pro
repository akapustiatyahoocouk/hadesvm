include(../hadesvm.pri)

TEMPLATE = lib
DEFINES += HADESVM_IBMHFP_LIBRARY

SOURCES += \
    Hfp.cpp \
    HfpReal32.cpp \
    HfpReal64.cpp

HEADERS += \
    API.hpp \
    Hfp.hpp \
    Linkage.hpp \
    Real32.hpp \
    Real64.hpp
