include(../hadesvm.pri)
QT -= core gui widgets xml network
win32-msvc {
    QMAKE_CXXFLAGS += /wd5045
}

CONFIG += cmdline

SOURCES += \
        append-line-action.cpp \
        command-line.cpp \
        create-directory-action.cpp \
        create-file-action.cpp \
        create-vfd-action.cpp \
        fat12-file-channel.cpp \
        fat12-file-system.cpp \
        file-system.cpp \
        list-directory-action.cpp \
        main.cpp \
        make-bootable-action.cpp \
        open-vfd-action.cpp \
        put-action.cpp \
        vfd-geometry.cpp \
        vfd.cpp

HEADERS += \
    actions.hpp \
    command-line.hpp \
    file-system.hpp \
    main.hpp \
    types.hpp \
    vfd.hpp

RESOURCES +=

DATADIR = ../../../src.qt/vfd-utils/data

#include "data/boot-sector-ibmpc-hades.bin.hxx"

bs1.target = $$DATADIR/boot-sector-ibmpc-hades.bin.hxx
bs1.depends = FORCE
bs1.commands = xxd -i $$DATADIR/boot-sector-ibmpc-hades.bin $$DATADIR/boot-sector-ibmpc-hades.bin.hxx

bs2.target = $$DATADIR/boot-sector-cereon-workstation-hades-be.bin.hxx
bs2.depends = FORCE
bs2.commands = xxd -i $$DATADIR/boot-sector-cereon-workstation-hades-be.bin $$DATADIR/boot-sector-cereon-workstation-hades-be.bin.hxx

bs3.target = $$DATADIR/boot-sector-cereon-workstation-hades-le.bin.hxx
bs3.depends = FORCE
bs3.commands = xxd -i $$DATADIR/boot-sector-cereon-workstation-hades-le.bin $$DATADIR/boot-sector-cereon-workstation-hades-le.bin.hxx

PRE_TARGETDEPS += $$DATADIR/boot-sector-cereon-workstation-hades-be.bin.hxx \
                  $$DATADIR/boot-sector-cereon-workstation-hades-le.bin.hxx \
                  $$DATADIR/boot-sector-ibmpc-hades.bin.hxx
QMAKE_EXTRA_TARGETS += bs1 bs2 bs3

DISTFILES += \
    data/boot-sector-cereon-workstation-hades-be.bin \
    data/boot-sector-cereon-workstation-hades-le.bin \
    data/boot-sector-ibmpc-hades.bin
