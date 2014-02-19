# Additional import path used to resolve QML modules in the Creator\'s code model
QML_IMPORT_PATH =

QMAKE_CXXFLAGS += -std=gnu++11
OTHER_FILES += simulator.qml

# The .cpp file which was generated for your project. Feel free to hack it.
# Input
HEADERS += cpu.h \
    memory.h \
    memoryViewModel.h \
    programViewModel.h
SOURCES += cpu.cpp \
    memory.cpp \
    simulator.cpp \
    memoryViewModel.cpp \
    programViewModel.cpp

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()
