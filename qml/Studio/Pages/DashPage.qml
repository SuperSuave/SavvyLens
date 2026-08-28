import QtQuick 2.15
import QtQuick.Controls 2.15
import SavvyLens 1.0

Item {
    id: root

    function metricCard(title, value, detail, accentColor) {
        return {
            "title": title,
            "value": value,
            "detail": detail,
            "accentColor": accentColor
        }
    }

    readonly property var metrics: [
        metricCard("Capture", "Ready",
                   "Open a capture or connect to a bus",
                   Theme.accent),
        metricCard("Traffic", "—",
                   "Live aggregate integration is next",
                   Theme.success),
        metricCard("Selection", "None",
                   "Shared Studio selection is intentionally deferred",
                   Theme.warning),
        metricCard("Explore", "Planned",
                   "State Explorer will be the first evidence workspace",
                   Theme.readOnly)
    ]

    Flickable {
        id: flickable

        anchors.fill: parent
        contentWidth: width
        contentHeight: dashboardContent.height + 52
        clip: true

        Column {
            id: dashboardContent

            x: 26
            y: 26
            width: Math.max(0, flickable.width - 52)
            spacing: 20

            Column {
                spacing: 5

                Text {
                    text: "Dash"
                    color: Theme.textPrimary
                    font.pixelSize: Theme.studioTextPageTitle
                    font.bold: true
                }

                Text {
                    text: "Your SavvyLens Studio starting point."
                    color: Theme.textMuted
                    font.pixelSize: Theme.studioTextBody
                }
            }

            Grid {
                id: metricGrid

                width: parent.width
                columns: width >= 980 ? 4 : 2
                spacing: 12

                Repeater {
                    model: root.metrics

                    delegate: Rectangle {
                        width: (metricGrid.width
                                - metricGrid.spacing
                                * (metricGrid.columns - 1))
                               / metricGrid.columns
                        height: Math.round(116 * Theme.studioScale)
                        radius: Theme.radiusLarge
                        color: Theme.surface
                        border.color: Theme.border
                        border.width: 1

                        Rectangle {
                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            width: 3
                            radius: 2
                            color: modelData.accentColor
                        }

                        Column {
                            anchors.fill: parent
                            anchors.margins: 17
                            anchors.leftMargin: 20
                            spacing: 7

                            Text {
                                text: modelData.title
                                color: Theme.textMuted
                                font.pixelSize: 12
                            }

                            Text {
                                text: modelData.value
                                color: Theme.textPrimary
                                font.pixelSize: Math.round(22 * Theme.studioScale)
                                font.bold: true
                            }

                            Text {
                                width: parent.width
                                text: modelData.detail
                                color: Theme.textFaint
                                font.pixelSize: 11
                                wrapMode: Text.WordWrap
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 260
                radius: Theme.radiusLarge
                color: Theme.surface
                border.color: Theme.border
                border.width: 1

                Column {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 10

                    Text {
                        text: "Studio activity"
                        color: Theme.textPrimary
                        font.pixelSize: 15
                        font.bold: true
                    }

                    Text {
                        width: parent.width
                        text: "This region will become the shared operational summary: active capture, traffic activity, selected context, and recent investigation work."
                        color: Theme.textMuted
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                    }

                    Item {
                        width: parent.width
                        height: Math.max(
                                    1,
                                    parent.height
                                    - 15
                                    - 10
                                    - 30
                                    - 10)

                        Text {
                            anchors.centerIn: parent
                            text: "No Studio activity yet"
                            color: Theme.neutral
                            font.pixelSize: Theme.studioTextBody
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: quickStart.implicitHeight + 38
                radius: Theme.radiusLarge
                color: Theme.surfaceRaised
                border.color: Theme.border
                border.width: 1

                Column {
                    id: quickStart

                    anchors.fill: parent
                    anchors.margins: 19
                    spacing: 7

                    Text {
                        text: "Build path"
                        color: Theme.textPrimary
                        font.pixelSize: 14
                        font.bold: true
                    }

                    Text {
                        width: parent.width
                        text: "Studio shell → State Explorer evidence page → Traffic integration → shared context and inspector → progressive legacy migration."
                        color: Theme.textMuted
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
    }
}
