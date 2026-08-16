QT += core gui serialbus testlib

CONFIG += c++11

PROJECT_ROOT = $$clean_path($$PWD/..)

INCLUDEPATH += \
    $$PROJECT_ROOT \
    $$PROJECT_ROOT/src

DEPENDPATH += $$PROJECT_ROOT/src

SOURCES += \
    tst_lfqueue.cpp \
    tst_frameaggregatestore.cpp \
    main.cpp \
    $$PROJECT_ROOT/src/analysis/frameaggregatestore.cpp

HEADERS += \
    tst_lfqueue.h \
    tst_frameaggregatestore.h \
    $$PROJECT_ROOT/src/analysis/frameaggregatestore.h \
    $$PROJECT_ROOT/src/can/can_structs.h

target.path = .
INSTALLS += target