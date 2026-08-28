#include "tst_temporalanalysis.h"

// SavvyLens headers
#include "analysis/temporalanalysis.h"
#include "can/can_structs.h"

// Qt headers
#include <QtTest>

namespace {

CANFrame frame(quint32 id, const QByteArray &payload) {
  CANFrame result;
  result.setFrameId(id);
  result.bus = 0;
  result.isReceived = true;
  result.setPayload(payload);
  return result;
}

TemporalAnalysis::TemporalRunSpec oneByteSpec(quint32 canId = 0x100,
                                              int maximumRetainedRuns = 64) {
  TemporalAnalysis::TemporalRunSpec spec;
  spec.signal.canId = canId;
  spec.signal.startBit = 0;
  spec.signal.bitLength = 8;
  spec.signal.isLittleEndian = true;
  spec.signal.isSigned = false;
  spec.maximumRetainedRuns = maximumRetainedRuns;
  return spec;
}

} // namespace

void TestTemporalAnalysis::emptyInput() {
  const TemporalAnalysis::Result result =
      TemporalAnalysis::analyze(QVector<CANFrame>(), oneByteSpec());

  QCOMPARE(result.acceptedSampleCount, 0);
  QCOMPARE(result.valueChangeCount, 0);
  QCOMPARE(result.runs.count(), 0);
  QVERIFY(result.completed);
  QCOMPARE(result.classification, TemporalAnalysis::Classification::NoSamples);
}

void TestTemporalAnalysis::singleAcceptedSampleIsStatic() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(7))));

  const TemporalAnalysis::Result result =
      TemporalAnalysis::analyze(frames, oneByteSpec());

  QCOMPARE(result.acceptedSampleCount, 1);
  QCOMPARE(result.valueChangeCount, 0);
  QCOMPARE(result.runs.count(), 1);
  QVERIFY(result.completed);
  QCOMPARE(result.classification, TemporalAnalysis::Classification::Static);

  QCOMPARE(result.runs.at(0).value, qint64(7));
  QCOMPARE(result.runs.at(0).firstAcceptedSampleIndex, 0);
  QCOMPARE(result.runs.at(0).lastAcceptedSampleIndex, 0);
  QCOMPARE(result.runs.at(0).sampleCount, 1);
}

void TestTemporalAnalysis::staticSequenceProducesOneRun() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(7))));
  frames.append(frame(0x100, QByteArray(1, char(7))));
  frames.append(frame(0x100, QByteArray(1, char(7))));

  const TemporalAnalysis::Result result =
      TemporalAnalysis::analyze(frames, oneByteSpec());

  QCOMPARE(result.acceptedSampleCount, 3);
  QCOMPARE(result.valueChangeCount, 0);
  QCOMPARE(result.runs.count(), 1);
  QVERIFY(result.completed);
  QCOMPARE(result.classification, TemporalAnalysis::Classification::Static);

  QCOMPARE(result.runs.at(0).value, qint64(7));
  QCOMPARE(result.runs.at(0).firstAcceptedSampleIndex, 0);
  QCOMPARE(result.runs.at(0).lastAcceptedSampleIndex, 2);
  QCOMPARE(result.runs.at(0).sampleCount, 3);
}

void TestTemporalAnalysis::multipleSelfRunsProduceOrderedRuns() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray(1, char(2))));
  frames.append(frame(0x100, QByteArray(1, char(2))));
  frames.append(frame(0x100, QByteArray(1, char(2))));
  frames.append(frame(0x100, QByteArray(1, char(1))));

  const TemporalAnalysis::Result result =
      TemporalAnalysis::analyze(frames, oneByteSpec());

  QCOMPARE(result.acceptedSampleCount, 6);
  QCOMPARE(result.valueChangeCount, 2);
  QCOMPARE(result.runs.count(), 3);
  QVERIFY(result.completed);
  QCOMPARE(result.classification,
           TemporalAnalysis::Classification::CandidateTemporalRuns);

  QCOMPARE(result.runs.at(0).value, qint64(1));
  QCOMPARE(result.runs.at(0).firstAcceptedSampleIndex, 0);
  QCOMPARE(result.runs.at(0).lastAcceptedSampleIndex, 1);
  QCOMPARE(result.runs.at(0).sampleCount, 2);

  QCOMPARE(result.runs.at(1).value, qint64(2));
  QCOMPARE(result.runs.at(1).firstAcceptedSampleIndex, 2);
  QCOMPARE(result.runs.at(1).lastAcceptedSampleIndex, 4);
  QCOMPARE(result.runs.at(1).sampleCount, 3);

  QCOMPARE(result.runs.at(2).value, qint64(1));
  QCOMPARE(result.runs.at(2).firstAcceptedSampleIndex, 5);
  QCOMPARE(result.runs.at(2).lastAcceptedSampleIndex, 5);
  QCOMPARE(result.runs.at(2).sampleCount, 1);
}

void TestTemporalAnalysis::toggleSequenceProducesOrderedRuns() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(0))));
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray(1, char(0))));
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray(1, char(0))));

  const TemporalAnalysis::Result result =
      TemporalAnalysis::analyze(frames, oneByteSpec());

  QCOMPARE(result.acceptedSampleCount, 5);
  QCOMPARE(result.valueChangeCount, 4);
  QCOMPARE(result.runs.count(), 5);
  QVERIFY(result.completed);
  QCOMPARE(result.classification,
           TemporalAnalysis::Classification::CandidateTemporalRuns);

  for (int index = 0; index < result.runs.count(); ++index) {
    QCOMPARE(result.runs.at(index).value, qint64(index % 2));
    QCOMPARE(result.runs.at(index).firstAcceptedSampleIndex, index);
    QCOMPARE(result.runs.at(index).lastAcceptedSampleIndex, index);
    QCOMPARE(result.runs.at(index).sampleCount, 1);
  }
}

void TestTemporalAnalysis::signedValuesAreRetained() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(0xff))));
  frames.append(frame(0x100, QByteArray(1, char(0xff))));
  frames.append(frame(0x100, QByteArray(1, char(0x00))));
  frames.append(frame(0x100, QByteArray(1, char(0xff))));

  TemporalAnalysis::TemporalRunSpec spec = oneByteSpec();
  spec.signal.isSigned = true;

  const TemporalAnalysis::Result result =
      TemporalAnalysis::analyze(frames, spec);

  QCOMPARE(result.runs.count(), 3);

  QCOMPARE(result.runs.at(0).value, qint64(-1));
  QCOMPARE(result.runs.at(0).firstAcceptedSampleIndex, 0);
  QCOMPARE(result.runs.at(0).lastAcceptedSampleIndex, 1);
  QCOMPARE(result.runs.at(0).sampleCount, 2);

  QCOMPARE(result.runs.at(1).value, qint64(0));
  QCOMPARE(result.runs.at(1).firstAcceptedSampleIndex, 2);
  QCOMPARE(result.runs.at(1).lastAcceptedSampleIndex, 2);
  QCOMPARE(result.runs.at(1).sampleCount, 1);

  QCOMPARE(result.runs.at(2).value, qint64(-1));
  QCOMPARE(result.runs.at(2).firstAcceptedSampleIndex, 3);
  QCOMPARE(result.runs.at(2).lastAcceptedSampleIndex, 3);
  QCOMPARE(result.runs.at(2).sampleCount, 1);
}

void TestTemporalAnalysis::
    filteringAndShortFramesDoNotConsumeAcceptedIndexes() {
  QVector<CANFrame> frames;
  frames.append(frame(0x200, QByteArray(1, char(99))));
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray()));
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x200, QByteArray(1, char(99))));
  frames.append(frame(0x100, QByteArray(1, char(2))));
  frames.append(frame(0x100, QByteArray()));

  const TemporalAnalysis::Result result =
      TemporalAnalysis::analyze(frames, oneByteSpec(0x100));

  QCOMPARE(result.acceptedSampleCount, 3);
  QCOMPARE(result.valueChangeCount, 1);
  QCOMPARE(result.runs.count(), 2);
  QVERIFY(result.completed);

  QCOMPARE(result.runs.at(0).value, qint64(1));
  QCOMPARE(result.runs.at(0).firstAcceptedSampleIndex, 0);
  QCOMPARE(result.runs.at(0).lastAcceptedSampleIndex, 1);
  QCOMPARE(result.runs.at(0).sampleCount, 2);

  QCOMPARE(result.runs.at(1).value, qint64(2));
  QCOMPARE(result.runs.at(1).firstAcceptedSampleIndex, 2);
  QCOMPARE(result.runs.at(1).lastAcceptedSampleIndex, 2);
  QCOMPARE(result.runs.at(1).sampleCount, 1);
}

void TestTemporalAnalysis::exactRunLimitCompletes() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(0))));
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray(1, char(2))));

  const TemporalAnalysis::Result result =
      TemporalAnalysis::analyze(frames, oneByteSpec(0x100, 3));

  QCOMPARE(result.acceptedSampleCount, 3);
  QCOMPARE(result.valueChangeCount, 2);
  QCOMPARE(result.runs.count(), 3);
  QVERIFY(result.completed);
  QCOMPARE(result.classification,
           TemporalAnalysis::Classification::CandidateTemporalRuns);
}

void TestTemporalAnalysis::limitOverflowIsBoundedAndTruncated() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(0))));
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray(1, char(2))));
  frames.append(frame(0x100, QByteArray(1, char(3))));

  const TemporalAnalysis::Result result =
      TemporalAnalysis::analyze(frames, oneByteSpec(0x100, 3));

  QCOMPARE(result.acceptedSampleCount, 4);
  QCOMPARE(result.valueChangeCount, 3);
  QCOMPARE(result.runs.count(), 3);
  QVERIFY(!result.completed);
  QCOMPARE(result.classification,
           TemporalAnalysis::Classification::HighCardinalityOrTruncated);

  QCOMPARE(result.runs.at(0).value, qint64(0));
  QCOMPARE(result.runs.at(1).value, qint64(1));
  QCOMPARE(result.runs.at(2).value, qint64(2));
}

void TestTemporalAnalysis::zeroLimitIsBoundedAndTruncatedForNonEmptyInput() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(0))));
  frames.append(frame(0x100, QByteArray(1, char(1))));

  const TemporalAnalysis::Result result =
      TemporalAnalysis::analyze(frames, oneByteSpec(0x100, 0));

  QCOMPARE(result.acceptedSampleCount, 2);
  QCOMPARE(result.valueChangeCount, 1);
  QCOMPARE(result.runs.count(), 0);
  QVERIFY(!result.completed);
  QCOMPARE(result.classification,
           TemporalAnalysis::Classification::HighCardinalityOrTruncated);
}

void TestTemporalAnalysis::
    negativeLimitIsBoundedAndTruncatedForNonEmptyInput() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(7))));

  const TemporalAnalysis::Result result =
      TemporalAnalysis::analyze(frames, oneByteSpec(0x100, -1));

  QCOMPARE(result.acceptedSampleCount, 1);
  QCOMPARE(result.valueChangeCount, 0);
  QCOMPARE(result.runs.count(), 0);
  QVERIFY(!result.completed);
  QCOMPARE(result.classification,
           TemporalAnalysis::Classification::HighCardinalityOrTruncated);
}

void TestTemporalAnalysis::retainedRunContinuesUntilItsBoundary() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray(1, char(2))));
  frames.append(frame(0x100, QByteArray(1, char(2))));
  frames.append(frame(0x100, QByteArray(1, char(3))));
  frames.append(frame(0x100, QByteArray(1, char(3))));

  const TemporalAnalysis::Result result =
      TemporalAnalysis::analyze(frames, oneByteSpec(0x100, 2));

  QCOMPARE(result.acceptedSampleCount, 6);
  QCOMPARE(result.valueChangeCount, 2);
  QCOMPARE(result.runs.count(), 2);
  QVERIFY(!result.completed);

  QCOMPARE(result.runs.at(0).value, qint64(1));
  QCOMPARE(result.runs.at(0).firstAcceptedSampleIndex, 0);
  QCOMPARE(result.runs.at(0).lastAcceptedSampleIndex, 1);
  QCOMPARE(result.runs.at(0).sampleCount, 2);

  QCOMPARE(result.runs.at(1).value, qint64(2));
  QCOMPARE(result.runs.at(1).firstAcceptedSampleIndex, 2);
  QCOMPARE(result.runs.at(1).lastAcceptedSampleIndex, 3);
  QCOMPARE(result.runs.at(1).sampleCount, 2);
}

void TestTemporalAnalysis::doesNotMutateInputFrames() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray(1, char(2))));

  const QVector<CANFrame> originalFrames = frames;

  TemporalAnalysis::analyze(frames, oneByteSpec());

  QCOMPARE(frames.count(), originalFrames.count());

  for (int index = 0; index < frames.count(); ++index) {
    QCOMPARE(frames.at(index).frameId(), originalFrames.at(index).frameId());
    QCOMPARE(frames.at(index).payload(), originalFrames.at(index).payload());
  }
}
