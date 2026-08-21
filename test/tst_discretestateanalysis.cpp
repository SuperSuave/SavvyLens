#include "tst_discretestateanalysis.h"

#include "analysis/discretestateanalysis.h"
#include "can/can_structs.h"

#include <QtTest>

namespace {
CANFrame makeFrame(quint32 frameId, const QByteArray &payload) {
  CANFrame frame;
  frame.setFrameId(frameId);
  frame.bus = 0;
  frame.isReceived = true;
  frame.setPayload(payload);
  return frame;
}

RangeSignalSpec byteSignal(quint32 canId, bool isSigned = false) {
  RangeSignalSpec spec;
  spec.canId = canId;
  spec.startBit = 0;
  spec.bitLength = 8;
  spec.isLittleEndian = true;
  spec.isSigned = isSigned;
  return spec;
}
} // namespace

void TestDiscreteStateAnalysis::emptyInput() {
  const QVector<CANFrame> frames;
  const DiscreteStateAnalysisResult result =
      DiscreteStateAnalysis::analyze(frames, byteSignal(0x100));

  QCOMPARE(result.sampleCount, 0);
  QCOMPARE(result.distinctValueCount, 0);
  QCOMPARE(result.observedValues.size(), 0);
  QCOMPARE(result.outcome, DiscreteStateAnalysisOutcome::Completed);
  QCOMPARE(result.classification, DiscreteStateClassification::NoSamples);
}

void TestDiscreteStateAnalysis::staticSignal() {
  QVector<CANFrame> frames;
  frames.append(makeFrame(0x100, QByteArray::fromHex("55")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("55")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("55")));

  const DiscreteStateAnalysisResult result =
      DiscreteStateAnalysis::analyze(frames, byteSignal(0x100));

  QCOMPARE(result.sampleCount, 3);
  QCOMPARE(result.distinctValueCount, 1);
  QCOMPARE(result.outcome, DiscreteStateAnalysisOutcome::Completed);
  QCOMPARE(result.classification, DiscreteStateClassification::Static);

  const DiscreteStateObservation observation = result.observedValues.at(0);
  QCOMPARE(observation.value, static_cast<qint64>(0x55));
  QCOMPARE(observation.occurrenceCount, 3);
  QCOMPARE(observation.firstSampleIndex, 0);
  QCOMPARE(observation.lastSampleIndex, 2);
}

void TestDiscreteStateAnalysis::toggleSignalTracksCountsAndIndexes() {
  QVector<CANFrame> frames;
  frames.append(makeFrame(0x100, QByteArray::fromHex("00")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("01")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("00")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("01")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("01")));

  const DiscreteStateAnalysisResult result =
      DiscreteStateAnalysis::analyze(frames, byteSignal(0x100));

  QCOMPARE(result.sampleCount, 5);
  QCOMPARE(result.distinctValueCount, 2);
  QCOMPARE(result.outcome, DiscreteStateAnalysisOutcome::Completed);
  QCOMPARE(result.classification,
           DiscreteStateClassification::CandidateDiscrete);

  const DiscreteStateObservation zero = result.observedValues.at(0);
  QCOMPARE(zero.value, static_cast<qint64>(0));
  QCOMPARE(zero.occurrenceCount, 2);
  QCOMPARE(zero.firstSampleIndex, 0);
  QCOMPARE(zero.lastSampleIndex, 2);

  const DiscreteStateObservation one = result.observedValues.at(1);
  QCOMPARE(one.value, static_cast<qint64>(1));
  QCOMPARE(one.occurrenceCount, 3);
  QCOMPARE(one.firstSampleIndex, 1);
  QCOMPARE(one.lastSampleIndex, 4);
}

void TestDiscreteStateAnalysis::observationsAreOrderedByValue() {
  QVector<CANFrame> frames;
  frames.append(makeFrame(0x100, QByteArray::fromHex("0A")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("02")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("07")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("02")));

  const DiscreteStateAnalysisResult result =
      DiscreteStateAnalysis::analyze(frames, byteSignal(0x100));

  QCOMPARE(result.distinctValueCount, 3);
  QCOMPARE(result.observedValues.at(0).value, static_cast<qint64>(2));
  QCOMPARE(result.observedValues.at(1).value, static_cast<qint64>(7));
  QCOMPARE(result.observedValues.at(2).value, static_cast<qint64>(10));
  QCOMPARE(result.observedValues.at(0).occurrenceCount, 2);
}

void TestDiscreteStateAnalysis::signedNegativeValues() {
  QVector<CANFrame> frames;
  frames.append(makeFrame(0x100, QByteArray::fromHex("FF")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("02")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("80")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("FF")));

  const DiscreteStateAnalysisResult result =
      DiscreteStateAnalysis::analyze(frames, byteSignal(0x100, true));

  QCOMPARE(result.sampleCount, 4);
  QCOMPARE(result.distinctValueCount, 3);
  QCOMPARE(result.observedValues.at(0).value, static_cast<qint64>(-128));
  QCOMPARE(result.observedValues.at(1).value, static_cast<qint64>(-1));
  QCOMPARE(result.observedValues.at(2).value, static_cast<qint64>(2));
  QCOMPARE(result.observedValues.at(1).occurrenceCount, 2);
  QCOMPARE(result.observedValues.at(1).firstSampleIndex, 0);
  QCOMPARE(result.observedValues.at(1).lastSampleIndex, 3);
}

void TestDiscreteStateAnalysis::filtersFramesByCanId() {
  QVector<CANFrame> frames;
  frames.append(makeFrame(0x100, QByteArray::fromHex("01")));
  frames.append(makeFrame(0x200, QByteArray::fromHex("09")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("02")));
  frames.append(makeFrame(0x200, QByteArray::fromHex("0A")));

  const DiscreteStateAnalysisResult result =
      DiscreteStateAnalysis::analyze(frames, byteSignal(0x100));

  QCOMPARE(result.sampleCount, 2);
  QCOMPARE(result.distinctValueCount, 2);
  QCOMPARE(result.observedValues.at(0).value, static_cast<qint64>(1));
  QCOMPARE(result.observedValues.at(1).value, static_cast<qint64>(2));
}

void TestDiscreteStateAnalysis::skipsUnsupportedMixedPayloadLengths() {
  QVector<CANFrame> frames;
  frames.append(makeFrame(0x100, QByteArray::fromHex("01")));
  frames.append(makeFrame(0x100, QByteArray()));
  frames.append(makeFrame(0x100, QByteArray::fromHex("02")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("03")));

  const DiscreteStateAnalysisResult result =
      DiscreteStateAnalysis::analyze(frames, byteSignal(0x100));

  QCOMPARE(result.sampleCount, 3);
  QCOMPARE(result.distinctValueCount, 3);
  QCOMPARE(result.observedValues.at(0).value, static_cast<qint64>(1));
  QCOMPARE(result.observedValues.at(0).firstSampleIndex, 0);
  QCOMPARE(result.observedValues.at(1).value, static_cast<qint64>(2));
  QCOMPARE(result.observedValues.at(1).firstSampleIndex, 1);
  QCOMPARE(result.observedValues.at(2).value, static_cast<qint64>(3));
  QCOMPARE(result.observedValues.at(2).firstSampleIndex, 2);
}

void TestDiscreteStateAnalysis::exactDistinctValueLimitCompletes() {
  QVector<CANFrame> frames;
  frames.append(makeFrame(0x100, QByteArray::fromHex("00")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("01")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("02")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("01")));

  DiscreteStateAnalysisConfig config;
  config.maxDistinctValues = 3;

  const DiscreteStateAnalysisResult result =
      DiscreteStateAnalysis::analyze(frames, byteSignal(0x100), config);

  QCOMPARE(result.sampleCount, 4);
  QCOMPARE(result.distinctValueCount, 3);
  QCOMPARE(result.observedValues.size(), 3);
  QCOMPARE(result.outcome, DiscreteStateAnalysisOutcome::Completed);
  QCOMPARE(result.classification,
           DiscreteStateClassification::CandidateDiscrete);
}

void TestDiscreteStateAnalysis::oneDistinctValueOverLimitTruncates() {
  QVector<CANFrame> frames;
  frames.append(makeFrame(0x100, QByteArray::fromHex("00")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("01")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("00")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("02")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("03")));

  DiscreteStateAnalysisConfig config;
  config.maxDistinctValues = 2;

  const DiscreteStateAnalysisResult result =
      DiscreteStateAnalysis::analyze(frames, byteSignal(0x100), config);

  QCOMPARE(result.sampleCount, 5);
  QCOMPARE(result.distinctValueCount, 2);
  QCOMPARE(result.observedValues.size(), 2);
  QCOMPARE(result.outcome,
           DiscreteStateAnalysisOutcome::TruncatedByDistinctValueLimit);
  QCOMPARE(result.classification,
           DiscreteStateClassification::HighCardinalityOrTruncated);

  QCOMPARE(result.observedValues.at(0).value, static_cast<qint64>(0));
  QCOMPARE(result.observedValues.at(0).occurrenceCount, 2);
  QCOMPARE(result.observedValues.at(0).lastSampleIndex, 2);
  QCOMPARE(result.observedValues.at(1).value, static_cast<qint64>(1));
  QCOMPARE(result.observedValues.at(1).occurrenceCount, 1);
  QCOMPARE(result.observedValues.at(1).lastSampleIndex, 1);
}

void TestDiscreteStateAnalysis::zeroDistinctValueLimitTruncatesNonEmptyInput() {
  QVector<CANFrame> frames;
  frames.append(makeFrame(0x100, QByteArray::fromHex("01")));

  DiscreteStateAnalysisConfig config;
  config.maxDistinctValues = 0;

  const DiscreteStateAnalysisResult result =
      DiscreteStateAnalysis::analyze(frames, byteSignal(0x100), config);

  QCOMPARE(result.sampleCount, 1);
  QCOMPARE(result.distinctValueCount, 0);
  QCOMPARE(result.observedValues.size(), 0);
  QCOMPARE(result.outcome,
           DiscreteStateAnalysisOutcome::TruncatedByDistinctValueLimit);
  QCOMPARE(result.classification,
           DiscreteStateClassification::HighCardinalityOrTruncated);
}

void TestDiscreteStateAnalysis::doesNotMutateInputFrames() {
  QVector<CANFrame> frames;
  frames.append(makeFrame(0x100, QByteArray::fromHex("11")));
  frames.append(makeFrame(0x100, QByteArray::fromHex("22")));
  frames.append(makeFrame(0x200, QByteArray::fromHex("33")));

  const QVector<CANFrame> originalFrames = frames;

  const DiscreteStateAnalysisResult firstResult =
      DiscreteStateAnalysis::analyze(frames, byteSignal(0x100));
  const DiscreteStateAnalysisResult secondResult =
      DiscreteStateAnalysis::analyze(frames, byteSignal(0x100));

  QCOMPARE(frames.size(), originalFrames.size());

  for (int i = 0; i < frames.size(); ++i) {
    QCOMPARE(frames.at(i).frameId(), originalFrames.at(i).frameId());
    QCOMPARE(frames.at(i).payload(), originalFrames.at(i).payload());
  }

  QCOMPARE(firstResult.sampleCount, secondResult.sampleCount);
  QCOMPARE(firstResult.distinctValueCount, secondResult.distinctValueCount);
  QCOMPARE(firstResult.outcome, secondResult.outcome);
  QCOMPARE(firstResult.classification, secondResult.classification);
}
