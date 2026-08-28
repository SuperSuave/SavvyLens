#include "candidateanalysis.h"

CandidateAnalysis::Result
CandidateAnalysis::analyze(const QVector<CANFrame> &frames,
                           const Config &config)
{
    Result result;
    result.candidate = config.candidate;

    result.discreteState = DiscreteStateAnalysis::analyze(
        frames, config.candidate, config.discreteState);

    TransitionAnalysis::TransitionSpec transitionSpec;
    transitionSpec.signal = config.candidate;
    transitionSpec.maximumDistinctTransitions =
        config.maximumDistinctTransitions;

    result.transitions = TransitionAnalysis::analyze(frames, transitionSpec);

    TemporalAnalysis::TemporalRunSpec temporalRunSpec;
    temporalRunSpec.signal = config.candidate;
    temporalRunSpec.maximumRetainedRuns = config.maximumRetainedRuns;

    result.temporalRuns = TemporalAnalysis::analyze(frames, temporalRunSpec);

    return result;
}
