import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: root

    property var markersModel: []

    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape
    padding: 0

    width: 760
    height: 460

    x: Math.round((root.parent.width - width) / 2)
    y: Math.round((root.parent.height - height) / 2)

    background: Rectangle {
        radius: 5
        color: "#1a1928"
        border.color: "#4a4868"
        border.width: 1
    }

    contentItem: Rectangle {
        color: "#1a1928"

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 58
                color: "#201f32"

                Rectangle {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    width: 5
                    color: "#00d4d4"
                }

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    height: 1
                    color: "#2e2d44"
                }

                Column {
                    anchors.left: parent.left
                    anchors.leftMargin: 22
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 3

                    Text {
                        text: "ANALYSIS MARKERS"
                        color: "#e8e8f0"

                        font.bold: true
                        font.pixelSize: 15
                        font.letterSpacing: 1.4
                    }

                    Text {
                        text: "Session-only markers created from Live Change Explorer"
                        color: "#7a7a99"
                        font.pixelSize: 11
                    }
                }

                Button {
                    id: closeButton

                    anchors.right: parent.right
                    anchors.rightMargin: 14
                    anchors.verticalCenter: parent.verticalCenter

                    width: 28
                    height: 28
                    text: "×"

                    background: Rectangle {
                        radius: 3
                        color: closeButton.hovered
                               ? "#252438"
                               : "transparent"
                        border.color: closeButton.hovered
                                      ? "#4a4868"
                                      : "transparent"
                        border.width: 1
                    }

                    contentItem: Text {
                        text: closeButton.text
                        color: "#e8e8f0"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 20
                    }

                    onClicked: root.close()
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 18
                    spacing: 12

                    Text {
                        Layout.fillWidth: true
                        text: "Read-only in-memory analysis markers. These are separate from legacy bookmarks."
                        color: "#7a7a99"
                        font.pixelSize: 11
                        wrapMode: Text.WordWrap
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: "#2e2d44"
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 30
                        radius: 3
                        color: "#252438"
                        border.color: "#2e2d44"
                        border.width: 1

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            anchors.rightMargin: 10
                            spacing: 12

                            Text {
                                Layout.preferredWidth: 62
                                text: "MARKER"
                                color: "#7a7a99"
                                font.bold: true
                                font.pixelSize: 10
                                font.letterSpacing: 0.8
                            }

                            Text {
                                Layout.preferredWidth: 145
                                text: "ANCHOR"
                                color: "#7a7a99"
                                font.bold: true
                                font.pixelSize: 10
                                font.letterSpacing: 0.8
                            }

                            Text {
                                Layout.preferredWidth: 55
                                text: "BUS"
                                color: "#7a7a99"
                                font.bold: true
                                font.pixelSize: 10
                                font.letterSpacing: 0.8
                            }

                            Text {
                                Layout.preferredWidth: 105
                                text: "CAN ID"
                                color: "#7a7a99"
                                font.bold: true
                                font.pixelSize: 10
                                font.letterSpacing: 0.8
                            }

                            Text {
                                Layout.fillWidth: true
                                text: "LABEL"
                                color: "#7a7a99"
                                font.bold: true
                                font.pixelSize: 10
                                font.letterSpacing: 0.8
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        radius: 3
                        color: "#1a1928"
                        border.color: "#2e2d44"
                        border.width: 1
                        clip: true

                        ListView {
                            id: markerList

                            anchors.fill: parent
                            anchors.margins: 4

                            clip: true
                            spacing: 4
                            model: root.markersModel

                            delegate: Rectangle {
                                required property int index
                                required property var modelData

                                width: markerList.width
                                height: 36
                                radius: 3

                                color: index % 2 === 0
                                       ? "#201f32"
                                       : "#252438"

                                border.color: Qt.rgba(0.180, 0.176, 0.267, 1.0)
                                border.width: 1

                                Rectangle {
                                    anchors.left: parent.left
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom

                                    width: 3
                                    color: "#00d4d4"
                                }

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.leftMargin: 10
                                    anchors.rightMargin: 10
                                    spacing: 12

                                    Text {
                                        Layout.preferredWidth: 62
                                        text: modelData.number
                                        color: "#e8e8f0"
                                        font.pixelSize: 11
                                    }

                                    Text {
                                        Layout.preferredWidth: 145
                                        text: modelData.anchorType
                                        color: "#e8e8f0"
                                        font.pixelSize: 11
                                        elide: Text.ElideRight
                                    }

                                    Text {
                                        Layout.preferredWidth: 55
                                        text: modelData.bus
                                        color: "#e8e8f0"
                                        font.pixelSize: 11
                                    }

                                    Text {
                                        Layout.preferredWidth: 105
                                        text: modelData.canId
                                        color: "#00d4d4"
                                        font.family: "Consolas"
                                        font.pixelSize: 11
                                    }

                                    Text {
                                        Layout.fillWidth: true
                                        text: modelData.label.length > 0
                                              ? modelData.label
                                              : "—"
                                        color: modelData.label.length > 0
                                               ? "#e8e8f0"
                                               : "#4a4a60"
                                        font.pixelSize: 11
                                        elide: Text.ElideRight
                                    }
                                }
                            }

                            Text {
                                anchors.centerIn: parent
                                visible: markerList.count === 0

                                text: "No analysis markers have been created in this session."
                                color: "#7a7a99"
                                font.pixelSize: 12
                            }
                        }
                    }
                }
            }
        }
    }
}