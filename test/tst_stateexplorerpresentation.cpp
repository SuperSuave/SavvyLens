#include "tst_stateexplorerpresentation.h"

// SavvyLens headers
#include "analysis/selectioncontext.h"
#include "app/stateexplorerpresentation.h"
#include "can/can_structs.h"

// Qt headers
#include <QByteArray>
#include <QSignalSpy>
#include <QVariantList>
#include <QVariantMap>
#include <QtTest>

namespace
{
CANFrame makeFrame(quint32 canId, const QByteArray &payload)
{
    CANFrame frame;
    frame.setFrameId(canId);
    frame.setPayload(payload);
    return frame;
}

CANFrame makeByteFrame(quint32 canId, quint8 value)
{
    return makeFrame(canId, QByteArray(1, static_cast<char>(value)));
}

CandidateAnalysis::Config makeConfig(
    quint32 canId,
    int startBit = 0,
    int bitLength = 8,
    bool littleEndian = true,
    bool isSigned = false)
{
    CandidateAnalysis::Config config;
    config.candidate.canId = canId;
    config.candidate.startBit = startBit;
    config.candidate.bitLength = bitLength;
    config.candidate.isLittleEndian = littleEndian;
    config.candidate.isSigned = isSigned;
    config.discreteState.maxDistinctValues = 32;
    config.maximumDistinctTransitions = 64;
    config.maximumRetainedRuns = 64;
    return config;
}

QVariantMap rowAt(const QVariantList &rows, int index)
    {
        return rows.at(index).toMap();
    }
}

void TestStateExplorerPresentation::mapsCandidateIdentityAndAcceptedSamples()
{
    StateExplorerPresentation presentation;

    QVector<CANFrame> frames;
    frames.append(makeByteFrame(0x000, 4));
    frames.append(makeByteFrame(0x000, 4));
    frames.append(makeByteFrame(0x000, 7));

    // Must not contribute accepted samples.
    frames.append(makeByteFrame(0x123, 99));

    CandidateAnalysis::Config config =
        makeConfig(0x000, 0, 8, true, false);

    presentation.setEvidence(frames, config);

    QCOMPARE(presentation.canIdText(), QStringLiteral("0x000"));
    QCOMPARE(presentation.startBit(), 0);
    QCOMPARE(presentation.bitLength(), 8);
    QCOMPARE(presentation.endianText(), QStringLiteral("Little endian"));
    QCOMPARE(presentation.signednessText(), QStringLiteral("Unsigned"));

    QCOMPARE(presentation.acceptedSampleCount(), 3);
    QVERIFY(presentation.hasEvidence());

    QCOMPARE(presentation.discreteClassificationText(),
             QStringLiteral("Bounded observed values"));
    QVERIFY(presentation.discreteCompleted());
    QVERIFY(!presentation.discreteTruncated());

    QCOMPARE(presentation.observedStates().size(), 2);
}

void TestStateExplorerPresentation::preservesEvidenceOrderingAndStringValues()
{
    StateExplorerPresentation presentation;

    constexpr quint32 canId = 0x321;

    QVector<CANFrame> frames;
    frames.append(makeByteFrame(canId, 0));
    frames.append(makeByteFrame(canId, 0));
    frames.append(makeByteFrame(canId, 1));
    frames.append(makeByteFrame(canId, 1));
    frames.append(makeByteFrame(canId, 2));
    frames.append(makeByteFrame(canId, 2));
    frames.append(makeByteFrame(canId, 1));
    frames.append(makeByteFrame(canId, 1));
    frames.append(makeByteFrame(canId, 0));

    presentation.setEvidence(frames, makeConfig(canId));

    const QVariantList states = presentation.observedStates();
    QCOMPARE(states.size(), 3);

    QCOMPARE(rowAt(states, 0).value(QStringLiteral("valueText")).toString(),
             QStringLiteral("0"));
    QCOMPARE(rowAt(states, 1).value(QStringLiteral("valueText")).toString(),
             QStringLiteral("1"));
    QCOMPARE(rowAt(states, 2).value(QStringLiteral("valueText")).toString(),
             QStringLiteral("2"));

    QCOMPARE(rowAt(states, 0).value(QStringLiteral("occurrenceCount")).toInt(),
             3);
    QCOMPARE(rowAt(states, 1).value(QStringLiteral("occurrenceCount")).toInt(),
             4);
    QCOMPARE(rowAt(states, 2).value(QStringLiteral("occurrenceCount")).toInt(),
             2);

    const QVariantList transitions = presentation.observedTransitions();
    QCOMPARE(transitions.size(), 4);

    // TransitionAnalysis ordering is deterministic by directed pair,
    // not temporal encounter order.
    QCOMPARE(rowAt(transitions, 0)
                 .value(QStringLiteral("fromValueText"))
                 .toString(),
             QStringLiteral("0"));
    QCOMPARE(rowAt(transitions, 0)
                 .value(QStringLiteral("toValueText"))
                 .toString(),
             QStringLiteral("1"));

    QCOMPARE(rowAt(transitions, 1)
                 .value(QStringLiteral("fromValueText"))
                 .toString(),
             QStringLiteral("1"));
    QCOMPARE(rowAt(transitions, 1)
                 .value(QStringLiteral("toValueText"))
                 .toString(),
             QStringLiteral("0"));

    QCOMPARE(rowAt(transitions, 2)
                 .value(QStringLiteral("fromValueText"))
                 .toString(),
             QStringLiteral("1"));
    QCOMPARE(rowAt(transitions, 2)
                 .value(QStringLiteral("toValueText"))
                 .toString(),
             QStringLiteral("2"));

    QCOMPARE(rowAt(transitions, 3)
                 .value(QStringLiteral("fromValueText"))
                 .toString(),
             QStringLiteral("2"));
    QCOMPARE(rowAt(transitions, 3)
                 .value(QStringLiteral("toValueText"))
                 .toString(),
             QStringLiteral("1"));

    const QVariantList runs = presentation.observedRuns();
    QCOMPARE(runs.size(), 5);

    // TemporalAnalysis preserves consecutive-run encounter order.
    QCOMPARE(rowAt(runs, 0).value(QStringLiteral("valueText")).toString(),
             QStringLiteral("0"));
    QCOMPARE(rowAt(runs, 0).value(QStringLiteral("sampleCount")).toInt(), 2);

    QCOMPARE(rowAt(runs, 1).value(QStringLiteral("valueText")).toString(),
             QStringLiteral("1"));
    QCOMPARE(rowAt(runs, 1).value(QStringLiteral("sampleCount")).toInt(), 2);

    QCOMPARE(rowAt(runs, 2).value(QStringLiteral("valueText")).toString(),
             QStringLiteral("2"));
    QCOMPARE(rowAt(runs, 2).value(QStringLiteral("sampleCount")).toInt(), 2);

    QCOMPARE(rowAt(runs, 3).value(QStringLiteral("valueText")).toString(),
             QStringLiteral("1"));
    QCOMPARE(rowAt(runs, 3).value(QStringLiteral("sampleCount")).toInt(), 2);

    QCOMPARE(rowAt(runs, 4).value(QStringLiteral("valueText")).toString(),
             QStringLiteral("0"));
    QCOMPARE(rowAt(runs, 4).value(QStringLiteral("sampleCount")).toInt(), 1);
}

void TestStateExplorerPresentation::mapsNoAcceptedSamplesSafely()
{
    StateExplorerPresentation presentation;

    QVector<CANFrame> frames;
    frames.append(makeByteFrame(0x100, 1));
    frames.append(makeByteFrame(0x101, 2));

    presentation.setEvidence(frames, makeConfig(0x000));

    QCOMPARE(presentation.canIdText(), QStringLiteral("0x000"));
    QCOMPARE(presentation.acceptedSampleCount(), 0);
    QVERIFY(!presentation.hasEvidence());

    QCOMPARE(presentation.discreteClassificationText(),
             QStringLiteral("No accepted samples"));
    QCOMPARE(presentation.transitionClassificationText(),
             QStringLiteral("No accepted samples"));
    QCOMPARE(presentation.temporalClassificationText(),
             QStringLiteral("No accepted samples"));

    QVERIFY(presentation.discreteCompleted());
    QVERIFY(presentation.transitionCompleted());
    QVERIFY(presentation.temporalCompleted());

    QVERIFY(!presentation.discreteTruncated());
    QVERIFY(!presentation.transitionTruncated());
    QVERIFY(!presentation.temporalTruncated());

    QVERIFY(presentation.observedStates().isEmpty());
    QVERIFY(presentation.observedTransitions().isEmpty());
    QVERIFY(presentation.observedRuns().isEmpty());
}

void TestStateExplorerPresentation::mapsTruncatedEvidenceFlags()
{
    StateExplorerPresentation presentation;

    constexpr quint32 canId = 0x222;

    QVector<CANFrame> frames;
    frames.append(makeByteFrame(canId, 0));
    frames.append(makeByteFrame(canId, 1));
    frames.append(makeByteFrame(canId, 2));
    frames.append(makeByteFrame(canId, 3));

    CandidateAnalysis::Config config = makeConfig(canId);
    config.discreteState.maxDistinctValues = 2;
    config.maximumDistinctTransitions = 2;
    config.maximumRetainedRuns = 2;

    presentation.setEvidence(frames, config);

    QCOMPARE(presentation.acceptedSampleCount(), 4);

    QVERIFY(presentation.discreteTruncated());
    QVERIFY(!presentation.discreteCompleted());

    QVERIFY(presentation.transitionTruncated());
    QVERIFY(!presentation.transitionCompleted());

    QVERIFY(presentation.temporalTruncated());
    QVERIFY(!presentation.temporalCompleted());

    QCOMPARE(presentation.discreteClassificationText(),
             QStringLiteral("High cardinality or incomplete evidence"));
    QCOMPARE(presentation.transitionClassificationText(),
             QStringLiteral("High cardinality or incomplete evidence"));
    QCOMPARE(presentation.temporalClassificationText(),
             QStringLiteral("High cardinality or incomplete evidence"));

    QCOMPARE(presentation.observedStates().size(), 2);
    QCOMPARE(presentation.observedTransitions().size(), 2);
    QCOMPARE(presentation.observedRuns().size(), 2);
}

bool setExplicitCandidateEvidence(StateExplorerPresentation &presentation,
                                  const QVector<CANFrame> &frames,
                                  quint32 canId,
                                  int startBit,
                                  int bitLength,
                                  bool isLittleEndian,
                                  bool isSigned)
{
    RangeSignalSpec candidate;
    candidate.canId = canId;
    candidate.startBit = startBit;
    candidate.bitLength = bitLength;
    candidate.isLittleEndian = isLittleEndian;
    candidate.isSigned = isSigned;

    if (!candidate.isValid())
        return false;

    CandidateAnalysis::Config config;
    config.candidate = candidate;
    config.discreteState.maxDistinctValues = 32;
    config.maximumDistinctTransitions = 64;
    config.maximumRetainedRuns = 64;

    presentation.setEvidence(frames, config);
    return true;
}

void TestStateExplorerPresentation::explicitCandidateRefreshesIdentityAndEvidence()
{
    StateExplorerPresentation presentation;
    const QVector<CANFrame> frames = {
        makeFrame(0x321, QByteArray::fromHex("00")),
        makeFrame(0x321, QByteArray::fromHex("01")),
        makeFrame(0x321, QByteArray::fromHex("02")),
        makeFrame(0x321, QByteArray::fromHex("01"))
    };

    QVERIFY(setExplicitCandidateEvidence(
        presentation,
        frames,
        0x321,
        0,
        8,
        true,
        false));

    QCOMPARE(presentation.canIdText(), QStringLiteral("0x321"));
    QCOMPARE(presentation.startBit(), 0);
    QCOMPARE(presentation.bitLength(), 8);
    QCOMPARE(presentation.endianText(), QStringLiteral("Little endian"));
    QCOMPARE(presentation.signednessText(), QStringLiteral("Unsigned"));
    QCOMPARE(presentation.acceptedSampleCount(), 4);
    QVERIFY(presentation.hasEvidence());
    QCOMPARE(presentation.observedStates().size(), 3);
    QCOMPARE(presentation.observedTransitions().size(), 3);
    QCOMPARE(presentation.observedRuns().size(), 4);
}

void TestStateExplorerPresentation::invalidCandidateIsRejectedBeforePresentationRefresh()
{
    StateExplorerPresentation presentation;
    const QVector<CANFrame> frames = {
        makeFrame(0x321, QByteArray::fromHex("00")),
        makeFrame(0x321, QByteArray::fromHex("01"))
    };

    QVERIFY(setExplicitCandidateEvidence(
        presentation,
        frames,
        0x321,
        0,
        8,
        true,
        false));

    const QString previousCanId = presentation.canIdText();
    const int previousStartBit = presentation.startBit();
    const int previousBitLength = presentation.bitLength();
    const int previousSampleCount = presentation.acceptedSampleCount();
    const QVariantList previousStates = presentation.observedStates();

    QVERIFY(!setExplicitCandidateEvidence(
        presentation,
        frames,
        0x321,
        0,
        0,
        true,
        false));

    QCOMPARE(presentation.canIdText(), previousCanId);
    QCOMPARE(presentation.startBit(), previousStartBit);
    QCOMPARE(presentation.bitLength(), previousBitLength);
    QCOMPARE(presentation.acceptedSampleCount(), previousSampleCount);
    QCOMPARE(presentation.observedStates(), previousStates);
}

void TestStateExplorerPresentation::canIdZeroRemainsSupported()
{
    StateExplorerPresentation presentation;
    const QVector<CANFrame> frames = {
        makeFrame(0x000, QByteArray::fromHex("02")),
        makeFrame(0x000, QByteArray::fromHex("03"))
    };

    QVERIFY(setExplicitCandidateEvidence(
        presentation,
        frames,
        0x000,
        0,
        8,
        true,
        false));

    QCOMPARE(presentation.canIdText(), QStringLiteral("0x000"));
    QCOMPARE(presentation.acceptedSampleCount(), 2);
    QVERIFY(presentation.hasEvidence());
}

void TestStateExplorerPresentation::explicitEndianAndSignednessAreForwarded()
{
    StateExplorerPresentation presentation;
    const QVector<CANFrame> frames = {
        makeFrame(0x321, QByteArray::fromHex("80"))
    };

    QVERIFY(setExplicitCandidateEvidence(
        presentation,
        frames,
        0x321,
        7,
        8,
        false,
        true));

    QCOMPARE(presentation.endianText(), QStringLiteral("Big endian"));
    QCOMPARE(presentation.signednessText(), QStringLiteral("Signed"));
    QCOMPARE(presentation.acceptedSampleCount(), 1);

    const QVariantList states = presentation.observedStates();
    QCOMPARE(states.size(), 1);
    QCOMPARE(states.constFirst().toMap()
             .value(QStringLiteral("valueText"))
             .toString(),
             QStringLiteral("-128"));
}

void TestStateExplorerPresentation::validCandidateWithNoAcceptedSamplesIsReadable()
{
    StateExplorerPresentation presentation;
    const QVector<CANFrame> frames = {
        makeFrame(0x321, QByteArray::fromHex("00")),
        makeFrame(0x321, QByteArray::fromHex("01"))
    };

    QVERIFY(setExplicitCandidateEvidence(
        presentation,
        frames,
        0x000,
        0,
        8,
        true,
        false));

    QCOMPARE(presentation.canIdText(), QStringLiteral("0x000"));
    QCOMPARE(presentation.acceptedSampleCount(), 0);
    QVERIFY(!presentation.hasEvidence());
    QCOMPARE(presentation.discreteClassificationText(),
             QStringLiteral("No accepted samples"));
    QCOMPARE(presentation.transitionClassificationText(),
             QStringLiteral("No accepted samples"));
    QCOMPARE(presentation.temporalClassificationText(),
             QStringLiteral("No accepted samples"));
}
void TestStateExplorerPresentation::sourcePresentationDistinguishesDemoAndSnapshot()
{
    StateExplorerPresentation presentation;
    QSignalSpy changedSpy(&presentation, &StateExplorerPresentation::changed);

    presentation.setDeterministicDemoSource();

    QCOMPARE(presentation.sourceLabel(),
             QStringLiteral("Source: Deterministic demo evidence"));
    QCOMPARE(
        presentation.sourceScopeText(),
        QStringLiteral(
            "Deterministic in-memory demo evidence. It is not captured "
            "traffic and does not update from live traffic or session "
            "history."));
    QVERIFY(!presentation.usesLiveChangeSnapshot());
    QCOMPARE(changedSpy.count(), 1);

    const QString snapshotLabel =
        QStringLiteral(
            "Source: Live Change Explorer snapshot · "
            "Bus 1 · Rx · Standard · Data · 0x000");

    const QString snapshotScopeText =
        QStringLiteral(
            "Bounded retained evidence snapshot. It is not live traffic "
            "and may exclude older matching frames that aged out of the "
            "session-wide rolling buffer.");

    presentation.setLiveChangeExplorerSnapshotSource(
        snapshotLabel,
        snapshotScopeText);

    QCOMPARE(presentation.sourceLabel(), snapshotLabel);
    QCOMPARE(presentation.sourceScopeText(), snapshotScopeText);
    QVERIFY(presentation.usesLiveChangeSnapshot());
    QCOMPARE(changedSpy.count(), 2);
}

void TestStateExplorerPresentation::preservesCandidateParametersAcrossSnapshotRefetches()
{
    StateExplorerPresentation presentation;

    QVector<CANFrame> frames;
    frames.append(makeByteFrame(0x123, 4));

    CandidateAnalysis::Config config =
        makeConfig(0x123, 16, 12, false, true);

    presentation.setEvidence(frames, config);

    QCOMPARE(presentation.canIdText(), QStringLiteral("0x123"));
    QCOMPARE(presentation.startBit(), 16);
    QCOMPARE(presentation.bitLength(), 12);
    QCOMPARE(presentation.endianText(), QStringLiteral("Big endian"));
    QCOMPARE(presentation.signednessText(), QStringLiteral("Signed"));

    // Simulate a snapshot refetch, updating the source presentation
    QSignalSpy spy(&presentation, &StateExplorerPresentation::changed);
    presentation.setLiveChangeExplorerSnapshotSource(
        QStringLiteral("Refetched label"),
        QStringLiteral("Refetched scope"));

    QCOMPARE(spy.count(), 1);

    // Verify candidate parameters are preserved after the source updates
    QCOMPARE(presentation.canIdText(), QStringLiteral("0x123"));
    QCOMPARE(presentation.startBit(), 16);
    QCOMPARE(presentation.bitLength(), 12);
    QCOMPARE(presentation.endianText(), QStringLiteral("Big endian"));
    QCOMPARE(presentation.signednessText(), QStringLiteral("Signed"));
}

void TestStateExplorerPresentation::sourcePresentationChangesWithoutRefreshingEvidence()
{
    StateExplorerPresentation presentation;

    const QVector<CANFrame> frames = {
        makeByteFrame(0x321, 1),
        makeByteFrame(0x321, 2)
    };

    presentation.setEvidence(frames, makeConfig(0x321));

    const QString candidateCanId = presentation.canIdText();
    const int acceptedSampleCount = presentation.acceptedSampleCount();
    const QVariantList observedStates = presentation.observedStates();

    presentation.setLiveChangeExplorerSnapshotSource(
        QStringLiteral(
            "Source: Live Change Explorer snapshot · "
            "Bus 2 · Tx · Extended · RTR · 0x18DAF110"),
        QStringLiteral(
            "Bounded retained evidence snapshot. It is not live traffic "
            "and may exclude older matching frames that aged out of the "
            "session-wide rolling buffer."));

    QCOMPARE(presentation.canIdText(), candidateCanId);
    QCOMPARE(presentation.acceptedSampleCount(), acceptedSampleCount);
    QCOMPARE(presentation.observedStates(), observedStates);
    QVERIFY(presentation.usesLiveChangeSnapshot());
}

void TestStateExplorerPresentation::rangeSummarySurfacesMetricsAndStringFormatting()
{
    StateExplorerPresentation presentation;

    QVector<CANFrame> frames;
    frames.append(makeByteFrame(0x123, 10));
    frames.append(makeByteFrame(0x123, 20));
    frames.append(makeByteFrame(0x123, 30));
    frames.append(makeByteFrame(0x123, 20));

    CandidateAnalysis::Config config = makeConfig(0x123);
    presentation.setEvidence(frames, config);

    QVERIFY(presentation.hasEvidence());
    QCOMPARE(presentation.minValueText(), QStringLiteral("10"));
    QCOMPARE(presentation.maxValueText(), QStringLiteral("30"));
    QCOMPARE(presentation.rangeSpanText(), QStringLiteral("20"));
    QVERIFY(presentation.rangeCoverageText().endsWith(QStringLiteral("%")));
    QCOMPARE(presentation.uniqueValueCount(), 3);
}

void TestStateExplorerPresentation::rangeSummaryHandlesZeroSampleEvidenceSafely()
{
    StateExplorerPresentation presentation;

    QVector<CANFrame> frames;
    // No frames matching candidate CAN ID 0x123
    frames.append(makeByteFrame(0x456, 10));

    CandidateAnalysis::Config config = makeConfig(0x123);
    presentation.setEvidence(frames, config);

    QVERIFY(!presentation.hasEvidence());
    QCOMPARE(presentation.minValueText(), QStringLiteral("0"));
    QCOMPARE(presentation.maxValueText(), QStringLiteral("0"));
    QCOMPARE(presentation.rangeSpanText(), QStringLiteral("0"));
    QCOMPARE(presentation.rangeCoverageText(), QStringLiteral("0.0%"));
    QCOMPARE(presentation.uniqueValueCount(), 0);
    QVERIFY(!presentation.isRanging());
}

void TestStateExplorerPresentation::seedsCandidateFromSelectionContextBitRange()
{
    SelectionContext context;
    context.setCanId(0x250);
    context.setBitRange(16, 16);

    QVERIFY(context.bitRange().isValid());
    QCOMPARE(context.bitRange().startBit, 16);
    QCOMPARE(context.bitRange().bitLength, 16);

    CandidateAnalysis::Config config;
    config.candidate.canId = context.canId();
    config.candidate.startBit = context.bitRange().startBit;
    config.candidate.bitLength = context.bitRange().bitLength;
    config.candidate.isLittleEndian = true;
    config.candidate.isSigned = false;

    // Frame with 4 bytes payload: 0x00, 0x00, 0x34, 0x12 -> bits 16..31 little endian = 0x1234 (4660)
    QVector<CANFrame> frames;
    frames.append(makeFrame(0x250, QByteArray::fromHex("00003412")));

    StateExplorerPresentation presentation;
    presentation.setEvidence(frames, config);

    QCOMPARE(presentation.canIdText(), QStringLiteral("0x250"));
    QCOMPARE(presentation.startBit(), 16);
    QCOMPARE(presentation.bitLength(), 16);
    QCOMPARE(presentation.endianText(), QStringLiteral("Little endian"));
    QCOMPARE(presentation.signednessText(), QStringLiteral("Unsigned"));
    QCOMPARE(presentation.acceptedSampleCount(), 1);
    QVERIFY(presentation.hasEvidence());

    const QVariantList states = presentation.observedStates();
    QCOMPARE(states.size(), 1);
    QCOMPARE(states.constFirst().toMap().value(QStringLiteral("valueText")).toString(),
             QStringLiteral("4660"));
}

void TestStateExplorerPresentation::fallbacksToDefaultBitfieldWhenContextLacksBitRange()
{
    SelectionContext context;
    context.setCanId(0x100);

    QVERIFY(!context.bitRange().isValid());

    // Seeding logic fallbacks to startBit=0, bitLength=8, isLittleEndian=true, isSigned=false
    const int startBit = context.bitRange().isValid() ? context.bitRange().startBit : 0;
    const int bitLength = context.bitRange().isValid() ? context.bitRange().bitLength : 8;
    const bool isLittleEndian = true;
    const bool isSigned = false;

    QCOMPARE(startBit, 0);
    QCOMPARE(bitLength, 8);

    CandidateAnalysis::Config config =
        makeConfig(context.canId(), startBit, bitLength, isLittleEndian, isSigned);

    QVector<CANFrame> frames;
    frames.append(makeByteFrame(0x100, 0x42));

    StateExplorerPresentation presentation;
    presentation.setEvidence(frames, config);

    QCOMPARE(presentation.canIdText(), QStringLiteral("0x100"));
    QCOMPARE(presentation.startBit(), 0);
    QCOMPARE(presentation.bitLength(), 8);
    QCOMPARE(presentation.endianText(), QStringLiteral("Little endian"));
    QCOMPARE(presentation.signednessText(), QStringLiteral("Unsigned"));
    QCOMPARE(presentation.acceptedSampleCount(), 1);

    const QVariantList states = presentation.observedStates();
    QCOMPARE(states.size(), 1);
    QCOMPARE(states.constFirst().toMap().value(QStringLiteral("valueText")).toString(),
             QStringLiteral("66"));
}

void TestStateExplorerPresentation::preservesUserModificationsAfterContextSeeding()
{
    SelectionContext context;
    context.setCanId(0x300);
    context.setBitRange(0, 8);

    CandidateAnalysis::Config seededConfig;
    seededConfig.candidate.canId = context.canId();
    seededConfig.candidate.startBit = context.bitRange().startBit;
    seededConfig.candidate.bitLength = context.bitRange().bitLength;
    seededConfig.candidate.isLittleEndian = true;
    seededConfig.candidate.isSigned = false;

    QVector<CANFrame> frames;
    frames.append(makeFrame(0x300, QByteArray::fromHex("00FE")));

    StateExplorerPresentation presentation;
    presentation.setEvidence(frames, seededConfig);

    QCOMPARE(presentation.canIdText(), QStringLiteral("0x300"));
    QCOMPARE(presentation.startBit(), 0);
    QCOMPARE(presentation.bitLength(), 8);
    QCOMPARE(presentation.signednessText(), QStringLiteral("Unsigned"));

    // User modifies candidate parameters: starts at bit 8, length 8, signed
    CandidateAnalysis::Config userModifiedConfig = seededConfig;
    userModifiedConfig.candidate.startBit = 8;
    userModifiedConfig.candidate.bitLength = 8;
    userModifiedConfig.candidate.isSigned = true;

    presentation.setEvidence(frames, userModifiedConfig);

    // Presentation reflects the user's manual modifications and re-analyzed evidence
    QCOMPARE(presentation.canIdText(), QStringLiteral("0x300"));
    QCOMPARE(presentation.startBit(), 8);
    QCOMPARE(presentation.bitLength(), 8);
    QCOMPARE(presentation.signednessText(), QStringLiteral("Signed"));

    const QVariantList states = presentation.observedStates();
    QCOMPARE(states.size(), 1);
    // 0xFE as signed 8-bit is -2
    QCOMPARE(states.constFirst().toMap().value(QStringLiteral("valueText")).toString(),
             QStringLiteral("-2"));
}

void TestStateExplorerPresentation::demoScenarioStaticProducesStaticClassification()
{
    StateExplorerPresentation presentation;
    constexpr quint32 canId = 0x321;
    
    QVector<CANFrame> frames;
    frames.append(makeByteFrame(canId, 5));
    frames.append(makeByteFrame(canId, 5));
    frames.append(makeByteFrame(canId, 5));
    frames.append(makeByteFrame(canId, 5));
    frames.append(makeByteFrame(canId, 5));
    
    presentation.setEvidence(frames, makeConfig(canId));
    
    QCOMPARE(presentation.acceptedSampleCount(), 5);
    QVERIFY(presentation.hasEvidence());
    QCOMPARE(presentation.discreteClassificationText(), QStringLiteral("Static observed value"));
    
    const QVariantList states = presentation.observedStates();
    QCOMPARE(states.size(), 1);
    QCOMPARE(rowAt(states, 0).value(QStringLiteral("valueText")).toString(), QStringLiteral("5"));
    QCOMPARE(rowAt(states, 0).value(QStringLiteral("occurrenceCount")).toInt(), 5);
    
    QVERIFY(presentation.observedTransitions().isEmpty());
    
    const QVariantList runs = presentation.observedRuns();
    QCOMPARE(runs.size(), 1);
    QCOMPARE(rowAt(runs, 0).value(QStringLiteral("valueText")).toString(), QStringLiteral("5"));
    QCOMPARE(rowAt(runs, 0).value(QStringLiteral("sampleCount")).toInt(), 5);
}

void TestStateExplorerPresentation::demoScenarioCounterProducesExpectedTransitionsAndRuns()
{
    StateExplorerPresentation presentation;
    constexpr quint32 canId = 0x321;
    
    QVector<CANFrame> frames;
    for (quint8 value = 0; value <= 7; ++value)
    {
        frames.append(makeByteFrame(canId, value));
    }
    
    presentation.setEvidence(frames, makeConfig(canId));
    
    QCOMPARE(presentation.acceptedSampleCount(), 8);
    QCOMPARE(presentation.observedStates().size(), 8);
    
    const QVariantList transitions = presentation.observedTransitions();
    QCOMPARE(transitions.size(), 7);
    QCOMPARE(rowAt(transitions, 0).value(QStringLiteral("fromValueText")).toString(), QStringLiteral("0"));
    QCOMPARE(rowAt(transitions, 0).value(QStringLiteral("toValueText")).toString(), QStringLiteral("1"));
    QCOMPARE(rowAt(transitions, 6).value(QStringLiteral("fromValueText")).toString(), QStringLiteral("6"));
    QCOMPARE(rowAt(transitions, 6).value(QStringLiteral("toValueText")).toString(), QStringLiteral("7"));
    
    const QVariantList runs = presentation.observedRuns();
    QCOMPARE(runs.size(), 8);
    QCOMPARE(rowAt(runs, 0).value(QStringLiteral("valueText")).toString(), QStringLiteral("0"));
    QCOMPARE(rowAt(runs, 0).value(QStringLiteral("sampleCount")).toInt(), 1);
}

void TestStateExplorerPresentation::demoScenarioEmptyProducesNoSamplesClassification()
{
    StateExplorerPresentation presentation;
    constexpr quint32 canId = 0x321;
    
    QVector<CANFrame> frames; // Empty
    
    presentation.setEvidence(frames, makeConfig(canId));
    
    QCOMPARE(presentation.acceptedSampleCount(), 0);
    QVERIFY(!presentation.hasEvidence());
    QCOMPARE(presentation.discreteClassificationText(), QStringLiteral("No accepted samples"));
    QCOMPARE(presentation.transitionClassificationText(), QStringLiteral("No accepted samples"));
    QCOMPARE(presentation.temporalClassificationText(), QStringLiteral("No accepted samples"));
}

void TestStateExplorerPresentation::generatesSelectionContextFromDiscreteStateRows()
{
    StateExplorerPresentation presentation;
    constexpr quint32 canId = 0x400;

    CANFrame frame1 = makeFrame(canId, QByteArray::fromHex("00123400"));
    frame1.bus = 2;
    CANFrame frame2 = makeFrame(canId, QByteArray::fromHex("00567800"));
    frame2.bus = 2;

    QVector<CANFrame> frames = { frame1, frame2 };

    CandidateAnalysis::Config config;
    config.candidate.canId = canId;
    config.candidate.startBit = 8;
    config.candidate.bitLength = 16;
    config.candidate.isLittleEndian = true;
    config.candidate.isSigned = false;

    presentation.setEvidence(frames, config);

    QCOMPARE(presentation.observedStates().size(), 2);

    // Query SelectionContext for state row 0
    const SelectionContext context0 = presentation.selectionContextForState(0);
    QVERIFY(!context0.isEmpty());
    QVERIFY(context0.hasSingleCanId());
    QCOMPARE(context0.canId(), canId);
    QVERIFY(context0.hasBus());
    QCOMPARE(context0.bus(), 2);
    QVERIFY(context0.bitRange().isValid());
    QCOMPARE(context0.bitRange().startBit, 8);
    QCOMPARE(context0.bitRange().bitLength, 16);
    QVERIFY(context0.hasFrameIndex());
    QCOMPARE(context0.frameIndex(), 0);

    // Query SelectionContext for state row 1
    const SelectionContext context1 = presentation.selectionContextForState(1);
    QVERIFY(!context1.isEmpty());
    QVERIFY(context1.hasSingleCanId());
    QCOMPARE(context1.canId(), canId);
    QVERIFY(context1.hasBus());
    QCOMPARE(context1.bus(), 2);
    QVERIFY(context1.bitRange().isValid());
    QCOMPARE(context1.bitRange().startBit, 8);
    QCOMPARE(context1.bitRange().bitLength, 16);
    QVERIFY(context1.hasFrameIndex());
    QCOMPARE(context1.frameIndex(), 1);
}

void TestStateExplorerPresentation::generatesSelectionContextFromTransitionRows()
{
    StateExplorerPresentation presentation;
    constexpr quint32 canId = 0x321;

    CANFrame frame1 = makeByteFrame(canId, 10);
    frame1.bus = 1;
    CANFrame frame2 = makeByteFrame(canId, 20);
    frame2.bus = 1;

    QVector<CANFrame> frames = { frame1, frame2 };

    CandidateAnalysis::Config config;
    config.candidate.canId = canId;
    config.candidate.startBit = 0;
    config.candidate.bitLength = 8;
    config.candidate.isLittleEndian = true;
    config.candidate.isSigned = false;

    presentation.setEvidence(frames, config);

    QCOMPARE(presentation.observedTransitions().size(), 1);

    const SelectionContext context = presentation.selectionContextForTransition(0);
    QVERIFY(!context.isEmpty());
    QVERIFY(context.hasSingleCanId());
    QCOMPARE(context.canId(), canId);
    QVERIFY(context.hasBus());
    QCOMPARE(context.bus(), 1);
    QVERIFY(context.bitRange().isValid());
    QCOMPARE(context.bitRange().startBit, 0);
    QCOMPARE(context.bitRange().bitLength, 8);
    QVERIFY(context.hasFrameIndex());
    QCOMPARE(context.frameIndex(), 1);
}

void TestStateExplorerPresentation::handlesOutOfBoundsSelectionContextQueriesSafely()
{
    StateExplorerPresentation uninitializedPresentation;

    // Uninitialized presentation safe fallback
    const SelectionContext uninitContextState =
        uninitializedPresentation.selectionContextForState(0);
    QVERIFY(uninitContextState.isEmpty());

    const SelectionContext uninitContextTrans =
        uninitializedPresentation.selectionContextForTransition(0);
    QVERIFY(uninitContextTrans.isEmpty());

    // Initialized presentation with evidence
    StateExplorerPresentation presentation;
    constexpr quint32 canId = 0x321;
    QVector<CANFrame> frames = { makeByteFrame(canId, 5) };
    presentation.setEvidence(frames, makeConfig(canId));

    // Negative indexes
    const SelectionContext negStateContext =
        presentation.selectionContextForState(-1);
    QVERIFY(negStateContext.isEmpty());

    const SelectionContext negTransContext =
        presentation.selectionContextForTransition(-1);
    QVERIFY(negTransContext.isEmpty());

    // Out-of-bounds positive indexes
    const SelectionContext oobStateContext =
        presentation.selectionContextForState(100);
    QVERIFY(oobStateContext.isEmpty());

    const SelectionContext oobTransContext =
        presentation.selectionContextForTransition(100);
    QVERIFY(oobTransContext.isEmpty());
}
