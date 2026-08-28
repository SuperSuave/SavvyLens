import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import SavvyLens 1.0

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
        radius: Theme.radiusMedium
        color: Theme.popoverSurface
        border.color: Theme.tooltipBorder
        border.width: 1
    }

    contentItem: Rectangle {
        color: Theme.surface

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: Theme.topBarHeight
                color: Theme.surfaceRaised

                Rectangle {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    width: 5
                    color: Theme.accent
                }

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    height: 1
                    color: Theme.border
                }

                Column {
                    anchors.left: parent.left
                    anchors.leftMargin: 22
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 3

                    Text {
                        text: "ANALYSIS MARKERS"
                        color: Theme.textPrimary

                        font.bold: true
                        font.pixelSize: 15
                        font.letterSpacing: 1.4
                    }

                    Text {
                        text: "Session-only markers created from Live Change Explorer"
                        color: Theme.textMuted
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
                        radius: Theme.radiusSmall
                        color: closeButton.down
                               ? Theme.pressed
                               : closeButton.hovered
                                 ? Theme.hover
                                 : Theme.transparent
                        border.color: closeButton.activeFocus
                                      ? Theme.focus
                                      : closeButton.hovered
                                        ? Theme.borderStrong
                                        : Theme.transparent
                        border.width: 1
                    }

                    contentItem: Text {
                        text: closeButton.text
                        color: Theme.textPrimary
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
                        color: Theme.readOnly
                        font.pixelSize: 11
                        wrapMode: Text.WordWrap
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: Theme.border
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 30
                        radius: 3
                        color: Theme.surfaceInset
                        border.color: Theme.border
                        border.width: 1

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            anchors.rightMargin: 10
                            spacing: 12

                            Text {
                                Layout.preferredWidth: 62
                                text: "MARKER"
                                color: Theme.textMuted
                                font.bold: true
                                font.pixelSize: 10
                                font.letterSpacing: 0.8
                            }

                            Text {
                                Layout.preferredWidth: 145
                                text: "ANCHOR"
                                color: Theme.textMuted
                                font.bold: true
                                font.pixelSize: 10
                                font.letterSpacing: 0.8
                            }

                            Text {
                                Layout.preferredWidth: 55
                                text: "BUS"
                                color: Theme.textMuted
                                font.bold: true
                                font.pixelSize: 10
                                font.letterSpacing: 0.8
                            }

                            Text {
                                Layout.preferredWidth: 105
                                text: "CAN ID"
                                color: Theme.textMuted
                                font.bold: true
                                font.pixelSize: 10
                                font.letterSpacing: 0.8
                            }

                            Text {
                                Layout.fillWidth: true
                                text: "LABEL"
                                color: Theme.textMuted
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
                        color: Theme.surface
                        border.color: Theme.border
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
                                       ? Theme.surfaceRaised
                                       : Theme.surfaceInset

                                border.color: Theme.border
                                border.width: 1

                                Rectangle {
                                    anchors.left: parent.left
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom

                                    width: 3
                                    color: Theme.accent
                                }

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.leftMargin: 10
                                    anchors.rightMargin: 10
                                    spacing: 12

                                    Text {
                                        Layout.preferredWidth: 62
                                        text: modelData.number
                                        color: Theme.textPrimary
                                        font.pixelSize: 11
                                    }

                                    Text {
                                        Layout.preferredWidth: 145
                                        text: modelData.anchorType
                                        color: Theme.textPrimary
                                        font.pixelSize: 11
                                        elide: Text.ElideRight
                                    }

                                    Text {
                                        Layout.preferredWidth: 55
                                        text: modelData.bus
                                        color: Theme.textPrimary
                                        font.pixelSize: 11
                                    }

                                    Text {
                                        Layout.preferredWidth: 105
                                        text: modelData.canId
                                        color: Theme.accent
                                        font.family: "Consolas"
                                        font.pixelSize: 11
                                    }

                                    Text {
                                        Layout.fillWidth: true
                                        text: modelData.label.length > 0
                                              ? modelData.label
                                              : "—"
                                        color: modelData.label.length > 0
                                               ? Theme.textPrimary
                                               : Theme.textFaint
                                        font.pixelSize: 11
                                        elide: Text.ElideRight
                                    }
                                }
                            }

                            Text {
                                anchors.centerIn: parent
                                visible: markerList.count === 0

                                text: "No analysis markers have been created in this session."
                                color: Theme.textMuted
                                font.pixelSize: 12
                            }
                        }
                    }
                }
            }
        }
    }
}