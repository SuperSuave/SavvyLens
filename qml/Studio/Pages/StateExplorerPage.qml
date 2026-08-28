import QtQuick 2.15
import QtQuick.Controls 2.15
import SavvyLens 1.0

Item {
    id: root

    readonly property var presentation: stateExplorerPresentation

    readonly property string sourceLabel: presentation.sourceLabel
    readonly property string sourceScopeText: presentation.sourceScopeText
    readonly property bool usesLiveChangeSnapshot:
        presentation.usesLiveChangeSnapshot

    property bool littleEndian: true
    property bool signedValue: false
    property string analysisRequestError: ""

    readonly property int parsedCanId: parseCanId(canIdField.text)
    readonly property int parsedStartBit: parseDecimal(startBitField.text)
    readonly property int parsedBitLength: parseDecimal(bitLengthField.text)

    readonly property bool hasParseableCandidate:
        parsedCanId >= 0
        && parsedStartBit >= 0
        && parsedBitLength > 0

    readonly property bool candidateInputValid:
        hasParseableCandidate
        && parsedStartBit + parsedBitLength <= 64

    function parseDecimal(text) {
        var value = text.trim()

        if (!/^[0-9]+$/.test(value))
            return -1

        var parsed = Number(value)
        return Number.isSafeInteger(parsed) ? parsed : -1
    }

    function parseCanId(text) {
        var value = text.trim()
        var base = 10

        if (/^0[xX][0-9a-fA-F]+$/.test(value)) {
            base = 16
            value = value.substring(2)
        } else if (!/^[0-9]+$/.test(value)) {
            return -1
        }

        var parsed = parseInt(value, base)
        if (!Number.isSafeInteger(parsed) || parsed < 0 || parsed > 0x1FFFFFFF)
            return -1

        return parsed
    }

    function inputErrorText() {
        if (!hasParseableCandidate)
            return "Enter a CAN ID, start bit, and bit length using valid non-negative technical values."

        if (parsedStartBit + parsedBitLength > 64)
            return "Candidate bit range must fit within the 64-bit CAN payload layout."

        return ""
    }

    function inputBorderColor(fieldValueValid) {
        return fieldValueValid ? Theme.borderStrong : Theme.warningBorder
    }

    function triggerAnalysis() {
        analysisRequestError = ""

        if (!candidateInputValid) {
            return false
        }

        var accepted =
                studioHost.analyzeStateExplorerCandidate(
                    parsedCanId,
                    parsedStartBit,
                    parsedBitLength,
                    littleEndian,
                    signedValue)

        if (!accepted) {
            analysisRequestError =
                    "Candidate layout was rejected. Evidence was not refreshed."
            return false
        }

        return true
    }

    function seedCandidate(canIdText, startBit, bitLength, isLittleEndian, isSigned) {
        canIdField.text = canIdText
        startBitField.text = (startBit !== undefined && startBit !== null && startBit >= 0) ? startBit.toString() : "0"
        bitLengthField.text = (bitLength !== undefined && bitLength !== null && bitLength > 0) ? bitLength.toString() : "8"
        littleEndian = (isLittleEndian !== undefined && isLittleEndian !== null) ? isLittleEndian : true
        signedValue = (isSigned !== undefined && isSigned !== null) ? isSigned : false
        analysisRequestError = ""

        if (candidateInputValid) {
            triggerAnalysis()
        }
    }

    function seedCandidateWithSpec(canIdText, startBit, bitLength, isLittleEndian, isSigned) {
        seedCandidate(canIdText, startBit, bitLength, isLittleEndian, isSigned)
    }

    function inputBackground(fieldValueValid) {
        return fieldValueValid ? Theme.surfaceInset : Theme.warningSubtle
    }

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
                height: candidateInputContent.implicitHeight
                        + Theme.studioSpacingLarge * 2
                radius: Theme.studioRadiusLarge
                color: Theme.surface
                border.color: root.candidateInputValid
                              ? Theme.borderStrong
                              : Theme.warningBorder
                border.width: 1

                Rectangle {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: 3
                    radius: Theme.radiusSmall
                    color: root.candidateInputValid
                           ? Theme.accent
                           : Theme.warning
                }

                Column {
                    id: candidateInputContent

                    anchors.fill: parent
                    anchors.margins: Theme.studioSpacingLarge
                    anchors.leftMargin: Theme.studioSpacingLarge
                                        + Theme.studioSpacingSmall
                    spacing: Theme.studioSpacingMedium

                    Row {
                        width: parent.width
                        spacing: Theme.studioSpacingSmall

                        Text {
                            text: "Candidate definition"
                            color: Theme.textPrimary
                            font.pixelSize: Theme.studioTextSection
                            font.bold: true
                        }

                        Rectangle {
                            width: inputModeLabel.implicitWidth
                                   + Theme.studioSpacingMedium * 2
                            height: inputModeLabel.implicitHeight
                                    + Theme.spacingSmall
                            radius: Theme.radiusPill
                            color: Theme.accentSubtle
                            border.color: Theme.accentMuted
                            border.width: 1

                            Text {
                                id: inputModeLabel
                                anchors.centerIn: parent
                                text: root.usesLiveChangeSnapshot
                                      ? "SNAPSHOT INPUT"
                                      : "DEMO INPUT"
                                color: Theme.accent
                                font.pixelSize: Theme.studioTextSmall
                                font.bold: true
                            }
                        }
                    }

                    Text {
                        width: parent.width
                        text: root.usesLiveChangeSnapshot
                              ? "Edit one explicit RangeSignalSpec, then explicitly analyze the bounded Live Change Explorer snapshot. The source is not live and no shared Studio selection is changed."
                              : "Edit one explicit RangeSignalSpec, then analyze only the controlled in-memory demo sequence. No capture, live traffic, or shared Studio selection is changed."
                        color: Theme.textMuted
                        font.pixelSize: Theme.studioTextBody
                        wrapMode: Text.WordWrap
                    }
                    Rectangle {
                        width: parent.width
                        height: sourceContent.implicitHeight
                                + Theme.studioSpacingSmall * 2
                        radius: Theme.radiusSmall
                        color: Theme.readOnlySubtle
                        border.color: Theme.readOnlyBorder
                        border.width: 1

                        Column {
                            id: sourceContent

                            anchors.fill: parent
                            anchors.margins: Theme.studioSpacingSmall
                            spacing: Theme.spacingSmall

                            Text {
                                width: parent.width
                                text: root.sourceLabel
                                color: Theme.readOnly
                                font.pixelSize: Theme.studioTextSmall
                                font.family: "monospace"
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                width: parent.width
                                text: root.sourceScopeText
                                color: Theme.textMuted
                                font.pixelSize: Theme.studioTextSmall
                                wrapMode: Text.WordWrap
                            }

                            Button {
                                id: retakeSnapshotButton
                                visible: root.usesLiveChangeSnapshot
                                text: "Re-take snapshot"
                                implicitHeight: Math.max(
                                                    Theme.studioTextSmall
                                                    + Theme.studioSpacingMedium,
                                                    28)

                                onClicked: studioHost.retakeStateExplorerSnapshot()

                                contentItem: Text {
                                    text: retakeSnapshotButton.text
                                    color: retakeSnapshotButton.enabled
                                           ? Theme.textPrimary
                                           : Theme.textDisabled
                                    font.pixelSize: Theme.studioTextSmall
                                    font.bold: true
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                background: Rectangle {
                                    radius: Theme.radiusSmall
                                    color: !retakeSnapshotButton.enabled
                                           ? Theme.disabled
                                           : retakeSnapshotButton.down
                                             ? Theme.pressed
                                             : retakeSnapshotButton.hovered
                                               ? Theme.chromeRaised
                                               : Theme.surface
                                    border.color: retakeSnapshotButton.hovered
                                                  ? Theme.accent
                                                  : Theme.borderStrong
                                    border.width: 1
                                }
                            }

                            Row {
                                visible: !root.usesLiveChangeSnapshot
                                spacing: Theme.spacingSmall

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: "Demo scenario:"
                                    color: Theme.textMuted
                                    font.pixelSize: Theme.studioTextSmall
                                }

                                ComboBox {
                                    id: demoScenarioSelector
                                    model: studioHost.demoScenarioNames()
                                    currentIndex: 0

                                    onActivated: {
                                        studioHost.loadDemoScenario(index)
                                        if (root.candidateInputValid) {
                                            root.triggerAnalysis()
                                        }
                                    }

                                    contentItem: Text {
                                        leftPadding: Theme.spacingSmall
                                        rightPadding: demoScenarioSelector.indicator.width + Theme.spacingSmall
                                        text: demoScenarioSelector.displayText
                                        font.pixelSize: Theme.studioTextSmall
                                        color: Theme.textPrimary
                                        verticalAlignment: Text.AlignVCenter
                                        elide: Text.ElideRight
                                    }

                                    indicator: Text {
                                        x: demoScenarioSelector.width - width - Theme.spacingSmall
                                        y: (demoScenarioSelector.height - height) / 2
                                        text: "▼"
                                        font.pixelSize: 8
                                        color: demoScenarioSelector.hovered ? Theme.accent : Theme.textMuted
                                    }

                                    background: Rectangle {
                                        implicitWidth: 200
                                        implicitHeight: 28
                                        radius: Theme.radiusSmall
                                        color: demoScenarioSelector.hovered ? Theme.chromeRaised : Theme.surfaceInset
                                        border.color: demoScenarioSelector.hovered ? Theme.accent : Theme.borderStrong
                                        border.width: 1
                                    }

                                    popup: Popup {
                                        y: demoScenarioSelector.height
                                        width: demoScenarioSelector.width
                                        implicitHeight: contentItem.implicitHeight
                                        padding: 1

                                        contentItem: ListView {
                                            clip: true
                                            implicitHeight: contentHeight
                                            model: demoScenarioSelector.popup.visible ? demoScenarioSelector.delegateModel : null
                                            currentIndex: demoScenarioSelector.highlightedIndex

                                            ScrollIndicator.vertical: ScrollIndicator { }
                                        }

                                        background: Rectangle {
                                            color: Theme.surfaceRaised
                                            border.color: Theme.borderStrong
                                            border.width: 1
                                            radius: Theme.radiusSmall
                                        }
                                    }

                                    delegate: ItemDelegate {
                                        width: demoScenarioSelector.width
                                        height: 28
                                        highlighted: demoScenarioSelector.highlightedIndex === index

                                        contentItem: Text {
                                            text: modelData
                                            color: highlighted ? Theme.accent : Theme.textPrimary
                                            font.pixelSize: Theme.studioTextSmall
                                            verticalAlignment: Text.AlignVCenter
                                            elide: Text.ElideRight
                                        }

                                        background: Rectangle {
                                            color: highlighted ? Theme.hover : (hovered ? Theme.chromeRaised : Theme.transparent)
                                        }
                                    }
                                }
                            }
                        }
                    }
                    Grid {
                        width: parent.width
                        columns: width >= 850 ? 5 : 2
                        columnSpacing: Theme.studioSpacingMedium
                        rowSpacing: Theme.studioSpacingMedium

                        Column {
                            width: (parent.width
                                    - parent.columnSpacing
                                    * (parent.columns - 1))
                                   / parent.columns
                            spacing: Theme.spacingXSmall

                            Text {
                                text: "CAN ID"
                                color: Theme.textFaint
                                font.pixelSize: Theme.studioTextSmall
                            }

                            TextField {
                                id: canIdField

                                width: parent.width
                                text: "0x321"
                                selectByMouse: true
                                color: Theme.textPrimary
                                font.pixelSize: Theme.studioTextBody
                                font.family: "monospace"
                                placeholderText: "0x000"
                                placeholderTextColor: Theme.textFaint
                                background: Rectangle {
                                    radius: Theme.radiusSmall
                                    color: root.inputBackground(
                                               root.parsedCanId >= 0)
                                    border.color: root.inputBorderColor(
                                                      root.parsedCanId >= 0)
                                    border.width: 1
                                }
                            }
                        }

                        Column {
                            width: (parent.width
                                    - parent.columnSpacing
                                    * (parent.columns - 1))
                                   / parent.columns
                            spacing: Theme.spacingXSmall

                            Text {
                                text: "START BIT"
                                color: Theme.textFaint
                                font.pixelSize: Theme.studioTextSmall
                            }

                            TextField {
                                id: startBitField

                                width: parent.width
                                text: "0"
                                selectByMouse: true
                                color: Theme.textPrimary
                                font.pixelSize: Theme.studioTextBody
                                font.family: "monospace"
                                inputMethodHints: Qt.ImhDigitsOnly
                                background: Rectangle {
                                    radius: Theme.radiusSmall
                                    color: root.inputBackground(
                                               root.parsedStartBit >= 0)
                                    border.color: root.inputBorderColor(
                                                      root.parsedStartBit >= 0)
                                    border.width: 1
                                }
                            }
                        }

                        Column {
                            width: (parent.width
                                    - parent.columnSpacing
                                    * (parent.columns - 1))
                                   / parent.columns
                            spacing: Theme.spacingXSmall

                            Text {
                                text: "BIT LENGTH"
                                color: Theme.textFaint
                                font.pixelSize: Theme.studioTextSmall
                            }

                            TextField {
                                id: bitLengthField

                                width: parent.width
                                text: "8"
                                selectByMouse: true
                                color: Theme.textPrimary
                                font.pixelSize: Theme.studioTextBody
                                font.family: "monospace"
                                inputMethodHints: Qt.ImhDigitsOnly
                                background: Rectangle {
                                    radius: Theme.radiusSmall
                                    color: root.inputBackground(
                                               root.parsedBitLength > 0)
                                    border.color: root.inputBorderColor(
                                                      root.parsedBitLength > 0)
                                    border.width: 1
                                }
                            }
                        }

                        Column {
                            width: (parent.width
                                    - parent.columnSpacing
                                    * (parent.columns - 1))
                                   / parent.columns
                            spacing: Theme.spacingXSmall

                            Text {
                                text: "ENDIAN"
                                color: Theme.textFaint
                                font.pixelSize: Theme.studioTextSmall
                            }

                            Row {
                                spacing: Theme.spacingSmall

                                RadioButton {
                                    text: "Little"
                                    checked: root.littleEndian

                                    onClicked: root.littleEndian = true

                                    contentItem: Text {
                                        text: parent.text
                                        leftPadding: parent.indicator.width
                                                     + Theme.spacingSmall
                                        color: Theme.textPrimary
                                        font.pixelSize: Theme.studioTextBody
                                        verticalAlignment: Text.AlignVCenter
                                    }

                                    indicator: Rectangle {
                                        implicitWidth: 16
                                        implicitHeight: 16
                                        x: 0
                                        y: parent.height / 2
                                           - height / 2
                                        radius: width / 2
                                        color: Theme.surfaceInset
                                        border.color: parent.checked
                                                      ? Theme.accent
                                                      : Theme.borderStrong
                                        border.width: 1

                                        Rectangle {
                                            anchors.centerIn: parent
                                            width: 8
                                            height: 8
                                            radius: width / 2
                                            color: Theme.accent
                                            visible: parent.parent.checked
                                        }
                                    }
                                }

                                RadioButton {
                                    text: "Big"
                                    checked: !root.littleEndian

                                    onClicked: root.littleEndian = false

                                    contentItem: Text {
                                        text: parent.text
                                        leftPadding: parent.indicator.width
                                                     + Theme.spacingSmall
                                        color: Theme.textPrimary
                                        font.pixelSize: Theme.studioTextBody
                                        verticalAlignment: Text.AlignVCenter
                                    }

                                    indicator: Rectangle {
                                        implicitWidth: 16
                                        implicitHeight: 16
                                        x: 0
                                        y: parent.height / 2
                                           - height / 2
                                        radius: width / 2
                                        color: Theme.surfaceInset
                                        border.color: parent.checked
                                                      ? Theme.accent
                                                      : Theme.borderStrong
                                        border.width: 1

                                        Rectangle {
                                            anchors.centerIn: parent
                                            width: 8
                                            height: 8
                                            radius: width / 2
                                            color: Theme.accent
                                            visible: parent.parent.checked
                                        }
                                    }
                                }
                            }
                        }

                        Column {
                            width: (parent.width
                                    - parent.columnSpacing
                                    * (parent.columns - 1))
                                   / parent.columns
                            spacing: Theme.spacingXSmall

                            Text {
                                text: "SIGNEDNESS"
                                color: Theme.textFaint
                                font.pixelSize: Theme.studioTextSmall
                            }

                            Row {
                                spacing: Theme.spacingSmall

                                RadioButton {
                                    text: "Unsigned"
                                    checked: !root.signedValue

                                    onClicked: root.signedValue = false

                                    contentItem: Text {
                                        text: parent.text
                                        leftPadding: parent.indicator.width
                                                     + Theme.spacingSmall
                                        color: Theme.textPrimary
                                        font.pixelSize: Theme.studioTextBody
                                        verticalAlignment: Text.AlignVCenter
                                    }

                                    indicator: Rectangle {
                                        implicitWidth: 16
                                        implicitHeight: 16
                                        x: 0
                                        y: parent.height / 2
                                           - height / 2
                                        radius: width / 2
                                        color: Theme.surfaceInset
                                        border.color: parent.checked
                                                      ? Theme.accent
                                                      : Theme.borderStrong
                                        border.width: 1

                                        Rectangle {
                                            anchors.centerIn: parent
                                            width: 8
                                            height: 8
                                            radius: width / 2
                                            color: Theme.accent
                                            visible: parent.parent.checked
                                        }
                                    }
                                }

                                RadioButton {
                                    text: "Signed"
                                    checked: root.signedValue

                                    onClicked: root.signedValue = true

                                    contentItem: Text {
                                        text: parent.text
                                        leftPadding: parent.indicator.width
                                                     + Theme.spacingSmall
                                        color: Theme.textPrimary
                                        font.pixelSize: Theme.studioTextBody
                                        verticalAlignment: Text.AlignVCenter
                                    }

                                    indicator: Rectangle {
                                        implicitWidth: 16
                                        implicitHeight: 16
                                        x: 0
                                        y: parent.height / 2
                                           - height / 2
                                        radius: width / 2
                                        color: Theme.surfaceInset
                                        border.color: parent.checked
                                                      ? Theme.accent
                                                      : Theme.borderStrong
                                        border.width: 1

                                        Rectangle {
                                            anchors.centerIn: parent
                                            width: 8
                                            height: 8
                                            radius: width / 2
                                            color: Theme.accent
                                            visible: parent.parent.checked
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Text {
                        visible: !root.candidateInputValid
                                 || root.analysisRequestError.length > 0
                        width: parent.width
                        text: root.analysisRequestError.length > 0
                              ? root.analysisRequestError
                              : root.inputErrorText()
                        color: Theme.warning
                        font.pixelSize: Theme.studioTextSmall
                        wrapMode: Text.WordWrap
                    }

                    Button {
                        id: analyzeButton

                        text: root.usesLiveChangeSnapshot
                              ? "Analyze snapshot evidence"
                              : "Analyze demo evidence"
                        enabled: root.candidateInputValid
                        implicitHeight: Math.max(
                                            Theme.studioTextBody
                                            + Theme.studioSpacingMedium,
                                            36)

                        onClicked: {
                            root.triggerAnalysis()
                        }

                        contentItem: Text {
                            text: analyzeButton.text
                            color: analyzeButton.enabled
                                   ? Theme.textInverse
                                   : Theme.textDisabled
                            font.pixelSize: Theme.studioTextBody
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            radius: Theme.radiusSmall
                            color: !analyzeButton.enabled
                                   ? Theme.disabled
                                   : analyzeButton.down
                                     ? Theme.pressed
                                     : analyzeButton.hovered
                                       ? Theme.accentMuted
                                       : Theme.accent
                            border.color: analyzeButton.enabled
                                          ? Theme.accent
                                          : Theme.disabled
                            border.width: 1
                        }
                    }
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
                            text: "Analyzed candidate identity"
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

            Rectangle {
                width: parent.width
                height: rangeSummaryContent.implicitHeight
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
                    id: rangeSummaryContent

                    anchors.fill: parent
                    anchors.margins: Theme.studioSpacingLarge
                    anchors.leftMargin: Theme.studioSpacingLarge
                                        + Theme.studioSpacingSmall
                    spacing: Theme.studioSpacingMedium

                    Row {
                        width: parent.width
                        spacing: Theme.studioSpacingSmall

                        Column {
                            width: parent.width - rangeEvidenceBadge.width
                                   - Theme.studioSpacingSmall
                            spacing: Theme.spacingXSmall

                            Text {
                                text: "Range summary evidence"
                                color: Theme.textPrimary
                                font.pixelSize: Theme.studioTextSection
                                font.bold: true
                            }

                            Text {
                                width: parent.width
                                text: root.presentation.hasEvidence
                                      ? (root.presentation.isRanging
                                         ? "Continuous or smooth numeric signal behavior observed"
                                         : "Discrete or step-like signal behavior observed")
                                      : "No accepted samples available"
                                color: Theme.textMuted
                                font.pixelSize: Theme.studioTextBody
                                wrapMode: Text.WordWrap
                            }
                        }

                        Rectangle {
                            id: rangeEvidenceBadge
                            width: rangeEvidenceLabel.implicitWidth
                                   + Theme.studioSpacingMedium * 2
                            height: rangeEvidenceLabel.implicitHeight
                                    + Theme.spacingSmall
                            radius: Theme.radiusPill
                            color: Theme.readOnlySubtle
                            border.color: Theme.readOnlyBorder
                            border.width: 1

                            Text {
                                id: rangeEvidenceLabel
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
                        columns: width >= 780 ? 6 : (width >= 500 ? 3 : 2)
                        columnSpacing: Theme.studioSpacingMedium
                        rowSpacing: Theme.studioSpacingMedium

                        Repeater {
                            model: [
                                { "label": "MIN VALUE", "value": root.presentation.hasEvidence ? root.presentation.minValueText : "-" },
                                { "label": "MAX VALUE", "value": root.presentation.hasEvidence ? root.presentation.maxValueText : "-" },
                                { "label": "RANGE SPAN", "value": root.presentation.hasEvidence ? root.presentation.rangeSpanText : "-" },
                                { "label": "COVERAGE", "value": root.presentation.hasEvidence ? root.presentation.rangeCoverageText : "-" },
                                { "label": "UNIQUE VALUES", "value": root.presentation.hasEvidence ? root.presentation.uniqueValueCount : "-" },
                                { "label": "SMOOTHNESS", "value": root.presentation.hasEvidence ? root.presentation.smoothnessScoreText : "-" }
                            ]

                            delegate: Rectangle {
                                width: (parent.width
                                        - parent.columnSpacing
                                        * (parent.columns - 1))
                                       / parent.columns
                                height: blockCol.implicitHeight + Theme.spacingMedium * 2
                                radius: Theme.radiusSmall
                                color: Theme.surfaceInset
                                border.color: Theme.borderStrong
                                border.width: 1

                                Column {
                                    id: blockCol
                                    anchors.fill: parent
                                    anchors.margins: Theme.spacingMedium
                                    spacing: Theme.spacingXSmall

                                    Text {
                                        text: modelData.label
                                        color: Theme.textFaint
                                        font.pixelSize: Theme.studioTextSmall
                                        font.bold: true
                                        elide: Text.ElideRight
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
