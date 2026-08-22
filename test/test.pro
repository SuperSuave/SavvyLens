QT += core gui serialbus testlib

CONFIG += c++11

PROJECT_ROOT = $$clean_path($$PWD/..)

INCLUDEPATH += \
    $$PROJECT_ROOT \
    $$PROJECT_ROOT/src

DEPENDPATH += $$PROJECT_ROOT/src

SOURCES += \
    tst_analysissession.cpp \
    tst_candidateanalysis.cpp \
    tst_discretestateanalysis.cpp \
    tst_frameaggregatestore.cpp \
    tst_framecomparison.cpp \
    tst_framehistory.cpp \
    tst_lfqueue.cpp \
    tst_payloaddiff.cpp \
    tst_rangestatistics.cpp \
    tst_stateexplorerpresentation.cpp \
    tst_temporalanalysis.cpp \
    tst_transitionanalysis.cpp \
    main.cpp \
    $$PROJECT_ROOT/src/app/stateexplorerpresentation.cpp \
    $$PROJECT_ROOT/src/analysis/analysismarker.cpp \
    $$PROJECT_ROOT/src/analysis/analysismarkerstore.cpp \
    $$PROJECT_ROOT/src/analysis/analysissession.cpp \
    $$PROJECT_ROOT/src/analysis/candidateanalysis.cpp \
    $$PROJECT_ROOT/src/analysis/discretestateanalysis.cpp \
    $$PROJECT_ROOT/src/analysis/frameaggregatestore.cpp \
    $$PROJECT_ROOT/src/analysis/framecomparison.cpp \
    $$PROJECT_ROOT/src/analysis/framehistory.cpp \
    $$PROJECT_ROOT/src/analysis/payloaddiff.cpp \
    $$PROJECT_ROOT/src/analysis/rangestatistics.cpp \
    $$PROJECT_ROOT/src/analysis/selectioncontext.cpp \
    $$PROJECT_ROOT/src/analysis/temporalanalysis.cpp \
    $$PROJECT_ROOT/src/analysis/transitionanalysis.cpp

HEADERS += \
    tst_analysissession.h \
    tst_candidateanalysis.h \
    tst_discretestateanalysis.h \
    tst_frameaggregatestore.h \
    tst_framecomparison.h \
    tst_framehistory.h \
    tst_lfqueue.h \
    tst_payloaddiff.h \
    tst_rangestatistics.h \
    tst_stateexplorerpresentation.h \
    tst_temporalanalysis.h \
    tst_transitionanalysis.h \
    $$PROJECT_ROOT/src/app/stateexplorerpresentation.h \
    $$PROJECT_ROOT/src/analysis/analysismarker.h \
    $$PROJECT_ROOT/src/analysis/analysismarkerstore.h \
    $$PROJECT_ROOT/src/analysis/analysissession.h \
    $$PROJECT_ROOT/src/analysis/candidateanalysis.h \
    $$PROJECT_ROOT/src/analysis/discretestateanalysis.h \
    $$PROJECT_ROOT/src/analysis/frameaggregatestore.h \
    $$PROJECT_ROOT/src/analysis/framecomparison.h \
    $$PROJECT_ROOT/src/analysis/framehistory.h \
    $$PROJECT_ROOT/src/analysis/payloaddiff.h \
    $$PROJECT_ROOT/src/analysis/rangestatistics.h \
    $$PROJECT_ROOT/src/analysis/selectioncontext.h \
    $$PROJECT_ROOT/src/analysis/temporalanalysis.h \
    $$PROJECT_ROOT/src/analysis/transitionanalysis.h \
    $$PROJECT_ROOT/src/can/can_structs.h

target.path = .
INSTALLS += target