include(../hadesvm.pri)

TEMPLATE = lib
DEFINES += HADESVM_KERNEL_LIBRARY

SOURCES += \
    Architecture.cpp \
    Device.cpp \
    DeviceManagerMainThread.cpp \
    Kernel.SystemCalls.cpp \
    Kernel.Type.cpp \
    Kernel.cpp \
    KernelEditor.cpp \
    LocalNode.cpp \
    MountedFolderDialog.cpp \
    NativeThread.SystemCalls.cpp \
    NativeThread.cpp \
    Node.cpp \
    Object.cpp \
    PhysicalDevice.cpp \
    Plugins.cpp \
    Process.cpp \
    SystemInterfaces.cpp \
    Thread.cpp

HEADERS += \
    API.hpp \
    Architecture.hpp \
    Classes.hpp \
    ComponentInterfaces.hpp \
    KErrno.hpp \
    Kernel.hpp \
    KernelEditor.hpp \
    Linkage.hpp \
    MountedFolderDialog.hpp \
    Objects.hpp \
    StorageDevices.hpp \
    SystemInterfaces.hpp \
    SystemProcesses.hpp

LIBS += -L$$DESTDIR -lhadesvm-core -lhadesvm-util

FORMS += \
    KernelEditor.ui \
    MountedFolderDialog.ui
