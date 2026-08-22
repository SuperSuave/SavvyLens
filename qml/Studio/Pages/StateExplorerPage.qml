import QtQuick 2.15
import QtQuick.Controls 2.15
import SavvyLens 1.0

Item {
    id: root

    readonly property var presentation: stateExplorerPresentation

    function statusColor(isTruncated) {
        return isTruncated ? Theme.warning : Theme.readOnly
    }

    function statusBackground(isTruncated) {
        return isTruncated ? Theme.warningSubtle : Theme.readOnlySubtle
    }

    function statusBorder(isTruncated) {
        return isTruncated ? Theme.warningBorder : Theme.readOnlyBorder
    }

    function evidenceStatusText(isTruncated) {
        return isTruncated
                ? "Incomplete / truncated evidence"
                : "Completed bounded evidence"
    }

    Flickable {
        id: flickable

        anchors.fill: parent
        clip: true
        contentWidth: width
        contentHeight: contentColumn.height + Theme.studioSpacingXLarge * 2

        Column {
            id: contentColumn

            x: Theme.studioSpacingXLarge
            y: Theme.studioSpacingXLarge
            width: Math.max(0, flickable.width - Theme.studioSpacingXLarge * 2)
            spacing: Theme.studioSpacingMedium

            Column {
                width: parent.width
                spacing: Theme.studioSpacingSmall

                Text {
                    text: "State Explorer"
                    color: Theme.textPrimary
                    font.pixelSize: Theme.studioTextPageTitle
                    font.bold: true
                }

                Text {
                    width: parent.width
                    text: "Evidence for one explicit signal candidate. Observed evidence only. Not a vehicle-semantic conclusion."
                    color: Theme.textMuted
                    font.pixelSize: Theme.studioTextBody
                    wrapMode: Text.WordWrap
                }
            }

            Rectangle {
                width: parent.width
                height: identityContent.implicitHeight
                        + Theme.studioSpacingLarge * 2
                radius: Theme.studioRadiusLarge
                color: Theme.surface
                border.color: Theme.readOnlyBorder
                border.width: 1

                Rectangle {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: 3
                    radius: Theme.radiusSmall
                    color: Theme.readOnly
                }

                Column {
                    id: identityContent

                    anchors.fill: parent
                    anchors.margins: Theme.studioSpacingLarge
                    anchors.leftMargin: Theme.studioSpacingLarge
                                        + Theme.studioSpacingSmall
                    spacing: Theme.studioSpacingMedium

                    Row {
                        width: parent.width
                        spacing: Theme.studioSpacingSmall

                        Text {
                            text: "Explicit demo candidate"
                            color: Theme.readOnly
                            font.pixelSize: Theme.studioTextSection
                            font.bold: true
                        }

                        Rectangle {
                            width: evidenceLabel.implicitWidth
                                   + Theme.studioSpacingMedium * 2
                            height: evidenceLabel.implicitHeight
                                    + Theme.spacingSmall
                            radius: Theme.radiusPill
                            color: Theme.readOnlySubtle
                            border.color: Theme.readOnlyBorder
                            border.width: 1

                            Text {
                                id: evidenceLabel
                                anchors.centerIn: parent
                                text: "READ-ONLY EVIDENCE"
                                color: Theme.readOnly
                                font.pixelSize: Theme.studioTextSmall
                                font.bold: true
                            }
                        }
                    }

                    Grid {
                        width: parent.width
                        columns: width >= 780 ? 5 : 2
                        columnSpacing: Theme.studioSpacingMedium
                        rowSpacing: Theme.studioSpacingMedium

                        Repeater {
                            model: [
                                { "label": "CAN ID", "value": root.presentation.canIdText },
                                { "label": "START BIT", "value": root.presentation.startBit },
                                { "label": "BIT LENGTH", "value": root.presentation.bitLength },
                                { "label": "ENDIAN", "value": root.presentation.endianText },
                                { "label": "SIGNEDNESS", "value": root.presentation.signednessText }
                            ]

                            delegate: Column {
                                width: (parent.width
                                        - parent.columnSpacing
                                        * (parent.columns - 1))
                                       / parent.columns
                                spacing: Theme.spacingXSmall

                                Text {
                                    text: modelData.label
                                    color: Theme.textFaint
                                    font.pixelSize: Theme.studioTextSmall
                                }

                                Text {
                                    text: modelData.value
                                    color: Theme.textPrimary
                                    font.pixelSize: Theme.studioTextSection
                                    font.bold: true
                                    elide: Text.ElideRight
                                }
                            }
                        }
                    }

                    Text {
                        width: parent.width
                        text: root.presentation.candidateDisplayName
                        color: Theme.textMuted
                        font.pixelSize: Theme.studioTextBody
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: acceptedContent.implicitHeight
                        + Theme.studioSpacingMedium * 2
                radius: Theme.studioRadiusMedium
                color: Theme.surfaceRaised
                border.color: Theme.border
                border.width: 1

                Row {
                    id: acceptedContent

                    anchors.fill: parent
                    anchors.margins: Theme.studioSpacingMedium
                    spacing: Theme.studioSpacingMedium

                    Text {
                        text: "Accepted samples"
                        color: Theme.textMuted
                        font.pixelSize: Theme.studioTextBody
                    }

                    Text {
                        text: root.presentation.acceptedSampleCount
                        color: Theme.textPrimary
                        font.pixelSize: Theme.studioTextTitle
                        font.bold: true
                    }

                    Text {
                        width: parent.width
                               - implicitWidth
                               - Theme.studioSpacingMedium
                        text: root.presentation.hasEvidence
                              ? "Frames rejected by CAN-ID filtering or payload support do not contribute accepted sample indexes."
                              : "No accepted samples are available for this candidate."
                        color: Theme.textFaint
                        font.pixelSize: Theme.studioTextSmall
                        wrapMode: Text.WordWrap
                    }
                }
            }

            EvidenceSection {
                width: parent.width
                title: "Discrete state evidence"
                classificationText: root.presentation.discreteClassificationText
                statusText: root.evidenceStatusText(
                                root.presentation.discreteTruncated)
                truncated: root.presentation.discreteTruncated
                emptyText: "No observed state values were retained."
                rowModel: root.presentation.observedStates
                columns: [
                    { "label": "VALUE", "role": "valueText" },
                    { "label": "OCCURRENCES", "role": "occurrenceCount" },
                    { "label": "FIRST SAMPLE", "role": "firstSampleIndex" },
                    { "label": "LAST SAMPLE", "role": "lastSampleIndex" }
                ]
            }

            EvidenceSection {
                width: parent.width
                title: "Directed transition evidence"
                classificationText: root.presentation.transitionClassificationText
                statusText: root.evidenceStatusText(
                                root.presentation.transitionTruncated)
                truncated: root.presentation.transitionTruncated
                detailText: root.presentation.transitionValueChangeCount
                            + " observed value changes; repeated equal values are not transitions."
                emptyText: "No directed transitions were retained."
                rowModel: root.presentation.observedTransitions
                columns: [
                    { "label": "FROM", "role": "fromValueText" },
                    { "label": "TO", "role": "toValueText" },
                    { "label": "OCCURRENCES", "role": "occurrenceCount" },
                    { "label": "FIRST SAMPLE", "role": "firstSampleIndex" },
                    { "label": "LAST SAMPLE", "role": "lastSampleIndex" }
                ]
            }

            EvidenceSection {
                width: parent.width
                title: "Temporal run evidence"
                classificationText: root.presentation.temporalClassificationText
                statusText: root.evidenceStatusText(
                                root.presentation.temporalTruncated)
                truncated: root.presentation.temporalTruncated
                detailText: root.presentation.temporalValueChangeCount
                            + " observed value changes. Runs contain sample counts only; no timestamps or elapsed dwell duration are inferred."
                emptyText: "No consecutive-value runs were retained."
                rowModel: root.presentation.observedRuns
                columns: [
                    { "label": "VALUE", "role": "valueText" },
                    { "label": "SAMPLES", "role": "sampleCount" },
                    { "label": "FIRST SAMPLE", "role": "firstSampleIndex" },
                    { "label": "LAST SAMPLE", "role": "lastSampleIndex" }
                ]
            }
        }
    }

    component EvidenceSection: Rectangle {
        id: section

        property string title: ""
        property string classificationText: ""
        property string statusText: ""
        property bool truncated: false
        property string detailText: ""
        property string emptyText: ""
        property var rowModel: []
        property var columns: []

        height: sectionContent.implicitHeight
                + Theme.studioSpacingLarge * 2
        radius: Theme.studioRadiusLarge
        color: Theme.surface
        border.color: root.statusBorder(truncated)
        border.width: 1

        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 3
            radius: Theme.radiusSmall
            color: root.statusColor(section.truncated)
        }

        Column {
            id: sectionContent

            anchors.fill: parent
            anchors.margins: Theme.studioSpacingLarge
            anchors.leftMargin: Theme.studioSpacingLarge
                                + Theme.studioSpacingSmall
            spacing: Theme.studioSpacingMedium

            Row {
                width: parent.width
                spacing: Theme.studioSpacingSmall

                Column {
                    width: parent.width - statusPill.width
                           - Theme.studioSpacingSmall
                    spacing: Theme.spacingXSmall

                    Text {
                        text: section.title
                        color: Theme.textPrimary
                        font.pixelSize: Theme.studioTextSection
                        font.bold: true
                    }

                    Text {
                        width: parent.width
                        text: section.classificationText
                        color: Theme.textMuted
                        font.pixelSize: Theme.studioTextBody
                        wrapMode: Text.WordWrap
                    }
                }

                Rectangle {
                    id: statusPill

                    width: statusTextItem.implicitWidth
                           + Theme.studioSpacingMedium * 2
                    height: statusTextItem.implicitHeight
                            + Theme.spacingSmall
                    radius: Theme.radiusPill
                    color: root.statusBackground(section.truncated)
                    border.color: root.statusBorder(section.truncated)
                    border.width: 1

                    Text {
                        id: statusTextItem
                        anchors.centerIn: parent
                        text: section.statusText
                        color: root.statusColor(section.truncated)
                        font.pixelSize: Theme.studioTextSmall
                        font.bold: true
                    }
                }
            }

            Text {
                visible: section.detailText.length > 0
                width: parent.width
                text: section.detailText
                color: Theme.textFaint
                font.pixelSize: Theme.studioTextSmall
                wrapMode: Text.WordWrap
            }

            Rectangle {
                width: parent.width
                height: 1
                color: Theme.divider
            }

            Grid {
                id: headerGrid

                visible: section.rowModel.length > 0
                width: parent.width
                columns: section.columns.length
                columnSpacing: Theme.studioSpacingMedium

                Repeater {
                    model: section.columns

                    delegate: Text {
                        width: (headerGrid.width
                                - headerGrid.columnSpacing
                                * (headerGrid.columns - 1))
                               / headerGrid.columns
                        text: modelData.label
                        color: Theme.textFaint
                        font.pixelSize: Theme.studioTextSmall
                        font.bold: true
                        elide: Text.ElideRight
                    }
                }
            }

            Repeater {
                model: section.rowModel

                delegate: Rectangle {
                    property var rowData: modelData

                    width: sectionContent.width
                    height: rowGrid.implicitHeight
                            + Theme.spacingSmall * 2
                    radius: Theme.radiusSmall
                    color: index % 2 === 0
                           ? Theme.surfaceInset
                           : Theme.surfaceRaised

                    Grid {
                        id: rowGrid

                        anchors.fill: parent
                        anchors.margins: Theme.spacingSmall
                        columns: section.columns.length
                        columnSpacing: Theme.studioSpacingMedium

                        Repeater {
                            model: section.columns

                            delegate: Text {
                                width: (rowGrid.width
                                        - rowGrid.columnSpacing
                                        * (rowGrid.columns - 1))
                                       / rowGrid.columns
                                text: {
                                    var key = modelData.role
                                    return rowData[key] === undefined ? "" : rowData[key]
                                }
                                color: Theme.textPrimary
                                font.pixelSize: Theme.studioTextBody
                                elide: Text.ElideRight
                            }
                        }
                    }
                }
            }

            Text {
                visible: section.rowModel.length === 0
                width: parent.width
                text: section.emptyText
                color: Theme.textFaint
                font.pixelSize: Theme.studioTextBody
                wrapMode: Text.WordWrap
            }

            Text {
                width: parent.width
                text: "Observed evidence only. Not a vehicle-semantic conclusion."
                color: Theme.textFaint
                font.pixelSize: Theme.studioTextSmall
                wrapMode: Text.WordWrap
            }
        }
    }
}
