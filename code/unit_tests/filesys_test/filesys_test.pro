TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += USER_PROGRAM FILESYS_NEEDED NETWORK LINUX HOST_i386 FILESYS UNIT_TESTS private=public

QMAKE_CC = $$QMAKE_CXX
QMAKE_CFLAGS  = $$QMAKE_CXXFLAGS

INCLUDEPATH += $$PWD/../../bin \
           $$PWD/../../machine \
           $$PWD/../../filesys \
           $$PWD/../../network \
           $$PWD/../../threads \
           $$PWD/../../userprog \
           $$PWD/../mocks \
           $$PWD/../common \

SOURCES += $$PWD/*.cc \
           $$PWD/../../machine/*.cc \
           $$PWD/../../filesys/*.cc \
           $$PWD/../../network/*.cc \
           $$files($$PWD/../../threads/*.cc) \
           $$PWD/../../threads/*.S \
           $$PWD/../../userprog/*.cc \

SOURCES -= $$PWD/../../threads/main.cc
SOURCES -= $$PWD/../../userprog/exception.cc

HEADERS += $$PWD/../mocks/*.h \
           $$PWD/../../machine/*.h \
           $$PWD/../../filesys/*.h \
           $$PWD/../../network/*.h \
           $$PWD/../../threads/*.h \
           $$PWD/../../userprog/*.h \
           $$PWD/../../bin/noff.h \
           $$PWD/../common/*.h \

QMAKE_CXXFLAGS += -fpermissive


