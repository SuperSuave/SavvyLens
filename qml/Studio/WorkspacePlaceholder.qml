import QtQuick 2.15
import QtQuick.Controls 2.15
import SavvyLens 1.0

Item {
    id: root

    property string title: ""
    property string summary: ""
    property string nextStep: ""

    Rectangle {
        id: card

        anchors.centerIn: parent
        width: Math.min(Math.max(320, parent.width - 80), 560)
        height: content.implicitHeight + 54
        radius: Theme.radiusLarge
        color: Theme.surface
        border.color: Theme.borderStrong
        border.width: 1

        Column {
            id: content

            anchors.fill: parent
            anchors.margins: 27
            spacing: 12

            Rectangle {
                width: 36
                height: 36
                radius: Theme.radiusPill
                color: Theme.readOnlySubtle
                border.color: Theme.readOnlyBorder
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: "+"
                    color: Theme.accent
                    font.pixelSize: Math.round(22 * Theme.studioScale)
                }
            }

            Text {
                text: root.title
                color: Theme.textPrimary
                font.pixelSize: Math.round(22 * Theme.studioScale)
                font.bold: true
            }

            Text {
                width: parent.width
                text: root.summary
                color: Theme.textMuted
                font.pixelSize: Theme.studioTextBody
                wrapMode: Text.WordWrap
            }

            Rectangle {
                width: parent.width
                height: nextStepText.implicitHeight + 18
                radius: Theme.radiusMedium
                color: Theme.neutralSubtle
                border.color: Theme.neutralBorder
                border.width: 1

                Text {
                    id: nextStepText

                    anchors.fill: parent
                    anchors.margins: 9
                    text: root.nextStep
                    color: Theme.textFaint
                    font.pixelSize: Theme.studioTextSmall
                    wrapMode: Text.WordWrap
                }
            }
        }
    }
}
