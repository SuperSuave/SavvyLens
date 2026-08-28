QT += core gui serialbus widgets testlib serialbus

CONFIG += c++11

INCLUDEPATH += ../src ../

SOURCES += \
    tst_lfqueue.cpp \
    main.cpp \
    tst_cancon.cpp \
    ../src/connections/canconfactory.cpp \
    ../src/connections/canconnection.cpp \
    ../src/connections/gvretserial.cpp \
    ../src/connections/socketcan.cpp \
    ../src/connections/canbus.cpp

HEADERS += \
    tst_lfqueue.h \
    tst_cancon.h \
    ../src/connections/canconconst.h \
    ../src/connections/canconfactory.h \
    ../src/connections/canconnection.h \
    ../src/connections/gvretserial.h \
    ../src/connections/socketcan.h \
    ../src/connections/canbus.h
