include(../hadesvm.pri)

TEMPLATE = lib
DEFINES += HADESVM_UTIL_LIBRARY

SOURCES += \
    Exceptions.cpp \
    PluginManager.cpp

HEADERS += \
    API.hpp \
    Exceptions.hpp \
    FromString.hpp \
    Linkage.hpp \
    Macros.hpp \
    Plugin.hpp \
    PluginManager.hpp \
    StockObject.hpp \
    ToString.hpp
