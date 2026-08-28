import QtQuick 2.15
import QtQuick.Controls 2.15
import SavvyLens 1.0

Rectangle {
    id: root

    property string projectTitle: "No capture loaded"
    property string statusText: "Studio foundation"
    property bool isCapturing: false

    implicitHeight: Theme.studioTopBarHeight
    color: Theme.chrome
    radius: Theme.radiusLarge
    border.color: Theme.border
    border.width: 1
    clip: true

    Row {
        anchors.fill: parent
        anchors.leftMargin: Theme.studioSpacingXLarge
        anchors.rightMargin: Theme.studioSpacingXLarge
        spacing: Theme.studioSpacingLarge

        Column {
            anchors.verticalCenter: parent.verticalCenter
            width: Math.min(420, Math.max(180, root.width * 0.40))
            spacing: 3

            Text {
                text: "SavvyLens Studio"
                color: Theme.textPrimary
                font.pixelSize: Theme.studioTextTitle
                font.bold: true
            }

            Text {
                width: parent.width
                text: root.projectTitle
                color: Theme.textMuted
                font.pixelSize: Theme.studioTextBody
                elide: Text.ElideRight
            }
        }

        Item {
            width: Math.max(
                0,
                root.width
                - 44
                - Math.min(420, Math.max(180, root.width * 0.40))
                - statusBadge.width
                - closeButton.width
                - 32)
            height: 1
        }

        Rectangle {
            id: statusBadge

            anchors.verticalCenter: parent.verticalCenter
            width: statusRow.implicitWidth + 20
            height: 28
            radius: Theme.radiusPill
            color: root.isCapturing ? Theme.successSubtle : Theme.chromeRaised
            border.color: root.isCapturing ? Theme.successBorder : Theme.neutralBorder

            Row {
                id: statusRow
                anchors.centerIn: parent
                spacing: 7

                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 7
                    height: 7
                    radius: 4
                    color: root.isCapturing ? Theme.success : Theme.textMuted
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: root.statusText
                    color: Theme.textPrimary
                    font.pixelSize: Theme.studioTextBody
                }
            }
        }

        ToolButton {
            id: closeButton

            anchors.verticalCenter: parent.verticalCenter
            width: Math.round(30 * Theme.studioScale)
            height: Math.round(30 * Theme.studioScale)
            text: "×"
            font.pixelSize: Math.round(22 * Theme.studioScale)

            contentItem: Text {
                text: parent.text
                color: closeButton.hovered
                       ? Theme.textPrimary
                       : Theme.textMuted
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font: parent.font
            }

            background: Rectangle {
                radius: Theme.radiusMedium
                color: closeButton.down
                       ? Theme.destructiveSubtle
                       : closeButton.hovered
                         ? Theme.errorSubtle
                         : Theme.transparent
                border.color: closeButton.activeFocus
                              ? Theme.focus
                              : closeButton.hovered
                                ? Theme.errorBorder
                                : Theme.border
                border.width: 1
            }

            ToolTip.visible: hovered
            ToolTip.text: "Close SavvyLens Studio"

            onClicked: {
                if (studioHost) {
                    studioHost.closeStudio()
                }
            }
        }
    }
}
