#include "tst_candidateanalysis.h"

// SavvyLens headers
#include "analysis/candidateanalysis.h"
#include "can/can_structs.h"

// Qt headers
#include <QtTest>

namespace
{

CANFrame frame(quint32 id, const QByteArray &payload)
{
    CANFrame result;
    result.setFrameId(id);
    result.bus = 0;
    result.isReceived = true;
    result.setPayload(payload);
    return result;
}

CandidateAnalysis::Config oneByteConfig(quint32 canId = 0x100)
{
    CandidateAnalysis::Config config;
    config.candidate.canId = canId;
    config.candidate.startBit = 0;
    config.candidate.bitLength = 8;
    config.candidate.isLittleEndian = true;
    config.candidate.isSigned = false;
    return config;
}

} // namespace

void TestCandidateAnalysis::emptyInputIsConsistentAcrossAnalyses()
{
    const CandidateAnalysis::Config config = oneByteConfig();

    const CandidateAnalysis::Result result =
        CandidateAnalysis::analyze(QVector<CANFrame>(), config);

    QCOMPARE(result.candidate, config.candidate);

    QCOMPARE(result.discreteState.sampleCount, 0);
    QCOMPARE(result.discreteState.classification,
            DiscreteStateClassification::NoSamples);

    QCOMPARE(result.transitions.acceptedSampleCount, 0);
    QCOMPARE(result.transitions.classification,
            TransitionAnalysis::Classification::NoSamples);

    QCOMPARE(result.temporalRuns.acceptedSampleCount, 0);
    QCOMPARE(result.temporalRuns.classification,
            TemporalAnalysis::Classification::NoSamples);
}

void TestCandidateAnalysis::appliesOneCandidateToEveryAnalyzer()
{
    QVector<CANFrame> frames;
    frames.append(frame(0x100, QByteArray(1, char(1))));
    frames.append(frame(0x100, QByteArray(1, char(1))));
    frames.append(frame(0x100, QByteArray(1, char(2))));
    frames.append(frame(0x200, QByteArray(1, char(99))));

    const CandidateAnalysis::Config config = oneByteConfig(0x100);

    const CandidateAnalysis::Result result =
        CandidateAnalysis::analyze(frames, config);

    QCOMPARE(result.candidate, config.candidate);
    QCOMPARE(result.discreteState.spec, config.candidate);
    QCOMPARE(result.transitions.signal, config.candidate);
    QCOMPARE(result.temporalRuns.signal, config.candidate);

    QCOMPARE(result.discreteState.sampleCount, 3);
    QCOMPARE(result.transitions.acceptedSampleCount, 3);
    QCOMPARE(result.temporalRuns.acceptedSampleCount, 3);

    QCOMPARE(result.transitions.valueChangeCount, 1);
    QCOMPARE(result.temporalRuns.valueChangeCount, 1);
    QCOMPARE(result.temporalRuns.runs.count(), 2);
}

void TestCandidateAnalysis::forwardsIndependentEvidenceLimits()
{
    QVector<CANFrame> frames;
    frames.append(frame(0x100, QByteArray(1, char(0))));
    frames.append(frame(0x100, QByteArray(1, char(1))));
    frames.append(frame(0x100, QByteArray(1, char(2))));
    frames.append(frame(0x100, QByteArray(1, char(3))));

    CandidateAnalysis::Config config = oneByteConfig();
    config.discreteState.maxDistinctValues = 2;
    config.maximumDistinctTransitions = 2;
    config.maximumRetainedRuns = 2;

    const CandidateAnalysis::Result result =
        CandidateAnalysis::analyze(frames, config);

    QCOMPARE(result.discreteState.observedValues.count(), 2);
    QCOMPARE(result.discreteState.outcome,
            DiscreteStateAnalysisOutcome::TruncatedByDistinctValueLimit);
    QCOMPARE(result.discreteState.classification,
            DiscreteStateClassification::HighCardinalityOrTruncated);

    QCOMPARE(result.transitions.transitions.count(), 2);
    QVERIFY(!result.transitions.completed);
    QCOMPARE(result.transitions.classification,
            TransitionAnalysis::Classification::HighCardinalityOrTruncated);

    QCOMPARE(result.temporalRuns.runs.count(), 2);
    QVERIFY(!result.temporalRuns.completed);
    QCOMPARE(result.temporalRuns.classification,
            TemporalAnalysis::Classification::HighCardinalityOrTruncated);
}

void TestCandidateAnalysis::preservesSignedExtraction()
{
    QVector<CANFrame> frames;
    frames.append(frame(0x100, QByteArray(1, char(0xff))));
    frames.append(frame(0x100, QByteArray(1, char(0x00))));
    frames.append(frame(0x100, QByteArray(1, char(0xff))));

    CandidateAnalysis::Config config = oneByteConfig();
    config.candidate.isSigned = true;

    const CandidateAnalysis::Result result =
        CandidateAnalysis::analyze(frames, config);

    QCOMPARE(result.discreteState.observedValues.count(), 2);
    QCOMPARE(result.discreteState.observedValues.at(0).value, qint64(-1));
    QCOMPARE(result.discreteState.observedValues.at(1).value, qint64(0));

    QCOMPARE(result.transitions.transitions.count(), 2);
    QCOMPARE(result.transitions.transitions.at(0).fromValue, qint64(-1));
    QCOMPARE(result.transitions.transitions.at(0).toValue, qint64(0));

    QCOMPARE(result.temporalRuns.runs.count(), 3);
    QCOMPARE(result.temporalRuns.runs.at(0).value, qint64(-1));
    QCOMPARE(result.temporalRuns.runs.at(1).value, qint64(0));
    QCOMPARE(result.temporalRuns.runs.at(2).value, qint64(-1));
}

void TestCandidateAnalysis::
    filteringAndShortFramesUseAcceptedSampleSequence()
{
    QVector<CANFrame> frames;
    frames.append(frame(0x200, QByteArray(1, char(99))));
    frames.append(frame(0x100, QByteArray(1, char(1))));
    frames.append(frame(0x100, QByteArray()));
    frames.append(frame(0x100, QByteArray(1, char(2))));
    frames.append(frame(0x200, QByteArray(1, char(99))));
    frames.append(frame(0x100, QByteArray(1, char(1))));

    const CandidateAnalysis::Result result =
        CandidateAnalysis::analyze(frames, oneByteConfig(0x100));

    QCOMPARE(result.discreteState.sampleCount, 3);
    QCOMPARE(result.transitions.acceptedSampleCount, 3);
    QCOMPARE(result.temporalRuns.acceptedSampleCount, 3);

    QCOMPARE(result.discreteState.observedValues.at(0).value, qint64(1));
    QCOMPARE(result.discreteState.observedValues.at(0).firstSampleIndex, 0);
    QCOMPARE(result.discreteState.observedValues.at(0).lastSampleIndex, 2);

    QCOMPARE(result.transitions.transitions.count(), 2);
    QCOMPARE(result.transitions.transitions.at(0).firstAcceptedSampleIndex, 1);
    QCOMPARE(result.transitions.transitions.at(1).firstAcceptedSampleIndex, 2);

    QCOMPARE(result.temporalRuns.runs.count(), 3);
    QCOMPARE(result.temporalRuns.runs.at(0).firstAcceptedSampleIndex, 0);
    QCOMPARE(result.temporalRuns.runs.at(0).lastAcceptedSampleIndex, 0);
    QCOMPARE(result.temporalRuns.runs.at(1).firstAcceptedSampleIndex, 1);
    QCOMPARE(result.temporalRuns.runs.at(2).firstAcceptedSampleIndex, 2);
}

void TestCandidateAnalysis::doesNotMutateInputFrames()
{
    QVector<CANFrame> frames;
    frames.append(frame(0x100, QByteArray(1, char(1))));
    frames.append(frame(0x100, QByteArray(1, char(2))));

    const QVector<CANFrame> originalFrames = frames;

    CandidateAnalysis::analyze(frames, oneByteConfig());

    QCOMPARE(frames.count(), originalFrames.count());

    for (int index = 0; index < frames.count(); ++index)
    {
        QCOMPARE(frames.at(index).frameId(), originalFrames.at(index).frameId());
        QCOMPARE(frames.at(index).payload(), originalFrames.at(index).payload());
    }
}