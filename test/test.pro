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
    tst_framecomparison.cpp \
    tst_framehistory.cpp \
    tst_payloaddiff.cpp \
    main.cpp \
    $$PROJECT_ROOT/src/analysis/frameaggregatestore.cpp \
    $$PROJECT_ROOT/src/analysis/framecomparison.cpp \
    $$PROJECT_ROOT/src/analysis/framehistory.cpp \
    $$PROJECT_ROOT/src/analysis/payloaddiff.cpp

HEADERS += \
    tst_lfqueue.h \
    tst_frameaggregatestore.h \
    tst_framecomparison.h \
    tst_framehistory.h \
    tst_payloaddiff.h \
    $$PROJECT_ROOT/src/analysis/frameaggregatestore.h \
    $$PROJECT_ROOT/src/analysis/framecomparison.h \
    $$PROJECT_ROOT/src/analysis/framehistory.h \
    $$PROJECT_ROOT/src/analysis/payloaddiff.h \
    $$PROJECT_ROOT/src/can/can_structs.h

target.path = .
INSTALLS += target