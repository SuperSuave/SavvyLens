#include "tst_transitionanalysis.h"

// SavvyLens headers
#include "src/analysis/transitionanalysis.h"
#include "src/can/can_structs.h"

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

TransitionAnalysis::TransitionSpec
oneByteSpec(quint32 canId = 0x100, int maximumDistinctTransitions = 64) {
  TransitionAnalysis::TransitionSpec spec;
  spec.signal.canId = canId;
  spec.signal.startBit = 0;
  spec.signal.bitLength = 8;
  spec.signal.isLittleEndian = true;
  spec.signal.isSigned = false;
  spec.maximumDistinctTransitions = maximumDistinctTransitions;
  return spec;
}
} // namespace

void TestTransitionAnalysis::emptyInput() {
  const TransitionAnalysis::Result result =
      TransitionAnalysis::analyze(QVector<CANFrame>(), oneByteSpec());

  QCOMPARE(result.acceptedSampleCount, 0);
  QCOMPARE(result.valueChangeCount, 0);
  QCOMPARE(result.transitions.count(), 0);
  QVERIFY(result.completed);
  QCOMPARE(result.classification,
           TransitionAnalysis::Classification::NoSamples);
}

void TestTransitionAnalysis::singleAcceptedSampleIsStatic() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(7))));

  const TransitionAnalysis::Result result =
      TransitionAnalysis::analyze(frames, oneByteSpec());

  QCOMPARE(result.acceptedSampleCount, 1);
  QCOMPARE(result.valueChangeCount, 0);
  QCOMPARE(result.transitions.count(), 0);
  QVERIFY(result.completed);
  QCOMPARE(result.classification, TransitionAnalysis::Classification::Static);
}

void TestTransitionAnalysis::staticSequenceHasNoTransitions() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(7))));
  frames.append(frame(0x100, QByteArray(1, char(7))));
  frames.append(frame(0x100, QByteArray(1, char(7))));

  const TransitionAnalysis::Result result =
      TransitionAnalysis::analyze(frames, oneByteSpec());

  QCOMPARE(result.acceptedSampleCount, 3);
  QCOMPARE(result.valueChangeCount, 0);
  QCOMPARE(result.transitions.count(), 0);
  QVERIFY(result.completed);
  QCOMPARE(result.classification, TransitionAnalysis::Classification::Static);
}

void TestTransitionAnalysis::selfRunsDoNotCreateTransitions() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray(1, char(2))));
  frames.append(frame(0x100, QByteArray(1, char(2))));
  frames.append(frame(0x100, QByteArray(1, char(1))));

  const TransitionAnalysis::Result result =
      TransitionAnalysis::analyze(frames, oneByteSpec());

  QCOMPARE(result.acceptedSampleCount, 5);
  QCOMPARE(result.valueChangeCount, 2);
  QCOMPARE(result.transitions.count(), 2);

  QCOMPARE(result.transitions.at(0).fromValue, qint64(1));
  QCOMPARE(result.transitions.at(0).toValue, qint64(2));
  QCOMPARE(result.transitions.at(0).occurrenceCount, 1);
  QCOMPARE(result.transitions.at(0).firstAcceptedSampleIndex, 2);
  QCOMPARE(result.transitions.at(0).lastAcceptedSampleIndex, 2);

  QCOMPARE(result.transitions.at(1).fromValue, qint64(2));
  QCOMPARE(result.transitions.at(1).toValue, qint64(1));
  QCOMPARE(result.transitions.at(1).occurrenceCount, 1);
  QCOMPARE(result.transitions.at(1).firstAcceptedSampleIndex, 4);
  QCOMPARE(result.transitions.at(1).lastAcceptedSampleIndex, 4);
}

void TestTransitionAnalysis::toggleSequenceAggregatesDirectedTransitions() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(0))));
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray(1, char(0))));
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray(1, char(0))));

  const TransitionAnalysis::Result result =
      TransitionAnalysis::analyze(frames, oneByteSpec());

  QCOMPARE(result.acceptedSampleCount, 5);
  QCOMPARE(result.valueChangeCount, 4);
  QCOMPARE(result.transitions.count(), 2);
  QVERIFY(result.completed);

  QCOMPARE(result.transitions.at(0).fromValue, qint64(0));
  QCOMPARE(result.transitions.at(0).toValue, qint64(1));
  QCOMPARE(result.transitions.at(0).occurrenceCount, 2);
  QCOMPARE(result.transitions.at(0).firstAcceptedSampleIndex, 1);
  QCOMPARE(result.transitions.at(0).lastAcceptedSampleIndex, 3);

  QCOMPARE(result.transitions.at(1).fromValue, qint64(1));
  QCOMPARE(result.transitions.at(1).toValue, qint64(0));
  QCOMPARE(result.transitions.at(1).occurrenceCount, 2);
  QCOMPARE(result.transitions.at(1).firstAcceptedSampleIndex, 2);
  QCOMPARE(result.transitions.at(1).lastAcceptedSampleIndex, 4);

  QCOMPARE(result.classification,
           TransitionAnalysis::Classification::CandidateTransitions);
}

void TestTransitionAnalysis::transitionsUseDeterministicOrdering() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(2))));
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray(1, char(3))));
  frames.append(frame(0x100, QByteArray(1, char(0))));

  const TransitionAnalysis::Result result =
      TransitionAnalysis::analyze(frames, oneByteSpec());

  QCOMPARE(result.transitions.count(), 3);

  QCOMPARE(result.transitions.at(0).fromValue, qint64(1));
  QCOMPARE(result.transitions.at(0).toValue, qint64(3));
  QCOMPARE(result.transitions.at(1).fromValue, qint64(2));
  QCOMPARE(result.transitions.at(1).toValue, qint64(1));
  QCOMPARE(result.transitions.at(2).fromValue, qint64(3));
  QCOMPARE(result.transitions.at(2).toValue, qint64(0));
}

void TestTransitionAnalysis::signedValuesAreRetained() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(0xff))));
  frames.append(frame(0x100, QByteArray(1, char(0x00))));
  frames.append(frame(0x100, QByteArray(1, char(0xff))));

  TransitionAnalysis::TransitionSpec spec = oneByteSpec();
  spec.signal.isSigned = true;

  const TransitionAnalysis::Result result =
      TransitionAnalysis::analyze(frames, spec);

  QCOMPARE(result.transitions.count(), 2);

  QCOMPARE(result.transitions.at(0).fromValue, qint64(-1));
  QCOMPARE(result.transitions.at(0).toValue, qint64(0));
  QCOMPARE(result.transitions.at(1).fromValue, qint64(0));
  QCOMPARE(result.transitions.at(1).toValue, qint64(-1));
}

void TestTransitionAnalysis::
    filteringAndShortFramesDoNotConsumeAcceptedIndexes() {
  QVector<CANFrame> frames;
  frames.append(frame(0x200, QByteArray(1, char(99))));
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray()));
  frames.append(frame(0x100, QByteArray(1, char(2))));
  frames.append(frame(0x200, QByteArray(1, char(99))));
  frames.append(frame(0x100, QByteArray(1, char(1))));

  const TransitionAnalysis::Result result =
      TransitionAnalysis::analyze(frames, oneByteSpec(0x100));

  QCOMPARE(result.acceptedSampleCount, 3);
  QCOMPARE(result.valueChangeCount, 2);
  QCOMPARE(result.transitions.count(), 2);

  QCOMPARE(result.transitions.at(0).fromValue, qint64(1));
  QCOMPARE(result.transitions.at(0).toValue, qint64(2));
  QCOMPARE(result.transitions.at(0).firstAcceptedSampleIndex, 1);

  QCOMPARE(result.transitions.at(1).fromValue, qint64(2));
  QCOMPARE(result.transitions.at(1).toValue, qint64(1));
  QCOMPARE(result.transitions.at(1).firstAcceptedSampleIndex, 2);
}

void TestTransitionAnalysis::exactTransitionLimitCompletes() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(0))));
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray(1, char(2))));

  const TransitionAnalysis::Result result =
      TransitionAnalysis::analyze(frames, oneByteSpec(0x100, 2));

  QCOMPARE(result.valueChangeCount, 2);
  QCOMPARE(result.transitions.count(), 2);
  QVERIFY(result.completed);
  QCOMPARE(result.classification,
           TransitionAnalysis::Classification::CandidateTransitions);
}

void TestTransitionAnalysis::limitOverflowIsBoundedAndTruncated() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(0))));
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray(1, char(2))));
  frames.append(frame(0x100, QByteArray(1, char(3))));

  const TransitionAnalysis::Result result =
      TransitionAnalysis::analyze(frames, oneByteSpec(0x100, 2));

  QCOMPARE(result.valueChangeCount, 3);
  QCOMPARE(result.transitions.count(), 2);
  QVERIFY(!result.completed);
  QCOMPARE(result.classification,
           TransitionAnalysis::Classification::HighCardinalityOrTruncated);
}

void TestTransitionAnalysis::zeroLimitIsBoundedAndTruncatedForChanges() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(0))));
  frames.append(frame(0x100, QByteArray(1, char(1))));

  const TransitionAnalysis::Result result =
      TransitionAnalysis::analyze(frames, oneByteSpec(0x100, 0));

  QCOMPARE(result.acceptedSampleCount, 2);
  QCOMPARE(result.valueChangeCount, 1);
  QCOMPARE(result.transitions.count(), 0);
  QVERIFY(!result.completed);
  QCOMPARE(result.classification,
           TransitionAnalysis::Classification::HighCardinalityOrTruncated);
}

void TestTransitionAnalysis::doesNotMutateInputFrames() {
  QVector<CANFrame> frames;
  frames.append(frame(0x100, QByteArray(1, char(1))));
  frames.append(frame(0x100, QByteArray(1, char(2))));

  const QVector<CANFrame> originalFrames = frames;

  TransitionAnalysis::analyze(frames, oneByteSpec(0x100));

  QCOMPARE(frames.count(), originalFrames.count());

  for (int index = 0; index < frames.count(); ++index) {
    QCOMPARE(frames.at(index).frameId(), originalFrames.at(index).frameId());
    QCOMPARE(frames.at(index).payload(), originalFrames.at(index).payload());
  }
}
