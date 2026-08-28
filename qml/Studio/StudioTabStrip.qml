import QtQuick 2.15
import QtQuick.Controls 2.15
import SavvyLens 1.0

Rectangle {
    id: root

    property var tabs: []
    property string activeWorkspaceId: ""
    signal tabRequested(string workspaceId)

    implicitHeight: Theme.studioTabBarHeight
    color: Theme.surfaceRaised
    border.color: Theme.border
    border.width: 1

    Row {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        spacing: 4

        Repeater {
            model: root.tabs

            delegate: Rectangle {
                readonly property bool active: modelData.id === root.activeWorkspaceId

                anchors.verticalCenter: parent.verticalCenter
                width: tabText.implicitWidth + 34
                height: Math.round(30 * Theme.studioScale)
                radius: Theme.radiusMedium
                color: active
                       ? Theme.surface
                       : tabMouse.pressed ? Theme.pressed
                                          : tabMouse.containsMouse
                                            ? Theme.hover
                                            : Theme.transparent
                border.color: active
                              ? Theme.selectionBorder
                              : tabMouse.containsMouse
                                ? Theme.borderStrong
                                : Theme.transparent
                border.width: 1

                Text {
                    id: tabText
                    anchors.centerIn: parent
                    text: modelData.label
                    color: active ? Theme.textPrimary : Theme.textMuted
                    font.pixelSize: Theme.studioTextSmall
                    font.bold: active
                }

                MouseArea {
                    id: tabMouse

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: root.tabRequested(modelData.id)
                }
            }
        }
    }
}
