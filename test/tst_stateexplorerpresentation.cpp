#include "tst_stateexplorerpresentation.h"

// SavvyLens headers
#include "app/stateexplorerpresentation.h"
#include "can/can_structs.h"

// Qt headers
#include <QByteArray>
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
