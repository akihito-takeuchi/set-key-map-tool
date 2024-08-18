TEMPLATE = app
TARGET = SetKeyMap
INCLUDEPATH += .
QT += widgets
DEFINES += QT_DEPRECATED_WARNINGS
HEADERS += \
        winutil.hpp \
        mainwindow.hpp \
        editkeymapdialog.hpp \
        keyboarddefs.hpp
SOURCES += \
        main.cpp \
        winutil.cpp \
        mainwindow.cpp \
        editkeymapdialog.cpp \
        keyboarddefs.cpp
