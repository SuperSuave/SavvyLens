#ifndef TRANSITIONANALYSIS_H
#define TRANSITIONANALYSIS_H

#include <QVector>

#include "rangestatistics.h"

class CANFrame;

namespace TransitionAnalysis {
enum class Classification {
  NoSamples,
  Static,
  CandidateTransitions,
  HighCardinalityOrTruncated
};

struct TransitionSpec {
  RangeSignalSpec signal;
  int maximumDistinctTransitions = 64;
};

struct TransitionEvidence {
  qint64 fromValue = 0;
  qint64 toValue = 0;
  int occurrenceCount = 0;
  int firstAcceptedSampleIndex = -1;
  int lastAcceptedSampleIndex = -1;
};

struct Result {
  RangeSignalSpec signal;
  int acceptedSampleCount = 0;
  int valueChangeCount = 0;
  QVector<TransitionEvidence> transitions;
  bool completed = true;
  Classification classification = Classification::NoSamples;
};

Result analyze(const QVector<CANFrame> &frames, const TransitionSpec &spec);
} // namespace TransitionAnalysis

#endif // TRANSITIONANALYSIS_H
