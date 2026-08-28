#ifndef DISCRETESTATEANALYSIS_H
#define DISCRETESTATEANALYSIS_H

// SavvyLens headers
#include "analysis/rangestatistics.h"

// Qt headers
#include <QVector>

struct DiscreteStateObservation {
  qint64 value = 0;
  int occurrenceCount = 0;
  int firstSampleIndex = -1;
  int lastSampleIndex = -1;
};

enum class DiscreteStateAnalysisOutcome {
  Completed,
  TruncatedByDistinctValueLimit
};

enum class DiscreteStateClassification {
  NoSamples,
  Static,
  CandidateDiscrete,
  HighCardinalityOrTruncated
};

struct DiscreteStateAnalysisConfig {
  // Maximum number of distinct values retained in the result.
  // A value less than one retains no values and truncates any non-empty input.
  int maxDistinctValues = 32;
};

struct DiscreteStateAnalysisResult {
  RangeSignalSpec spec;
  int sampleCount = 0;

  // Count of values retained in observedValues. If outcome is truncated,
  // the true distinct count is greater than distinctValueCount.
  int distinctValueCount = 0;
  int maxDistinctValues = 0;

  DiscreteStateAnalysisOutcome outcome =
      DiscreteStateAnalysisOutcome::Completed;
  DiscreteStateClassification classification =
      DiscreteStateClassification::NoSamples;

  // Ordered by ascending numeric value, not encounter order.
  QVector<DiscreteStateObservation> observedValues;
};

class DiscreteStateAnalysis {
public:
  //
  // Extracts values through RangeStatistics::extractSignalValues() and
  // summarizes the accepted sample sequence.
  //
  // Sample indices refer to positions in that accepted extracted-value
  // sequence. Frames rejected by RangeStatistics due to CAN-ID mismatch
  // or insufficient payload support do not consume an index.
  //
  // CandidateDiscrete means only that the complete observed sample set had
  // bounded cardinality. It is not proof that a signal is semantically a
  // vehicle state, switch, enum, or transition-driven state machine.
  //
  static DiscreteStateAnalysisResult
  analyze(const QVector<CANFrame> &frames, const RangeSignalSpec &spec,
          const DiscreteStateAnalysisConfig &config =
              DiscreteStateAnalysisConfig());
};

#endif // DISCRETESTATEANALYSIS_H
