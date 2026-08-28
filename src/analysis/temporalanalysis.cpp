#include "temporalanalysis.h"

namespace {

TemporalAnalysis::Classification classificationFor(int acceptedSampleCount,
                                                   int valueChangeCount,
                                                   bool completed)
{
    if (acceptedSampleCount == 0) {
        return TemporalAnalysis::Classification::NoSamples;
    }

    if (!completed) {
        return TemporalAnalysis::Classification::HighCardinalityOrTruncated;
    }

    if (valueChangeCount == 0) {
        return TemporalAnalysis::Classification::Static;
    }

    return TemporalAnalysis::Classification::CandidateTemporalRuns;
}

} // namespace

TemporalAnalysis::Result
TemporalAnalysis::analyze(const QVector<CANFrame> &frames,
                          const TemporalRunSpec &spec)
{
    Result result;
    result.signal = spec.signal;

    const QVector<qint64> values =
        RangeStatistics::extractSignalValues(frames, spec.signal);

    result.acceptedSampleCount = values.count();

    if (values.isEmpty()) {
        result.classification = classificationFor(
            result.acceptedSampleCount, result.valueChangeCount, result.completed);
        return result;
    }

    const int limit = qMax(0, spec.maximumRetainedRuns);

    qint64 currentValue = values.at(0);
    int currentRunFirstIndex = 0;
    int currentRunSampleCount = 1;
    bool retainingCurrentRun = false;

    if (limit > 0) {
        RunEvidence firstRun;
        firstRun.value = currentValue;
        firstRun.firstAcceptedSampleIndex = currentRunFirstIndex;
        firstRun.lastAcceptedSampleIndex = currentRunFirstIndex;
        firstRun.sampleCount = currentRunSampleCount;
        result.runs.append(firstRun);
        retainingCurrentRun = true;
    } else {
        result.completed = false;
    }

    for (int acceptedSampleIndex = 1; acceptedSampleIndex < values.count();
         ++acceptedSampleIndex) {
        const qint64 value = values.at(acceptedSampleIndex);

        if (value == currentValue) {
            ++currentRunSampleCount;

            if (retainingCurrentRun) {
                RunEvidence &run = result.runs.last();
                run.lastAcceptedSampleIndex = acceptedSampleIndex;
                run.sampleCount = currentRunSampleCount;
            }

            continue;
        }

        ++result.valueChangeCount;

        currentValue = value;
        currentRunFirstIndex = acceptedSampleIndex;
        currentRunSampleCount = 1;

        if (result.runs.count() >= limit) {
            result.completed = false;
            retainingCurrentRun = false;
            continue;
        }

        RunEvidence run;
        run.value = currentValue;
        run.firstAcceptedSampleIndex = currentRunFirstIndex;
        run.lastAcceptedSampleIndex = currentRunFirstIndex;
        run.sampleCount = currentRunSampleCount;
        result.runs.append(run);
        retainingCurrentRun = true;
    }

    result.classification = classificationFor(
        result.acceptedSampleCount, result.valueChangeCount, result.completed);

    return result;
}
