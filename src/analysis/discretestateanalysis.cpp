#include "discretestateanalysis.h"

// Qt headers
#include <QMap>

namespace {
DiscreteStateClassification
classificationFor(int sampleCount, DiscreteStateAnalysisOutcome outcome,
                  int distinctValueCount) {
  if (sampleCount == 0) {
    return DiscreteStateClassification::NoSamples;
  }

  if (outcome == DiscreteStateAnalysisOutcome::TruncatedByDistinctValueLimit) {
    return DiscreteStateClassification::HighCardinalityOrTruncated;
  }

  if (distinctValueCount == 1) {
    return DiscreteStateClassification::Static;
  }

  return DiscreteStateClassification::CandidateDiscrete;
}
} // namespace

DiscreteStateAnalysisResult
DiscreteStateAnalysis::analyze(const QVector<CANFrame> &frames,
                               const RangeSignalSpec &spec,
                               const DiscreteStateAnalysisConfig &config) {
  DiscreteStateAnalysisResult result;
  result.spec = spec;
  result.maxDistinctValues = config.maxDistinctValues;

  const QVector<qint64> values =
      RangeStatistics::extractSignalValues(frames, spec);

  result.sampleCount = values.size();

  if (values.isEmpty()) {
    result.classification = classificationFor(
        result.sampleCount, result.outcome, result.distinctValueCount);
    return result;
  }

  if (config.maxDistinctValues < 1) {
    result.outcome =
        DiscreteStateAnalysisOutcome::TruncatedByDistinctValueLimit;
    result.classification = classificationFor(
        result.sampleCount, result.outcome, result.distinctValueCount);
    return result;
  }

  QMap<qint64, DiscreteStateObservation> observations;

  for (int sampleIndex = 0; sampleIndex < values.size(); ++sampleIndex) {
    const qint64 value = values.at(sampleIndex);
    QMap<qint64, DiscreteStateObservation>::iterator existing =
        observations.find(value);

    if (existing == observations.end()) {
      if (observations.size() >= config.maxDistinctValues) {
        result.outcome =
            DiscreteStateAnalysisOutcome::TruncatedByDistinctValueLimit;
        break;
      }

      DiscreteStateObservation observation;
      observation.value = value;
      observation.occurrenceCount = 1;
      observation.firstSampleIndex = sampleIndex;
      observation.lastSampleIndex = sampleIndex;
      observations.insert(value, observation);
    } else {
      existing->occurrenceCount++;
      existing->lastSampleIndex = sampleIndex;
    }
  }

  result.observedValues.reserve(observations.size());

  for (QMap<qint64, DiscreteStateObservation>::const_iterator it =
           observations.constBegin();
       it != observations.constEnd(); ++it) {
    result.observedValues.append(it.value());
  }

  result.distinctValueCount = result.observedValues.size();
  result.classification = classificationFor(result.sampleCount, result.outcome,
                                            result.distinctValueCount);

  return result;
}
