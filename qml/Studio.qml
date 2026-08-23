import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import SavvyLens 1.0
import "Studio"
import "Studio/Pages"

Rectangle {
    id: root

    implicitWidth: 1280
    implicitHeight: 760
    color: Theme.background

    property string activeWorkspaceId: "dash"
    property string stateExplorerSourceLabel:
        "Source: Deterministic demo evidence"
    property bool stateExplorerUsesLiveChangeSnapshot: false
    readonly property int shellMargin: Theme.studioSpacingMedium

    readonly property var workspaceDefinitions: [
        {
            "id": "dash",
            "label": "Dash",
            "summary": "A unified operational starting point for SavvyLens Studio.",
            "nextStep": "Dash establishes the Studio shell and will later consume shared capture and project state."
        },
        {
            "id": "traffic",
            "label": "Traffic",
            "summary": "Live traffic discovery and aggregation will be integrated here without duplicating the existing Live Change Explorer analysis model.",
            "nextStep": "Next integration: host the established Live Change Explorer as a Studio workspace."
        },
        {
            "id": "explore",
            "label": "Explore",
            "summary": "Find and understand CAN behavior through selected-signal evidence, state observations, transitions, and temporal runs.",
            "nextStep": "Current integration: bounded State Explorer evidence for an explicit demo RangeSignalSpec."
        },
        {
            "id": "compare",
            "label": "Compare",
            "summary": "Compare captures, frames, and investigation evidence.",
            "nextStep": "Future integration: existing comparator and bookmark-event analysis workflows."
        },
        {
            "id": "experiment",
            "label": "Experiment",
            "summary": "Test hypotheses with clearly separated active-operation safety boundaries.",
            "nextStep": "Future integration: control analysis, sender, fuzzing, and replay—without weakening safety boundaries."
        },
        {
            "id": "visualize",
            "label": "Visualize",
            "summary": "Present decoded and raw evidence through graphs, dashboards, and signal views.",
            "nextStep": "Future integration: Graphing and Signal Viewer workflows."
        },
        {
            "id": "automate",
            "label": "Automate",
            "summary": "Organize repeatable investigation workflows, scripts, filters, and triggers.",
            "nextStep": "Future integration: scripting, triggers, and query work."
        },
        {
            "id": "diagnostics",
            "label": "Diagnostics",
            "summary": "Inspect protocol and ECU diagnostic workflows in an explicit, safety-aware workspace.",
            "nextStep": "Future integration: ISO-TP, UDS Scan, and firmware workflows."
        }
    ]

    property var openWorkspaces: [workspaceDefinitions[0]]

    function workspaceDefinition(workspaceId) {
        for (var index = 0; index < workspaceDefinitions.length; ++index) {
            if (workspaceDefinitions[index].id === workspaceId)
                return workspaceDefinitions[index]
        }

        return workspaceDefinitions[0]
    }

    function openWorkspace(workspaceId) {
        var definition = workspaceDefinition(workspaceId)
        var alreadyOpen = false

        for (var index = 0; index < openWorkspaces.length; ++index) {
            if (openWorkspaces[index].id === definition.id) {
                alreadyOpen = true
                break
            }
        }

        if (!alreadyOpen) {
            var updatedWorkspaces = openWorkspaces.slice(0)
            updatedWorkspaces.push(definition)
            openWorkspaces = updatedWorkspaces
        }

        activeWorkspaceId = definition.id
    }

    function seedStateExplorerCandidate(canIdText) {
        openWorkspace("explore")

        for (var index = 0; index < workspaceStack.count; ++index) {
            var item = workspaceStack.itemAt(index)

            if (item
                    && item.item
                    && item.item.seedCandidate) {
                item.item.seedCandidate(canIdText)
                return
            }
        }
    }

    StudioTopBar {
        id: topBar

        anchors.left: parent.left
        anchors.leftMargin: root.shellMargin
        anchors.top: parent.top
        anchors.topMargin: root.shellMargin
        anchors.right: parent.right
        anchors.rightMargin: root.shellMargin

        projectTitle: "Unified CAN reverse-engineering workspace"
        statusText: "Studio foundation"
        isCapturing: false
    }

    StudioBottomBar {
        id: bottomBar

        anchors.left: parent.left
        anchors.leftMargin: root.shellMargin
        anchors.right: parent.right
        anchors.rightMargin: root.shellMargin
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.shellMargin

        captureStatusText: "No capture loaded"
        selectionStatusText: "No Studio selection"
        systemStatusText: "Passive analysis"
    }

    StudioNavRail {
        id: navigationRail

        anchors.left: parent.left
        anchors.leftMargin: root.shellMargin
        anchors.top: topBar.bottom
        anchors.topMargin: Theme.spacingSmall
        anchors.bottom: bottomBar.top
        anchors.bottomMargin: Theme.spacingSmall

        currentWorkspaceId: root.activeWorkspaceId

        onWorkspaceRequested: root.openWorkspace(workspaceId)
    }

    Rectangle {
        id: inspector

        width: Theme.studioInspectorWidth
        anchors.top: topBar.bottom
        anchors.topMargin: Theme.spacingSmall
        anchors.right: parent.right
        anchors.rightMargin: root.shellMargin
        anchors.bottom: bottomBar.top
        anchors.bottomMargin: Theme.spacingSmall

        color: Theme.surface
        radius: Theme.radiusLarge
        clip: true
        border.color: Theme.border
        border.width: 1

        Text {
            id: inspectorTitle

            anchors.top: parent.top
            anchors.topMargin: 18
            anchors.left: parent.left
            anchors.leftMargin: 18
            anchors.right: parent.right
            anchors.rightMargin: 18

            text: "Inspector"
            color: Theme.textPrimary
            font.pixelSize: Theme.studioTextSection
            font.bold: true
        }

        Text {
            id: inspectorDescription

            anchors.top: inspectorTitle.bottom
            anchors.topMargin: 12
            anchors.left: parent.left
            anchors.leftMargin: 18
            anchors.right: parent.right
            anchors.rightMargin: 18

            text: "Shared selection and evidence inspection will appear here when at least two Studio workspaces need the same context."
            color: Theme.textMuted
            font.pixelSize: Theme.studioTextSmall
            wrapMode: Text.WordWrap
        }

        Rectangle {
            id: inspectorDivider

            anchors.top: inspectorDescription.bottom
            anchors.topMargin: 12
            anchors.left: parent.left
            anchors.leftMargin: 18
            anchors.right: parent.right
            anchors.rightMargin: 18

            height: 1
            color: Theme.border
        }

        Text {
            id: currentWorkspaceLabel

            anchors.top: inspectorDivider.bottom
            anchors.topMargin: 12
            anchors.left: parent.left
            anchors.leftMargin: 18
            anchors.right: parent.right
            anchors.rightMargin: 18

            text: "Current workspace"
            color: Theme.textFaint
            font.pixelSize: 11
        }

        Rectangle {
            id: currentWorkspaceAccent

            anchors.top: currentWorkspaceLabel.bottom
            anchors.topMargin: Theme.spacingSmall
            anchors.left: parent.left
            anchors.leftMargin: Theme.spacingLarge

            width: 3
            height: currentWorkspaceName.implicitHeight
            radius: Theme.radiusSmall
            color: Theme.accent
        }

        Text {
            id: currentWorkspaceName

            anchors.top: currentWorkspaceLabel.bottom
            anchors.topMargin: Theme.spacingSmall
            anchors.left: currentWorkspaceAccent.right
            anchors.leftMargin: Theme.spacingSmall
            anchors.right: parent.right
            anchors.rightMargin: Theme.spacingLarge

            text: root.workspaceDefinition(root.activeWorkspaceId).label
            color: Theme.accent
            font.pixelSize: Theme.studioTextSection
            font.bold: true
        }

        Rectangle {
            id: inspectorNoteCard

            anchors.left: parent.left
            anchors.leftMargin: 18
            anchors.right: parent.right
            anchors.rightMargin: 18
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 18

            height: inspectorNote.implicitHeight + 18
            radius: 5
            color: Theme.surfaceInset
            border.color: Theme.border
            border.width: 1

            Text {
                id: inspectorNote

                anchors.fill: parent
                anchors.margins: 9

                text: "The Studio shell is intentionally passive. Existing capture, replay, transmit, and legacy tools remain unchanged."
                color: Theme.textFaint
                font.pixelSize: 11
                wrapMode: Text.WordWrap
            }
        }
    }

    StudioTabStrip {
        id: tabStrip

        anchors.left: navigationRail.right
        anchors.leftMargin: Theme.spacingSmall
        anchors.top: topBar.bottom
        anchors.topMargin: Theme.spacingSmall
        anchors.right: inspector.left
        anchors.rightMargin: Theme.spacingSmall

        tabs: root.openWorkspaces
        activeWorkspaceId: root.activeWorkspaceId

        onTabRequested: root.activeWorkspaceId = workspaceId
    }

    Rectangle {
        id: contentSurface

        anchors.left: navigationRail.right
        anchors.leftMargin: Theme.spacingSmall
        anchors.top: tabStrip.bottom
        anchors.topMargin: Theme.spacingSmall
        anchors.right: inspector.left
        anchors.rightMargin: Theme.spacingSmall
        anchors.bottom: bottomBar.top
        anchors.bottomMargin: Theme.spacingSmall

        color: Theme.background
        radius: Theme.radiusLarge
        clip: true

        StackLayout {
            id: workspaceStack

            anchors.fill: parent

            currentIndex: {
                for (var index = 0; index < root.openWorkspaces.length; ++index) {
                    if (root.openWorkspaces[index].id === root.activeWorkspaceId)
                        return index
                }

                return 0
            }

            Repeater {
                model: root.openWorkspaces

                delegate: Loader {
                    active: true
                    sourceComponent: modelData.id === "dash"
                                    ? dashComponent
                                    : modelData.id === "traffic"
                                    ? trafficComponent
                                    : modelData.id === "explore"
                                        ? stateExplorerComponent
                                        : placeholderComponent

                    property var workspaceDefinition: modelData
                }
            }
        }
    }

    Component {
        id: dashComponent

        DashPage {
            anchors.fill: parent
        }
    }
    Component {
        id: trafficComponent

        LiveChangeExplorer {
            anchors.fill: parent
        }
    }
    Component {
        id: stateExplorerComponent

        StateExplorerPage {
            anchors.fill: parent
        }
    }
    Component {
        id: placeholderComponent

        WorkspacePlaceholder {
            anchors.fill: parent
            title: workspaceDefinition.label
            summary: workspaceDefinition.summary
            nextStep: workspaceDefinition.nextStep
        }
    }
}
