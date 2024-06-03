include(../hadesvm.pri)

TEMPLATE = lib
DEFINES += HADESVM_KERNEL_LIBRARY

SOURCES += \
    Architecture.cpp \
    Kernel.Type.cpp \
    Kernel.cpp \
    KernelEditor.cpp \
    MountedFolderDialog.cpp \
    Plugins.cpp

HEADERS += \
    API.hpp \
    Architecture.hpp \
    Classes.hpp \
    Kernel.hpp \
    KernelEditor.hpp \
    Linkage.hpp \
    MountedFolderDialog.hpp \
    Object.hpp \
    Process.hpp \
    Thread.hpp

LIBS += -L$$DESTDIR -lhadesvm-core -lhadesvm-util

FORMS += \
    KernelEditor.ui \
    MountedFolderDialog.ui
