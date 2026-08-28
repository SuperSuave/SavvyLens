import QtQuick 2.15
import QtQuick.Controls 2.15
import SavvyLens 1.0

Rectangle {
    id: root

    property string captureStatusText: "No capture loaded"
    property string selectionStatusText: "No Studio selection"
    property string systemStatusText: "Passive analysis"

    implicitHeight: 42
    color: Theme.chrome
    radius: Theme.radiusLarge
    border.color: Theme.border
    border.width: 1
    clip: true

    Row {
        anchors.fill: parent
        anchors.leftMargin: Theme.spacingLarge
        anchors.rightMargin: Theme.spacingLarge
        spacing: Theme.spacingMedium

        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            width: 7
            height: 7
            radius: Theme.radiusPill
            color: Theme.neutral
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.captureStatusText
            color: Theme.textMuted
            font.pixelSize: 11
            font.family: "Consolas"
        }

        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            width: 1
            height: 16
            color: Theme.border
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.selectionStatusText
            color: Theme.textFaint
            font.pixelSize: 11
        }

        Item {
            width: Math.max(
                       0,
                       parent.width
                       - 420
                       - statusChip.width)
            height: 1
        }

        Rectangle {
            id: statusChip

            anchors.verticalCenter: parent.verticalCenter
            width: statusChipText.implicitWidth + 16
            height: 22
            radius: Theme.radiusPill
            color: Theme.readOnlySubtle
            border.color: Theme.readOnlyBorder
            border.width: 1

            Text {
                id: statusChipText

                anchors.centerIn: parent
                text: root.systemStatusText
                color: Theme.readOnly
                font.pixelSize: 10
                font.bold: true
                font.letterSpacing: 0.6
            }
        }
    }
}
