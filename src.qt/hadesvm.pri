QT += core gui widgets xml network

CONFIG += c++latest strict_c++
linux-g++ {
    QMAKE_CXXFLAGS += -ansi -Wall -Wcast-align -Wcast-qual -Weffc++ -Wextra -Wformat=2 -Winit-self -Wmissing-noreturn -Wno-long-long -Wold-style-cast -Woverloaded-virtual -Wpointer-arith -Wredundant-decls -Wsign-promo -Wswitch-default -Wswitch-enum -Wundef -Wunused -Wconversion -Wformat-security -Wformat-y2k -Wmissing-declarations -Wmissing-field-initializers -Wmissing-include-dirs -Wpacked -Wshadow -Wvariadic-macros -Wwrite-strings -fno-strict-aliasing -Wno-c++0x-compat
    QMAKE_LFLAGS += -Wl,--no-undefined

    QMAKE_CXXFLAGS_RELEASE -= -O0
    QMAKE_CXXFLAGS_RELEASE -= -O1
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE += -O3
}
win32-msvc {
    #   TODO try removing /wd4710 /wd4711 when switching to MSVC2022
    QMAKE_CXXFLAGS += /GS- /std:c++latest /Za /Zc:rvalueCast /Wall /WX /wd4710 /wd4711
}

INCLUDEPATH += ..

CONFIG(debug, debug|release) {
    DESTDIR = ../../../bin.qt/Debug
} else {
    DESTDIR = ../../../bin.qt/Release
}
