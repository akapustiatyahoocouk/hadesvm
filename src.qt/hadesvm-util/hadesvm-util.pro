include(../hadesvm.pri)

TEMPLATE = lib
DEFINES += HADESVM_UTIL_LIBRARY

SOURCES += \
    Exceptions.cpp \
    Math.cpp \
    PluginManager.cpp \
    ToString.cpp

HEADERS += \
    API.hpp \
    ByteOrder.hpp \
    Exceptions.hpp \
    FromString.hpp \
    Linkage.hpp \
    Macros.hpp \
    Math.hpp \
    Plugin.hpp \
    PluginManager.hpp \
    StockObject.hpp \
    ToString.hpp
