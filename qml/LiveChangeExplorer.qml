import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root

    implicitWidth: 1130
    implicitHeight: 640
    
    color: "#12111e"

    readonly property color background: "#12111e"
    readonly property color surface: "#1a1928"
    readonly property color surfaceRaised: "#201f32"
    readonly property color surfaceInset: "#252438"
    readonly property color border: "#2e2d44"
    readonly property color borderStrong: "#4a4868"

    readonly property color textPrimary: "#e8e8f0"
    readonly property color textMuted: "#7a7a99"
    readonly property color textFaint: "#4a4a60"

    readonly property color accent: "#00d4d4"
    readonly property color accentSubtle: "#173f46"

    readonly property color success: "#39d353"
    readonly property color successSubtle: "#173d2a"

    readonly property color warning: "#f5a623"
    readonly property color warningSubtle: "#463514"

    readonly property color error: "#ff4d4d"
    readonly property color errorSubtle: "#472126"

    property int selectedRow: -1

    readonly property var columnHeaders: [
        "Bus",
        "CAN ID",
        "Direction",
        "Format",
        "Type",
        "Count",
        "Latest Payload",
        "Changed Bytes"
    ]

    readonly property var columnWidths: [
        64,
        108,
        88,
        76,
        88,
        110,
        320,
        230
    ]

    function cellText(column,
                      bus,
                      canIdText,
                      directionText,
                      formatText,
                      frameTypeText,
                      occurrenceCount,
                      latestPayloadText,
                      changedBytesText) {
        switch (column) {
        case 0:
            return bus
        case 1:
            return canIdText
        case 2:
            return directionText
        case 3:
            return formatText
        case 4:
            return frameTypeText
        case 5:
            return occurrenceCount
        case 6:
            return latestPayloadText
        case 7:
            return changedBytesText
        default:
            return ""
        }
    }

    Rectangle {
        id: workspaceHeader

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: 70
        color: root.surface

        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            width: 5
            color: root.accent
        }

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            height: 1
            color: root.border
        }

        Column {
            anchors.left: parent.left
            anchors.leftMargin: 24
            anchors.verticalCenter: parent.verticalCenter
            spacing: 4

            Text {
                text: "LIVE CHANGE EXPLORER"
                color: root.textPrimary

                font.bold: true
                font.pixelSize: 16
                font.letterSpacing: 1.6
            }

            Text {
                text: "CAN identity and payload-difference analysis"
                color: root.textMuted
                font.pixelSize: 12
            }
        }

        Rectangle {
            anchors.right: parent.right
            anchors.rightMargin: 18
            anchors.verticalCenter: parent.verticalCenter

            width: 94
            height: 26
            radius: 13

            color: root.successSubtle
            border.color: "#268d3a"
            border.width: 1

            Row {
                anchors.centerIn: parent
                spacing: 6

                Rectangle {
                    width: 7
                    height: 7
                    radius: 4
                    color: root.success
                }

                Text {
                    text: "LIVE"
                    color: root.success

                    font.bold: true
                    font.pixelSize: 10
                    font.letterSpacing: 1
                }
            }
        }
    }

    Rectangle {
        id: tableCard

        anchors.top: workspaceHeader.bottom
        anchors.topMargin: 16
        anchors.left: parent.left
        anchors.leftMargin: 16
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 16

        radius: 6
        color: root.surface
        border.color: root.border
        border.width: 1

        Rectangle {
            id: tableAccentLine

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            height: 2
            color: root.accent
            opacity: 0.7
        }

        Row {
            id: headerRow

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: tableAccentLine.height

            height: 36

            Repeater {
                model: root.columnHeaders.length

                Rectangle {
                    width: root.columnWidths[index]
                    height: headerRow.height

                    color: root.surfaceRaised
                    border.color: root.border
                    border.width: 1

                    Text {
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        anchors.rightMargin: 8

                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight

                        text: root.columnHeaders[index]
                        color: root.textMuted

                        font.bold: true
                        font.pixelSize: 11
                        font.letterSpacing: 0.6
                    }
                }
            }
        }

        TableView {
            id: tableView

            anchors.top: headerRow.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            clip: true
            boundsBehavior: Flickable.StopAtBounds

            model: liveChangeExplorerModel

            columnWidthProvider: function(column) {
                return root.columnWidths[column]
            }

            rowHeightProvider: function(row) {
                return 32
            }

            delegate: Rectangle {
                implicitWidth: root.columnWidths[column]
                implicitHeight: 32

                property bool isSelected: row === root.selectedRow
                property bool isChangeColumn: column === 7
                property bool comparisonAvailable: hasPrevious
                property bool lengthChanged: hasPrevious && payloadLengthChanged
                property bool payloadChanged:
                    hasPrevious
                    && changedBytesText !== "None"
                    && !payloadLengthChanged

                color: {
                    if (isSelected)
                        return root.accentSubtle

                    if (isChangeColumn && lengthChanged)
                        return root.warningSubtle

                    if (isChangeColumn && payloadChanged)
                        return root.successSubtle

                    return row % 2 === 0
                            ? root.surface
                            : root.surfaceInset
                }

                border.color: {
                    if (isChangeColumn && lengthChanged)
                        return "#88601a"

                    if (isChangeColumn && payloadChanged)
                        return "#277b3a"

                    return root.border
                }

                border.width: isSelected ? 0 : 1

                Rectangle {
                    visible: !isSelected

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    height: 1
                    color: root.border
                }

                Rectangle {
                    visible: isSelected

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top

                    height: 1
                    color: root.accent
                }

                Rectangle {
                    visible: isSelected

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    height: 1
                    color: root.accent
                }

                Rectangle {
                    visible: isSelected && column === 0

                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    width: 3
                    color: root.accent
                }

                Rectangle {
                    visible: isSelected
                             && column === root.columnHeaders.length - 1

                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    width: 1
                    color: root.accent
                }
                Row {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 8
                    spacing: 7

                    // Reserve dot space only for a Changed Bytes cell that has an actual change.
                    Item {
                        id: changeIndicatorSlot

                        readonly property bool active:
                            isChangeColumn
                            && comparisonAvailable
                            && (payloadChanged || lengthChanged)

                        width: active ? 7 : 0
                        height: parent.height
                        visible: active

                        Rectangle {
                            anchors.centerIn: parent

                            width: 7
                            height: 7
                            radius: width / 2

                            color: lengthChanged ? root.warning : root.success
                        }
                    }

                    Item {
                        id: payloadCell

                        width: column === 6 ? parent.width : 0
                        height: parent.height

                        visible: column === 6

                        readonly property bool payloadHasChanges:
                            hasPrevious
                            && changedByteIndexes
                            && changedByteIndexes.length > 0

                        // Always-present fallback payload text.
                        Text {
                            id: plainPayloadText

                            anchors.fill: parent
                            anchors.rightMargin: 4

                            visible: !payloadCell.payloadHasChanges

                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight

                            text: latestPayloadText
                            color: root.textPrimary

                            font.family: "Consolas"
                            font.pixelSize: 12
                        }

                        // Used only for frames with an actual changed byte.
                        Row {
                            id: highlightedPayloadRow

                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 3

                            visible: payloadCell.payloadHasChanges

                            Repeater {
                                model: payloadCell.payloadHasChanges
                                    ? latestPayloadText.split(" ")
                                    : []

                                Rectangle {
                                    readonly property bool isChangedByte:
                                        changedByteIndexes.indexOf(index) !== -1

                                    width: byteLabel.implicitWidth + 6
                                    height: 20
                                    radius: 3

                                    color: {
                                        if (!isChangedByte)
                                            return "transparent"

                                        return payloadLengthChanged
                                            ? root.warningSubtle
                                            : root.successSubtle
                                    }

                                    border.color: {
                                        if (!isChangedByte)
                                            return "transparent"

                                        return payloadLengthChanged
                                            ? "#88601a"
                                            : "#277b3a"
                                    }

                                    border.width: isChangedByte ? 1 : 0

                                    Text {
                                        id: byteLabel
                                        anchors.centerIn: parent

                                        text: modelData

                                        color: isChangedByte
                                            ? (payloadLengthChanged
                                                ? root.warning
                                                : root.success)
                                            : root.textPrimary

                                        font.family: "Consolas"
                                        font.pixelSize: 12
                                    }
                                }
                            }
                        }
                    }

                    Text {
                        id: cellLabel

                        width: parent.width - changeIndicatorSlot.width
                        height: parent.height

                        visible: column !== 6

                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter

                        text: root.cellText(
                            column,
                            bus,
                            canIdText,
                            directionText,
                            formatText,
                            frameTypeText,
                            occurrenceCount,
                            latestPayloadText,
                            changedBytesText)

                        color: {
                            if (column === 0)
                                return root.textMuted

                            if (column === 1)
                                return root.accent

                            if (column === 2)
                                return directionText === "Rx"
                                    ? root.success
                                    : root.warning

                            if (column === 7 && !comparisonAvailable)
                                return root.textFaint

                            if (column === 7 && lengthChanged)
                                return root.warning

                            if (column === 7 && payloadChanged)
                                return root.success

                            return root.textPrimary
                        }

                        font.family: "Consolas"
                        font.pixelSize: 12
                    }
                }
                
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        root.selectedRow = row
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded

                contentItem: Rectangle {
                    implicitWidth: 8
                    radius: 4
                    color: root.borderStrong
                }
            }
        }

        Text {
            anchors.centerIn: tableView
            visible: tableView.rows === 0

            text: "Waiting for live CAN traffic"
            color: root.textMuted
            font.pixelSize: 16
        }
    }
}
