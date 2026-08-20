import QtQuick 2.15
import QtQuick.Controls 2.15
import SavvyLens 1.0

Rectangle {
    id: root

    implicitWidth: 1130
    implicitHeight: 640

    color: Theme.background

    property int selectedRow: -1

    readonly property var columnHeaders: [
        "Bus",
        "CAN ID",
        "Direction",
        "Format",
        "Type",
        "Count",
        "Rate",
        "Last Seen",
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
        88,
        98,
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
                      rateText,
                      lastSeenAgeText,
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
            return rateText
        case 7:
            return lastSeenAgeText
        case 8:
            return latestPayloadText
        case 9:
            return changedBytesText
        default:
            return ""
        }
    }

    AnalysisMarkersDialog {
        id: analysisMarkersDialog

        parent: root
        markersModel: []
    }

    Timer {
        interval: 1000
        repeat: true
        running: true

        onTriggered: liveChangeExplorerModel.refreshActivityAges()
    }

    Popup {
        id: markerLabelDialog

        property int markerRow: -1

        parent: root
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape
        padding: 0

        width: 420
        height: 214

        x: Math.round((root.width - width) / 2)
        y: Math.round((root.height - height) / 2)

        background: Rectangle {
            radius: 5
            color: Theme.surface
            border.color: Theme.borderStrong
            border.width: 1
        }

        contentItem: Rectangle {
            color: Theme.surface

            Column {
                anchors.fill: parent
                anchors.margins: 18
                spacing: 12

                Text {
                    text: "CREATE ANALYSIS MARKER"
                    color: Theme.textPrimary

                    font.bold: true
                    font.pixelSize: 14
                    font.letterSpacing: 1.0
                }

                Text {
                    width: parent.width
                    text: "Optionally label this session-only marker."
                    color: Theme.textMuted
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                }

                Text {
                    text: "LABEL"
                    color: Theme.textMuted

                    font.bold: true
                    font.pixelSize: 10
                    font.letterSpacing: 0.8
                }

                TextField {
                    id: markerLabelField

                    width: parent.width
                    placeholderText: "Optional marker label"
                    color: Theme.textPrimary
                    placeholderTextColor: Theme.textFaint
                    selectByMouse: true

                    background: Rectangle {
                        radius: 3
                        color: Theme.surfaceInset
                        border.color: markerLabelField.activeFocus
                                      ? Theme.accent
                                      : Theme.border
                        border.width: 1
                    }

                    onAccepted: {
                        liveChangeExplorerHost.createMarkerForRow(
                                    markerLabelDialog.markerRow,
                                    markerLabelField.text.trim())

                        markerLabelDialog.close()
                    }
                }

                Row {
                    anchors.right: parent.right
                    spacing: 8

                    Button {
                        id: cancelMarkerButton

                        text: "Cancel"

                        background: Rectangle {
                            radius: 3
                            color: cancelMarkerButton.hovered
                                   ? Theme.surfaceInset
                                   : Theme.surfaceRaised
                            border.color: Theme.border
                            border.width: 1
                        }

                        contentItem: Text {
                            text: cancelMarkerButton.text
                            color: Theme.textPrimary
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 11
                        }

                        onClicked: markerLabelDialog.close()
                    }

                    Button {
                        id: confirmMarkerButton

                        text: "Create Marker"

                        background: Rectangle {
                            radius: 3
                            color: confirmMarkerButton.hovered
                                   ? Theme.accent
                                   : Theme.accentSubtle
                            border.color: Theme.accent
                            border.width: 1
                        }

                        contentItem: Text {
                            text: confirmMarkerButton.text
                            color: Theme.textPrimary
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 11
                        }

                        onClicked: {
                            liveChangeExplorerHost.createMarkerForRow(
                                        markerLabelDialog.markerRow,
                                        markerLabelField.text.trim())

                            markerLabelDialog.close()
                        }
                    }
                }
            }
        }

        onVisibleChanged: {
            if (!visible)
            {
                markerRow = -1
                markerLabelField.text = ""
            }
        }
    }

    function openAnalysisMarkersDialog(markers) {
        analysisMarkersDialog.markersModel = markers
        analysisMarkersDialog.open()
    }

    Rectangle {
        id: workspaceHeader

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: 70
        color: Theme.surface

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
            anchors.leftMargin: 24
            anchors.verticalCenter: parent.verticalCenter
            spacing: 4

            Text {
                text: "LIVE CHANGE EXPLORER"
                color: Theme.textPrimary

                font.bold: true
                font.pixelSize: 16
                font.letterSpacing: 1.6
            }

            Text {
                text: "CAN identity and payload-difference analysis"
                color: Theme.textMuted
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

            color: Theme.successSubtle
            border.color: Theme.successStrongBorder
            border.width: 1

            Row {
                anchors.centerIn: parent
                spacing: 6

                Rectangle {
                    width: 7
                    height: 7
                    radius: 4
                    color: Theme.success
                }

                Text {
                    text: "LIVE"
                    color: Theme.success

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
        color: Theme.surface
        border.color: Theme.border
        border.width: 1

        Rectangle {
            id: tableAccentLine

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            height: 2
            color: Theme.accent
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

                    color: Theme.surfaceRaised
                    border.color: Theme.border
                    border.width: 1

                    Text {
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        anchors.rightMargin: 8

                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight

                        text: root.columnHeaders[index]
                        color: Theme.textMuted

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
            anchors.bottomMargin: 44

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
                property bool isChangeColumn: column === 9
                property bool comparisonAvailable:
                    typeof hasPrevious !== "undefined"
                    && hasPrevious === true
                property bool lengthChanged:
                    comparisonAvailable
                    && typeof payloadLengthChanged !== "undefined"
                    && payloadLengthChanged === true
                property bool payloadChanged:
                    comparisonAvailable
                    && typeof changedBytesText !== "undefined"
                    && changedBytesText !== "None"
                    && !lengthChanged

                color: {
                    if (isSelected)
                        return Theme.accentSubtle

                    if (isChangeColumn && lengthChanged)
                        return Theme.warningSubtle

                    if (isChangeColumn && payloadChanged)
                        return Theme.successSubtle

                    return row % 2 === 0
                            ? Theme.surface
                            : Theme.surfaceInset
                }

                border.color: {
                    if (isChangeColumn && lengthChanged)
                        return Theme.warningBorder

                    if (isChangeColumn && payloadChanged)
                        return Theme.successBorder

                    return Theme.border
                }

                border.width: isSelected ? 0 : 1

                Rectangle {
                    visible: !isSelected

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    height: 1
                    color: Theme.border
                }

                Rectangle {
                    visible: isSelected

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top

                    height: 1
                    color: Theme.accent
                }

                Rectangle {
                    visible: isSelected

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    height: 1
                    color: Theme.accent
                }

                Rectangle {
                    visible: isSelected && column === 0

                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    width: 3
                    color: Theme.accent
                }

                Rectangle {
                    visible: isSelected
                             && column === root.columnHeaders.length - 1

                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    width: 1
                    color: Theme.accent
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

                            color: lengthChanged ? Theme.warning : Theme.success
                        }
                    }

                    Item {
                        id: payloadCell

                        width: column === 8 ? parent.width : 0
                        height: parent.height

                        visible: column === 8

                        readonly property bool payloadHasChanges:
                            typeof hasPrevious !== "undefined"
                            && hasPrevious === true
                            && typeof changedByteIndexes !== "undefined"
                            && changedByteIndexes !== null
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
                            color: Theme.textPrimary

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
                                                ? Theme.warningSubtle
                                                : Theme.successSubtle
                                    }

                                    border.color: {
                                        if (!isChangedByte)
                                            return "transparent"

                                        return payloadLengthChanged
                                                ? Theme.warningBorder
                                                : Theme.successBorder
                                    }

                                    border.width: isChangedByte ? 1 : 0

                                    Text {
                                        id: byteLabel
                                        anchors.centerIn: parent

                                        text: modelData

                                        color: isChangedByte
                                               ? (payloadLengthChanged
                                                  ? Theme.warning
                                                  : Theme.success)
                                               : Theme.textPrimary

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

                        visible: column !== 8

                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter

                        text: root.cellText(
                                  column,
                                  typeof bus !== "undefined" ? bus : "",
                                  typeof canIdText !== "undefined"
                                      ? canIdText : "",
                                  typeof directionText !== "undefined"
                                      ? directionText : "",
                                  typeof formatText !== "undefined"
                                      ? formatText : "",
                                  typeof frameTypeText !== "undefined"
                                      ? frameTypeText : "",
                                  typeof occurrenceCount !== "undefined"
                                      ? occurrenceCount : "",
                                  typeof rateText !== "undefined"
                                      ? rateText : "—",
                                  typeof lastSeenAgeText !== "undefined"
                                      ? lastSeenAgeText : "—",
                                  typeof latestPayloadText !== "undefined"
                                      ? latestPayloadText : "—",
                                  typeof changedBytesText !== "undefined"
                                      ? changedBytesText : "—")

                        color: {
                            if (column === 0)
                                return Theme.textMuted

                            if (column === 1)
                                return Theme.accent

                            if (column === 2)
                                return directionText === "Rx"
                                        ? Theme.success
                                        : Theme.warning

                            if ((column === 6 && rateText === "—")
                                    || (column === 7
                                        && lastSeenAgeText === "—")
                                    || (column === 9
                                        && !comparisonAvailable))
                                return Theme.textFaint

                            if (column === 9 && lengthChanged)
                                return Theme.warning

                            if (column === 9 && payloadChanged)
                                return Theme.success

                            return Theme.textPrimary
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
                    color: Theme.borderStrong
                }
            }
        }

        Connections {
            target: liveChangeExplorerModel

            function onModelReset() {
                root.selectedRow = -1
                tableView.contentY = 0
            }
        }

        Rectangle {
            id: selectionActions

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            height: 44
            color: Theme.surfaceRaised
            border.color: Theme.border
            border.width: 1

            Row {
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                spacing: 8

                    Text {
                        text: "Search IDs:"
                        color: Theme.textMuted
                        font.pixelSize: 11
                    }

                    TextField {
                        id: filterIdsField

                        width: 190
                        height: 28

                        text: liveChangeExplorerModel.filterText
                        placeholderText: "e.g. 123, 7E0 0x646"
                        placeholderTextColor: Theme.textFaint
                        color: Theme.textPrimary
                        selectByMouse: true

                        ToolTip.visible: hovered
                        ToolTip.text:
                            "Comma- or space-separated CAN ID text; matches any term"

                        background: Rectangle {
                            radius: 3
                            color: Theme.surfaceInset
                            border.color: filterIdsField.activeFocus
                                          ? Theme.accent
                                          : Theme.border
                            border.width: 1
                        }

                        onTextEdited: {
                            root.selectedRow = -1
                            liveChangeExplorerModel.filterText = text
                        }
                    }

                Rectangle {
                    width: 1
                    height: 18
                    color: Theme.border
                }

                Text {
                    text: root.selectedRow >= 0
                          ? "Selected row " + (root.selectedRow + 1)
                          : "Select a frame to inspect"
                    color: root.selectedRow >= 0
                           ? Theme.textMuted
                           : Theme.textFaint
                    font.pixelSize: 11
                }

                Button {
                    id: openFrameInfoButton

                    text: "Open Frame Info"
                    enabled: root.selectedRow >= 0

                    background: Rectangle {
                        radius: 3
                        color: openFrameInfoButton.enabled
                               ? Theme.accentSubtle
                               : Theme.surfaceInset
                        border.color: openFrameInfoButton.enabled
                                      ? Theme.accent
                                      : Theme.border
                        border.width: 1
                    }

                    contentItem: Text {
                        text: openFrameInfoButton.text
                        color: openFrameInfoButton.enabled
                               ? Theme.textPrimary
                               : Theme.textFaint
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 11
                    }

                    onClicked: {
                        liveChangeExplorerHost.openFrameInfoForRow(
                                    root.selectedRow)
                    }
                }

                Button {
                    id: graphSelectedIdButton

                    text: "Graph Selected ID"
                    enabled: root.selectedRow >= 0

                    background: Rectangle {
                        radius: 3
                        color: graphSelectedIdButton.enabled
                               ? Theme.accentSubtle
                               : Theme.surfaceInset
                        border.color: graphSelectedIdButton.enabled
                                      ? Theme.accent
                                      : Theme.border
                        border.width: 1
                    }

                    contentItem: Text {
                        text: graphSelectedIdButton.text
                        color: graphSelectedIdButton.enabled
                               ? Theme.textPrimary
                               : Theme.textFaint
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 11
                    }

                    onClicked: {
                        liveChangeExplorerHost.openGraphingForRow(
                                    root.selectedRow)
                    }
                }

                Button {
                    id: createMarkerForRow

                    text: "Create Marker"
                    enabled: root.selectedRow >= 0

                    background: Rectangle {
                        radius: 3
                        color: createMarkerForRow.enabled
                               ? Theme.accentSubtle
                               : Theme.surfaceInset
                        border.color: createMarkerForRow.enabled
                                      ? Theme.accent
                                      : Theme.border
                        border.width: 1
                    }

                    contentItem: Text {
                        text: createMarkerForRow.text
                        color: createMarkerForRow.enabled
                               ? Theme.textPrimary
                               : Theme.textFaint
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 11
                    }

                    onClicked: {
                        markerLabelDialog.markerRow = root.selectedRow
                        markerLabelField.text = ""
                        markerLabelDialog.open()
                        markerLabelField.forceActiveFocus()
                    }
                }

                Button {
                    id: openAnalysisMarkers

                    text: "Analysis Markers"

                    background: Rectangle {
                        radius: 3
                        color: openAnalysisMarkers.enabled
                               ? Theme.accentSubtle
                               : Theme.surfaceInset
                        border.color: openAnalysisMarkers.enabled
                                      ? Theme.accent
                                      : Theme.border
                        border.width: 1
                    }

                    contentItem: Text {
                        text: openAnalysisMarkers.text
                        color: openAnalysisMarkers.enabled
                               ? Theme.textPrimary
                               : Theme.textFaint
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 11
                    }

                    onClicked: {
                        liveChangeExplorerHost.openAnalysisMarkers()
                    }
                }
            }
        }

        Text {
            anchors.centerIn: tableView
            visible: tableView.rows === 0

            text: filterIdsField.text.trim().length > 0
                  ? "No CAN IDs match this search"
                  : "Waiting for live CAN traffic"
            color: Theme.textMuted
            font.pixelSize: 16
        }
    }
}
