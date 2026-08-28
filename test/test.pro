QT += core gui serialbus serialport widgets testlib network

CONFIG += c++17

SOURCES += \

HEADERS += \

target.path = .
INSTALLS += target
PROJECT_ROOT = $$clean_path($$PWD/..)

INCLUDEPATH += \
    $$PROJECT_ROOT \
    $$PROJECT_ROOT/src

DEPENDPATH += $$PROJECT_ROOT/src

SOURCES += \
    main.cpp \
    tst_analysissession.cpp \
    tst_cancon.cpp \
    tst_candidateanalysis.cpp \
    tst_discretestateanalysis.cpp \
    tst_frameaggregatestore.cpp \
    tst_framecomparison.cpp \
    tst_framefileio.cpp \
    tst_framehistory.cpp \
    tst_lfqueue.cpp \
    tst_payloaddiff.cpp \
    tst_rangestatistics.cpp \
    tst_stateexplorerpresentation.cpp \
    tst_temporalanalysis.cpp \
    tst_transitionanalysis.cpp \
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
    $$PROJECT_ROOT/src/analysis/transitionanalysis.cpp \
    $$PROJECT_ROOT/src/app/stateexplorerpresentation.cpp \
    $$PROJECT_ROOT/src/bookmarks/bookmarkmanager.cpp \
    $$PROJECT_ROOT/src/can/canfilter.cpp \
    $$PROJECT_ROOT/src/common/savvylenspaths.cpp \
    $$PROJECT_ROOT/src/common/utility.cpp \
    $$PROJECT_ROOT/src/connections/canbus.cpp \
    $$PROJECT_ROOT/src/connections/canconfactory.cpp \
    $$PROJECT_ROOT/src/connections/canconmanager.cpp \
    $$PROJECT_ROOT/src/connections/canconnection.cpp \
    $$PROJECT_ROOT/src/connections/canlogserver.cpp \
    $$PROJECT_ROOT/src/connections/canserver.cpp \
    $$PROJECT_ROOT/src/connections/gvretserial.cpp \
    $$PROJECT_ROOT/src/connections/lawicel_serial.cpp \
    $$PROJECT_ROOT/src/connections/mqtt_bus.cpp \
    $$PROJECT_ROOT/src/connections/serialbusconnection.cpp \
    $$PROJECT_ROOT/src/connections/socketcand.cpp \
    $$PROJECT_ROOT/src/io/formats/blfhandler.cpp \
    $$PROJECT_ROOT/src/io/formats/pcaplite.cpp \
    $$PROJECT_ROOT/src/io/framefileio.cpp \
    $$PROJECT_ROOT/src/mqtt/qmqtt_client_p.cpp \
    $$PROJECT_ROOT/src/mqtt/qmqtt_client.cpp \
    $$PROJECT_ROOT/src/mqtt/qmqtt_frame.cpp \
    $$PROJECT_ROOT/src/mqtt/qmqtt_message.cpp \
    $$PROJECT_ROOT/src/mqtt/qmqtt_network.cpp \
    $$PROJECT_ROOT/src/mqtt/qmqtt_router.cpp \
    $$PROJECT_ROOT/src/mqtt/qmqtt_routesubscription.cpp \
    $$PROJECT_ROOT/src/mqtt/qmqtt_socket.cpp \
    $$PROJECT_ROOT/src/mqtt/qmqtt_ssl_socket.cpp \
    $$PROJECT_ROOT/src/mqtt/qmqtt_timer.cpp \
    $$PROJECT_ROOT/src/mqtt/qmqtt_websocket.cpp \
    $$PROJECT_ROOT/src/mqtt/qmqtt_websocketiodevice.cpp \
    $$PROJECT_ROOT/src/third_party/simplecrypt.cpp

HEADERS += \
    tst_analysissession.h \
    tst_cancon.h \
    tst_cancon.h \
    tst_candidateanalysis.h \
    tst_discretestateanalysis.h \
    tst_frameaggregatestore.h \
    tst_framecomparison.h \
    tst_framefileio.h \
    tst_framehistory.h \
    tst_lfqueue.h \
    tst_lfqueue.h \
    tst_payloaddiff.h \
    tst_rangestatistics.h \
    tst_stateexplorerpresentation.h \
    tst_temporalanalysis.h \
    tst_transitionanalysis.h \
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
    $$PROJECT_ROOT/src/app/stateexplorerpresentation.h \
    $$PROJECT_ROOT/src/bookmarks/bookmarkmanager.h \
    $$PROJECT_ROOT/src/can/can_structs.h \
    $$PROJECT_ROOT/src/can/canfilter.h \
    $$PROJECT_ROOT/src/common/savvylenspaths.h \
    $$PROJECT_ROOT/src/common/utility.h \
    $$PROJECT_ROOT/src/connections/canbus.h \
    $$PROJECT_ROOT/src/connections/canbus.h \
    $$PROJECT_ROOT/src/connections/canconconst.h \
    $$PROJECT_ROOT/src/connections/canconconst.h \
    $$PROJECT_ROOT/src/connections/canconfactory.h \
    $$PROJECT_ROOT/src/connections/canconfactory.h \
    $$PROJECT_ROOT/src/connections/canconmanager.h \
    $$PROJECT_ROOT/src/connections/canconnection.h \
    $$PROJECT_ROOT/src/connections/canconnection.h \
    $$PROJECT_ROOT/src/connections/canlogserver.h \
    $$PROJECT_ROOT/src/connections/canserver.h \
    $$PROJECT_ROOT/src/connections/gvretserial.h \
    $$PROJECT_ROOT/src/connections/gvretserial.h \
    $$PROJECT_ROOT/src/connections/lawicel_serial.h \
    $$PROJECT_ROOT/src/connections/mqtt_bus.h \
    $$PROJECT_ROOT/src/connections/serialbusconnection.h \
    $$PROJECT_ROOT/src/connections/socketcand.h \
    $$PROJECT_ROOT/src/io/formats/blfhandler.h \
    $$PROJECT_ROOT/src/io/formats/pcaplite.h \
    $$PROJECT_ROOT/src/io/framefileio.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_client_p.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_client.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_frame.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_global.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_message_p.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_message.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_network_p.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_networkinterface.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_routedmessage.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_router.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_routesubscription.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_socket_p.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_socketinterface.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_ssl_socket_p.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_timer_p.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_timerinterface.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_websocket_p.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt_websocketiodevice_p.h \
    $$PROJECT_ROOT/src/mqtt/qmqtt.h \
    $$PROJECT_ROOT/src/third_party/simplecrypt.h

target.path = .
INSTALLS += target
