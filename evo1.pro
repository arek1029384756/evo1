######################################################################
# evo1.pro
######################################################################

QT += widgets

QMAKE_PROJECT_NAME = evo1

INCLUDEPATH += .
QMAKE_CXXFLAGS += -Wextra -pedantic -std=c++11

# Input
HEADERS += window.h
SOURCES += main.cpp window.cpp
