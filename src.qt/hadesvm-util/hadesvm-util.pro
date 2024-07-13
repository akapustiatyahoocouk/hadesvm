include(../hadesvm.pri)

TEMPLATE = lib
DEFINES += HADESVM_UTIL_LIBRARY

SOURCES += \
    ByteOrder.cpp \
    Exceptions.cpp \
    FromString.cpp \
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
    Sync.hpp \
    ToString.hpp
