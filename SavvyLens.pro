#-------------------------------------------------
#
# SavvyLens qmake project
#
#-------------------------------------------------

!versionAtLeast(QT_VERSION, 5.14.0) {
    error("Current version of Qt ($${QT_VERSION}) is too old; SavvyLens requires Qt 5.14 or newer.")
}

TARGET = SavvyLens
TEMPLATE = app

QT += \
    core \
    gui \
    help \
    network \
    opengl \
    printsupport \
    qml \
    quick \
    quickwidgets \
    serialbus \
    serialport \
    widgets

greaterThan(QT_MAJOR_VERSION, 5) {
    QT += openglwidgets
}

greaterThan(QT_MAJOR_VERSION, 5) {
    QT += openglwidgets
}

greaterThan(QT_MAJOR_VERSION, 5) {
    QT += openglwidgets
}

greaterThan(QT_MAJOR_VERSION, 5) {
    QT += openglwidgets
}

greaterThan(QT_MAJOR_VERSION, 5) {
    QT += openglwidgets
}

CONFIG += \
    c++17 \
    NO_UNIT_TESTS

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}

DEFINES += QCUSTOMPLOT_USE_OPENGL

INCLUDEPATH += \
    $$PWD \
    $$PWD/src

DEPENDPATH += $$PWD/src

#-------------------------------------------------
# macOS
#-------------------------------------------------

macx {
    QMAKE_INFO_PLIST = $$PWD/src/packaging/macos/Info.plist.template
    ICON = $$PWD/icons/SavvyLens.icns
}

#-------------------------------------------------
# Windows
#-------------------------------------------------

win32-msvc* {
    LIBS += opengl32.lib
    LIBS += Dbghelp.lib
}

win32-g++ {
    LIBS += -lopengl32
    LIBS += -ldbghelp
}

windows {
    RC_ICONS = $$PWD/icons/SavvyLens.ico
}

#-------------------------------------------------
# Linux / Unix installation
# Excludes macOS because it uses application bundles.
#-------------------------------------------------

unix:!macx {
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }

    target.path = $$PREFIX/bin

    shortcutfiles.files = $$PWD/src/packaging/linux/SavvyLens.desktop
    shortcutfiles.path = $$PREFIX/share/applications

    examplefiles.files = $$PWD/examples
    examplefiles.path = $$PREFIX/share/SavvyLens/examples

    iconfiles.files = $$PWD/icons
    iconfiles.path = $$PREFIX/share/SavvyLens/icons

    linuxiconfiles.files = $$PWD/icons/hicolor
    linuxiconfiles.path = $$PREFIX/share/icons

    helpfiles.files = $$files($$PWD/help/*)
    helpfiles.path = $$PREFIX/share/SavvyLens/help

    templatefiles.files = $$files($$PWD/src/scripting/templates/*)
    templatefiles.path = $$PREFIX/share/SavvyLens/templates

    INSTALLS += \
        examplefiles \
        helpfiles \
        iconfiles \
        linuxiconfiles \
        shortcutfiles \
        target \
        templatefiles

    DISTFILES += $$PWD/src/packaging/linux/SavvyLens.desktop
}

#-------------------------------------------------
# Source files
#-------------------------------------------------

SOURCES += \
    src/analysis/analysismarker.cpp \
    src/analysis/analysismarkerstore.cpp \
    src/analysis/analysissession.cpp \
    src/analysis/candidateanalysis.cpp \
    src/analysis/discretestateanalysis.cpp \
    src/analysis/frameaggregatestore.cpp \
    src/analysis/framecomparison.cpp \
    src/analysis/framehistory.cpp \
    src/analysis/livechangeexplorermodel.cpp \
    src/analysis/payloaddiff.cpp \
    src/analysis/rangestatistics.cpp \
    src/analysis/selectioncontext.cpp \
    src/analysis/temporalanalysis.cpp \
    src/analysis/transitionanalysis.cpp \
    src/app/helpwindow.cpp \
    src/app/livechangeexplorerhost.cpp \
    src/app/main.cpp \
    src/app/mainsettingsdialog.cpp \
    src/app/mainwindow.cpp \
    src/app/stateexplorerpresentation.cpp \
    src/app/studiohost.cpp \
    src/bookmarks/bookmarkmanager.cpp \
    src/bookmarks/bookmarkmanagerdialog.cpp \
    src/bus_protocols/isotp_handler.cpp \
    src/bus_protocols/j1939_handler.cpp \
    src/bus_protocols/uds_handler.cpp \
    src/can/canfilter.cpp \
    src/common/savvylenspaths.cpp \
    src/common/utility.cpp \
    src/connections/canbridgewindow.cpp \
    src/connections/canbus.cpp \
    src/connections/canconfactory.cpp \
    src/connections/canconmanager.cpp \
    src/connections/canconnection.cpp \
    src/connections/canconnectionmodel.cpp \
    src/connections/canlogserver.cpp \
    src/connections/canserver.cpp \
    src/connections/connectionwindow.cpp \
    src/connections/gvretserial.cpp \
    src/connections/lawicel_serial.cpp \
    src/connections/mqtt_bus.cpp \
    src/connections/newconnectiondialog.cpp \
    src/connections/serialbusconnection.cpp \
    src/connections/socketcand.cpp \
    src/dbc/dbc_classes.cpp \
    src/dbc/dbchandler.cpp \
    src/dbc/dbcloadsavewindow.cpp \
    src/dbc/dbcmaineditor.cpp \
    src/dbc/dbcmessageeditor.cpp \
    src/dbc/dbcnodeduplicateeditor.cpp \
    src/dbc/dbcnodeeditor.cpp \
    src/dbc/dbcnoderebaseeditor.cpp \
    src/dbc/dbcsignaleditor.cpp \
    src/dbc/dbcsignalselectortree.cpp \
    src/dbc/signalviewerwindow.cpp \
    src/frames/canframemodel.cpp \
    src/io/formats/blfhandler.cpp \
    src/io/formats/pcaplite.cpp \
    src/io/framefileio.cpp \
    src/mcp/mcpserver.cpp \
    src/mcp/mcptools_call.cpp \
    src/mcp/mcptools_list.cpp \
    src/mqtt/qmqtt_client_p.cpp \
    src/mqtt/qmqtt_client.cpp \
    src/mqtt/qmqtt_frame.cpp \
    src/mqtt/qmqtt_message.cpp \
    src/mqtt/qmqtt_network.cpp \
    src/mqtt/qmqtt_router.cpp \
    src/mqtt/qmqtt_routesubscription.cpp \
    src/mqtt/qmqtt_socket.cpp \
    src/mqtt/qmqtt_ssl_socket.cpp \
    src/mqtt/qmqtt_timer.cpp \
    src/mqtt/qmqtt_websocket.cpp \
    src/mqtt/qmqtt_websocketiodevice.cpp \
    src/playback/frameplaybackobject.cpp \
    src/playback/frameplaybackwindow.cpp \
    src/re/bisectwindow.cpp \
    src/re/bookmarkeventanalyzer.cpp \
    src/re/controlanalysisdialog.cpp \
    src/re/controlcandidatemodel.cpp \
    src/re/controlstatedetector.cpp \
    src/re/dbccomparatorwindow.cpp \
    src/re/discretestatewindow.cpp \
    src/re/filecomparatorwindow.cpp \
    src/re/flowviewwindow.cpp \
    src/re/frameinfowindow.cpp \
    src/re/fuzzingwindow.cpp \
    src/re/graphingwindow.cpp \
    src/re/isotp_interpreterwindow.cpp \
    src/re/newgraphdialog.cpp \
    src/re/rangestatewindow.cpp \
    src/re/sniffer/SnifferDelegate.cpp \
    src/re/sniffer/snifferitem.cpp \
    src/re/sniffer/sniffermodel.cpp \
    src/re/sniffer/snifferwindow.cpp \
    src/re/temporalgraphwindow.cpp \
    src/re/udsfirmwareuploaderwindow.cpp \
    src/re/udsscanwindow.cpp \
    src/scripting/jsedit.cpp \
    src/scripting/scriptcontainer.cpp \
    src/scripting/scriptingwindow.cpp \
    src/sender/framesenderobject.cpp \
    src/sender/framesenderwindow.cpp \
    src/sender/triggerdialog.cpp \
    src/themes/thememanager.cpp \
    src/third_party/qcustomplot.cpp \
    src/third_party/simplecrypt.cpp \
    src/utils/logger.cpp \
    src/widgets/candatagrid.cpp \
    src/widgets/filterutility.cpp \
    src/widgets/framebytedatadelegate.cpp \
    src/widgets/plotting/qcpaxistickerhex.cpp

#-------------------------------------------------
# Header files
#-------------------------------------------------

HEADERS += \
    config.h \
    src/analysis/analysismarker.h \
    src/analysis/analysismarkerstore.h \
    src/analysis/analysissession.h \
    src/analysis/candidateanalysis.h \
    src/analysis/discretestateanalysis.h \
    src/analysis/frameaggregatestore.h \
    src/analysis/framecomparison.h \
    src/analysis/framehistory.h \
    src/analysis/livechangeexplorermodel.h \
    src/analysis/payloaddiff.h \
    src/analysis/rangestatistics.h \
    src/analysis/selectioncontext.h \
    src/analysis/temporalanalysis.h \
    src/analysis/transitionanalysis.h \
    src/app/helpwindow.h \
    src/app/livechangeexplorerhost.h \
    src/app/mainsettingsdialog.h \
    src/app/mainwindow.h \
    src/app/stateexplorerpresentation.h \
    src/app/studiohost.h \
    src/bookmarks/bookmarkmanager.h \
    src/bookmarks/bookmarkmanagerdialog.h \
    src/bus_protocols/isotp_handler.h \
    src/bus_protocols/isotp_message.h \
    src/bus_protocols/j1939_handler.h \
    src/bus_protocols/uds_handler.h \
    src/can/can_structs.h \
    src/can/canfilter.h \
    src/common/savvylenspaths.h \
    src/common/utility.h \
    src/connections/canbridgewindow.h \
    src/connections/canbus.h \
    src/connections/canconconst.h \
    src/connections/canconfactory.h \
    src/connections/canconmanager.h \
    src/connections/canconnection.h \
    src/connections/canconnectionmodel.h \
    src/connections/canlogserver.h \
    src/connections/canserver.h \
    src/connections/connectionwindow.h \
    src/connections/gvretserial.h \
    src/connections/lawicel_serial.h \
    src/connections/mqtt_bus.h \
    src/connections/newconnectiondialog.h \
    src/connections/serialbusconnection.h \
    src/connections/socketcand.h \
    src/dbc/dbc_classes.h \
    src/dbc/dbchandler.h \
    src/dbc/dbcloadsavewindow.h \
    src/dbc/dbcmaineditor.h \
    src/dbc/dbcmessageeditor.h \
    src/dbc/dbcnodeduplicateeditor.h \
    src/dbc/dbcnodeeditor.h \
    src/dbc/dbcnoderebaseeditor.h \
    src/dbc/dbcsignaleditor.h \
    src/dbc/dbcsignalselectortree.h \
    src/dbc/signalviewerwindow.h \
    src/frames/canframemodel.h \
    src/io/formats/blfhandler.h \
    src/io/formats/pcaplite.h \
    src/io/framefileio.h \
    src/mcp/mcpserver.h \
    src/mqtt/qmqtt_client_p.h \
    src/mqtt/qmqtt_client.h \
    src/mqtt/qmqtt_frame.h \
    src/mqtt/qmqtt_global.h \
    src/mqtt/qmqtt_message_p.h \
    src/mqtt/qmqtt_message.h \
    src/mqtt/qmqtt_network_p.h \
    src/mqtt/qmqtt_networkinterface.h \
    src/mqtt/qmqtt_routedmessage.h \
    src/mqtt/qmqtt_router.h \
    src/mqtt/qmqtt_routesubscription.h \
    src/mqtt/qmqtt_socket_p.h \
    src/mqtt/qmqtt_socketinterface.h \
    src/mqtt/qmqtt_ssl_socket_p.h \
    src/mqtt/qmqtt_timer_p.h \
    src/mqtt/qmqtt_timerinterface.h \
    src/mqtt/qmqtt_websocket_p.h \
    src/mqtt/qmqtt_websocketiodevice_p.h \
    src/mqtt/qmqtt.h \
    src/playback/frameplaybackobject.h \
    src/playback/frameplaybackwindow.h \
    src/re/bisectwindow.h \
    src/re/bookmarkeventanalyzer.h \
    src/re/controlanalysisdialog.h \
    src/re/controlcandidatemodel.h \
    src/re/controlstatedetector.h \
    src/re/dbccomparatorwindow.h \
    src/re/discretestatewindow.h \
    src/re/filecomparatorwindow.h \
    src/re/flowviewwindow.h \
    src/re/frameinfowindow.h \
    src/re/fuzzingwindow.h \
    src/re/graphingwindow.h \
    src/re/isotp_interpreterwindow.h \
    src/re/newgraphdialog.h \
    src/re/rangestatewindow.h \
    src/re/sniffer/SnifferDelegate.h \
    src/re/sniffer/snifferitem.h \
    src/re/sniffer/sniffermodel.h \
    src/re/sniffer/snifferwindow.h \
    src/re/temporalgraphwindow.h \
    src/re/udsfirmwareuploaderwindow.h \
    src/re/udsscanwindow.h \
    src/scripting/jsedit.h \
    src/scripting/scriptcontainer.h \
    src/scripting/scriptingwindow.h \
    src/sender/can_trigger_structs.h \
    src/sender/framesenderobject.h \
    src/sender/framesenderwindow.h \
    src/sender/triggerdialog.h \
    src/themes/thememanager.h \
    src/third_party/qcustomplot.h \
    src/third_party/simplecrypt.h \
    src/utils/lfqueue.h \
    src/utils/logger.h \
    src/widgets/candatagrid.h \
    src/widgets/filterutility.h \
    src/widgets/framebytedatadelegate.h \
    src/widgets/plotting/qcpaxistickerhex.h

#-------------------------------------------------
# Qt Designer forms
#-------------------------------------------------

FORMS += \
    triggerdialog.ui \
    ui/bisectwindow.ui \
    ui/bookmarkmanagerdialog.ui \
    ui/canbridgewindow.ui \
    ui/candatagrid.ui \
    ui/connectionwindow.ui \
    ui/dbccomparatorwindow.ui \
    ui/dbcloadsavewindow.ui \
    ui/dbcmaineditor.ui \
    ui/dbcmessageeditor.ui \
    ui/dbcnodeduplicateeditor.ui \
    ui/dbcnodeeditor.ui \
    ui/dbcnoderebaseeditor.ui \
    ui/dbcsignaleditor.ui \
    ui/dbcsignalselectortree.ui \
    ui/discretestatewindow.ui \
    ui/filecomparatorwindow.ui \
    ui/flowviewwindow.ui \
    ui/frameinfowindow.ui \
    ui/frameplaybackwindow.ui \
    ui/framesenderwindow.ui \
    ui/fuzzingwindow.ui \
    ui/graphingwindow.ui \
    ui/helpwindow.ui \
    ui/isotp_interpreterwindow.ui \
    ui/mainsettingsdialog.ui \
    ui/mainwindow.ui \
    ui/newconnectiondialog.ui \
    ui/newgraphdialog.ui \
    ui/rangestatewindow.ui \
    ui/scriptingwindow.ui \
    ui/signalviewerwindow.ui \
    ui/snifferwindow.ui \
    ui/temporalgraphwindow.ui \
    ui/udsfirmwareuploaderwindow.ui \
    ui/udsscanwindow.ui

#-------------------------------------------------
# Qt resources and translations
#-------------------------------------------------

QML_IMPORT_PATH += $$PWD/qml

RESOURCES += \
    icons.qrc \
    images.qrc \
    qml/qml.qrc

TRANSLATIONS += \
    translations/SavvyLens_en.ts \
    translations/SavvyLens_pt_BR.ts

DISTFILES += \
    translations/pt_BR.qph
