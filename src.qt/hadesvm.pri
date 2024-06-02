QT += core gui widgets xml

CONFIG += c++latest strict_c++
QMAKE_CXXFLAGS += -ansi -Wall -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Weffc++ -Wextra -Wformat=2 -Winit-self -Wmissing-noreturn -Wno-long-long -Wold-style-cast -Woverloaded-virtual -Wpointer-arith -Wredundant-decls -Wsign-promo -Wswitch-default -Wswitch-enum -Wundef -Wunused -Wconversion -Wformat-security -Wformat-y2k -Wmissing-declarations -Wmissing-field-initializers -Wmissing-include-dirs -Wpacked -Wshadow -Wvariadic-macros -Wwrite-strings -fno-strict-aliasing -Wno-c++0x-compat
QMAKE_LFLAGS += -Wl,--no-undefined

INCLUDEPATH += ..

CONFIG(debug, debug|release) {
    DESTDIR = ../../../bin.qt/Debug
} else {
    DESTDIR = ../../../bin.qt/Release
}
