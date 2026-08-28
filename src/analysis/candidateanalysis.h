#ifndef CANDIDATEANALYSIS_H
#define CANDIDATEANALYSIS_H

// SavvyLens headers
#include "discretestateanalysis.h"
#include "temporalanalysis.h"
#include "transitionanalysis.h"

class CANFrame;

namespace CandidateAnalysis {

    struct Config {
    // The one authoritative signal specification used by every contained
    // analyzer.
    RangeSignalSpec candidate;

    // Maximum distinct values retained by DiscreteStateAnalysis.
    DiscreteStateAnalysisConfig discreteState;

    // Maximum distinct directed transitions retained by TransitionAnalysis.
    int maximumDistinctTransitions = 64;

    // Maximum ordered consecutive runs retained by TemporalAnalysis.
    int maximumRetainedRuns = 64;
};

struct Result {
    // The selected candidate specification used for this complete result.
    RangeSignalSpec candidate;

    // Independent UI-neutral evidence. Each embedded analyzer retains its own
    // accepted-sample, truncation, and conservative classification contract.
    DiscreteStateAnalysisResult discreteState;
    TransitionAnalysis::Result transitions;
    TemporalAnalysis::Result temporalRuns;
};

// Runs independent UI-neutral analyses for one selected RangeSignalSpec.
//
// The same candidate specification is applied consistently to each contained
// analyzer. Each analyzer retains its existing extraction contract through
// RangeStatistics::extractSignalValues().
//
// This composition layer adds no semantic scoring or vehicle interpretation,
// does not mutate input frames, and does not mutate shared analysis state.
Result analyze(const QVector<CANFrame> &frames, const Config &config);

} // namespace CandidateAnalysis

#endif // CANDIDATEANALYSIS_H
