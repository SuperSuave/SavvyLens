#include "transitionanalysis.h"

#include <QMap>
#include <QPair>

namespace {
typedef QPair<qint64, qint64> TransitionKey;

TransitionAnalysis::Classification classificationFor(int acceptedSampleCount,
                                                     int valueChangeCount,
                                                     bool completed) {
  if (acceptedSampleCount == 0) {
    return TransitionAnalysis::Classification::NoSamples;
  }

  if (!completed) {
    return TransitionAnalysis::Classification::HighCardinalityOrTruncated;
  }

  if (valueChangeCount == 0) {
    return TransitionAnalysis::Classification::Static;
  }

  return TransitionAnalysis::Classification::CandidateTransitions;
}
} // namespace

TransitionAnalysis::Result
TransitionAnalysis::analyze(const QVector<CANFrame> &frames,
                            const TransitionSpec &spec) {
  Result result;
  result.signal = spec.signal;

  const QVector<qint64> values =
      RangeStatistics::extractSignalValues(frames, spec.signal);

  result.acceptedSampleCount = values.count();

  QMap<TransitionKey, TransitionEvidence> evidenceByTransition;
  const int limit = qMax(0, spec.maximumDistinctTransitions);

  for (int acceptedSampleIndex = 1; acceptedSampleIndex < values.count();
       ++acceptedSampleIndex) {
    const qint64 fromValue = values.at(acceptedSampleIndex - 1);
    const qint64 toValue = values.at(acceptedSampleIndex);

    if (fromValue == toValue) {
      continue;
    }

    ++result.valueChangeCount;

    const TransitionKey key(fromValue, toValue);
    QMap<TransitionKey, TransitionEvidence>::iterator existing =
        evidenceByTransition.find(key);

    if (existing != evidenceByTransition.end()) {
      ++existing->occurrenceCount;
      existing->lastAcceptedSampleIndex = acceptedSampleIndex;
      continue;
    }

    if (evidenceByTransition.count() >= limit) {
      result.completed = false;
      continue;
    }

    TransitionEvidence evidence;
    evidence.fromValue = fromValue;
    evidence.toValue = toValue;
    evidence.occurrenceCount = 1;
    evidence.firstAcceptedSampleIndex = acceptedSampleIndex;
    evidence.lastAcceptedSampleIndex = acceptedSampleIndex;

    evidenceByTransition.insert(key, evidence);
  }

  result.transitions.reserve(evidenceByTransition.count());

  for (QMap<TransitionKey, TransitionEvidence>::const_iterator it =
           evidenceByTransition.constBegin();
       it != evidenceByTransition.constEnd(); ++it) {
    result.transitions.append(it.value());
  }

  result.classification = classificationFor(
      result.acceptedSampleCount, result.valueChangeCount, result.completed);

  return result;
}
