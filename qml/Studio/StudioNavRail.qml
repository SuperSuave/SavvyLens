import QtQuick 2.15
import QtQuick.Controls 2.15
import SavvyLens 1.0

Rectangle {
    id: root

    property string currentWorkspaceId: "dash"
    signal workspaceRequested(string workspaceId)

    readonly property var navigationItems: [
        { "id": "dash",        "label": "Dash",        "glyph": "⌂" },
        { "id": "traffic",     "label": "Traffic",     "glyph": "◈" },
        { "id": "explore",     "label": "Explore",     "glyph": "⌕" },
        { "id": "compare",     "label": "Compare",     "glyph": "⇄" },
        { "id": "experiment",  "label": "Experiment",  "glyph": "⚗" },
        { "id": "visualize",   "label": "Visualize",   "glyph": "◌" },
        { "id": "automate",    "label": "Automate",    "glyph": "⚙" },
        { "id": "diagnostics", "label": "Diagnostics", "glyph": "✚" }
    ]

    width: Theme.studioNavigationRailWidth
    color: Theme.surface
    radius: Theme.radiusLarge
    border.color: Theme.border
    border.width: 1
    clip: true

    Text {
        id: workspaceHeading

        anchors.top: parent.top
        anchors.topMargin: 18
        anchors.left: parent.left
        anchors.leftMargin: 18
        anchors.right: parent.right
        anchors.rightMargin: 18

        text: "WORKSPACES"
        color: Theme.textFaint
        font.pixelSize: Theme.studioTextSmall
        font.bold: true
        font.letterSpacing: 1.2
    }

    Column {
        id: navigationList

        anchors.top: workspaceHeading.bottom
        anchors.topMargin: 7
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 3

        Repeater {
            model: root.navigationItems

            delegate: Item {
                width: navigationList.width
                height: Math.round(40 * Theme.studioScale)

                readonly property bool selected: modelData.id === root.currentWorkspaceId

                Rectangle {
                    id: navigationItemBackground

                    anchors.left: parent.left
                    anchors.leftMargin: Theme.spacingSmall
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.spacingSmall
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    radius: Theme.radiusMedium
                    color: selected
                           ? Theme.transparent
                           : navMouse.pressed
                             ? Theme.pressed
                             : navMouse.containsMouse
                               ? Theme.hover
                               : Theme.transparent
                    border.color: selected
                                  ? Theme.transparent
                                  : navMouse.containsMouse
                                    ? Theme.borderStrong
                                    : Theme.transparent
                    border.width: 1

                    Rectangle {
                        anchors.fill: parent
                        radius: parent.radius
                        visible: selected

                        gradient: Gradient {
                            orientation: Gradient.Horizontal

                            GradientStop {
                                position: 0.0
                                color: Theme.accentSubtle
                            }

                            GradientStop {
                                position: 0.38
                                color: Theme.selection
                            }

                            GradientStop {
                                position: 1.0
                                color: Theme.transparent
                            }
                        }
                    }

                    Rectangle {
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        width: Math.max(3, Math.round(3 * Theme.studioScale))
                        radius: Theme.radiusSmall
                        visible: selected
                        color: Theme.accent
                    }
                }

                Row {
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    spacing: 11

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        width: 16
                        text: modelData.glyph
                        color: selected ? Theme.accent : Theme.textMuted
                        font.pixelSize: Math.round(16 * Theme.studioScale)
                        horizontalAlignment: Text.AlignHCenter
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: modelData.label
                        color: selected || navMouse.containsMouse
                               ? Theme.textPrimary
                               : Theme.textMuted
                        font.pixelSize: Theme.studioTextBody
                        font.bold: selected
                    }
                }

                MouseArea {
                    id: navMouse

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: root.workspaceRequested(modelData.id)
                }
            }
        }
    }

    Rectangle {
        id: footerDivider

        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.bottom: footerText.top
        anchors.bottomMargin: 12

        height: 1
        color: Theme.border
    }

    Text {
        id: footerText

        anchors.left: parent.left
        anchors.leftMargin: 18
        anchors.right: parent.right
        anchors.rightMargin: 18
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 18

        text: "The unified workspace is being assembled one proven workflow at a time."
        color: Theme.textFaint
        font.pixelSize: 11
        wrapMode: Text.WordWrap
    }
}
