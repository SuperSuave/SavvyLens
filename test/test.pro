QT += core gui serialbus serialport widgets testlib network

CONFIG += c++17

INCLUDEPATH += ../src ../

SOURCES += \
    tst_lfqueue.cpp \
    main.cpp \
    tst_cancon.cpp \
    tst_framefileio.cpp \
    ../src/connections/canconfactory.cpp \
    ../src/connections/canconmanager.cpp \
    ../src/connections/canconnection.cpp \
    ../src/connections/gvretserial.cpp \
    ../src/connections/lawicel_serial.cpp \
    ../src/connections/serialbusconnection.cpp \
    ../src/connections/socketcand.cpp \
    ../src/connections/canserver.cpp \
    ../src/connections/canlogserver.cpp \
    ../src/connections/mqtt_bus.cpp \
    ../src/connections/canbus.cpp \
    ../src/mqtt/qmqtt_client_p.cpp \
    ../src/mqtt/qmqtt_client.cpp \
    ../src/mqtt/qmqtt_frame.cpp \
    ../src/mqtt/qmqtt_message.cpp \
    ../src/mqtt/qmqtt_network.cpp \
    ../src/mqtt/qmqtt_router.cpp \
    ../src/mqtt/qmqtt_routesubscription.cpp \
    ../src/mqtt/qmqtt_socket.cpp \
    ../src/mqtt/qmqtt_ssl_socket.cpp \
    ../src/mqtt/qmqtt_timer.cpp \
    ../src/mqtt/qmqtt_websocket.cpp \
    ../src/mqtt/qmqtt_websocketiodevice.cpp \
    ../src/third_party/simplecrypt.cpp \
    ../src/io/framefileio.cpp \
    ../src/io/formats/blfhandler.cpp \
    ../src/io/formats/pcaplite.cpp \
    ../src/common/utility.cpp \
    ../src/bookmarks/bookmarkmanager.cpp \
    ../src/can/canfilter.cpp

HEADERS += \
    tst_lfqueue.h \
    tst_cancon.h \
    tst_framefileio.h \
    ../src/connections/canconconst.h \
    ../src/connections/canconfactory.h \
    ../src/connections/canconmanager.h \
    ../src/connections/canconnection.h \
    ../src/connections/gvretserial.h \
    ../src/connections/lawicel_serial.h \
    ../src/connections/serialbusconnection.h \
    ../src/connections/socketcand.h \
    ../src/connections/canserver.h \
    ../src/connections/canlogserver.h \
    ../src/connections/mqtt_bus.h \
    ../src/connections/canbus.h \
    ../src/mqtt/qmqtt_client_p.h \
    ../src/mqtt/qmqtt_client.h \
    ../src/mqtt/qmqtt_frame.h \
    ../src/mqtt/qmqtt_global.h \
    ../src/mqtt/qmqtt_message_p.h \
    ../src/mqtt/qmqtt_message.h \
    ../src/mqtt/qmqtt_network_p.h \
    ../src/mqtt/qmqtt_networkinterface.h \
    ../src/mqtt/qmqtt_routedmessage.h \
    ../src/mqtt/qmqtt_router.h \
    ../src/mqtt/qmqtt_routesubscription.h \
    ../src/mqtt/qmqtt_socket_p.h \
    ../src/mqtt/qmqtt_socketinterface.h \
    ../src/mqtt/qmqtt_ssl_socket_p.h \
    ../src/mqtt/qmqtt_timer_p.h \
    ../src/mqtt/qmqtt_timerinterface.h \
    ../src/mqtt/qmqtt_websocket_p.h \
    ../src/mqtt/qmqtt_websocketiodevice_p.h \
    ../src/mqtt/qmqtt.h \
    ../src/third_party/simplecrypt.h \
    ../src/io/framefileio.h \
    ../src/io/formats/blfhandler.h \
    ../src/io/formats/pcaplite.h \
    ../src/common/utility.h \
    ../src/bookmarks/bookmarkmanager.h \
    ../src/can/canfilter.h

target.path = .
INSTALLS += target
