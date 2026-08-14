QT += core gui serialbus widgets testlib serialbus

CONFIG += c++11

INCLUDEPATH += ../

SOURCES += \
    test/tst_lfqueue.cpp \
    test/main.cpp \
    test/tst_cancon.cpp \
    src/connections/canconfactory.cpp \
    src/connections/canconnection.cpp \
    src/connections/gvretserial.cpp \
    src/connections/socketcan.cpp \
    src/connections/canbus.cpp

#HEADERS += \
#    /utils/lfqueue.h

target.path= .
INSTALLS += target

HEADERS += \
    test/tst_lfqueue.h \
    test/tst_cancon.h \
    /connections/canconconst.h \
    /connections/canconfactory.h \
    /connections/canconnection.h \
    /connections/gvretserial.h \
    /connections/socketcan.h \
    /connections/canbus.h
