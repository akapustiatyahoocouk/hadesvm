include(../hadesvm.pri)

TEMPLATE = lib
DEFINES += HADESVM_KERNEL_LIBRARY

SOURCES += \
    Architecture.cpp \
    Atom.cpp \
    Device.cpp \
    DeviceManagerProcess.cpp \
    Kernel.SystemCalls.cpp \
    Kernel.Type.cpp \
    Kernel.cpp \
    KernelEditor.cpp \
    LocalNode.cpp \
    Message.cpp \
    MountedFolderDialog.cpp \
    NativeThread.SystemCalls.cpp \
    NativeThread.cpp \
    Node.cpp \
    Object.cpp \
    PhysicalDevice.cpp \
    Plugins.cpp \
    Process.cpp \
    Server.cpp \
    Service.cpp \
    SystemInterfaces.cpp \
    SystemServices.cpp \
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
    SystemProcesses.hpp \
    SystemServices.hpp

LIBS += -L$$DESTDIR -lhadesvm-core -lhadesvm-util

FORMS += \
    KernelEditor.ui \
    MountedFolderDialog.ui
