#ifndef TEMPORALANALYSIS_H
#define TEMPORALANALYSIS_H

#include "rangestatistics.h"

// Qt headers
#include <QVector>

class CANFrame;

namespace TemporalAnalysis {

enum class Classification {
    NoSamples,
    Static,
    CandidateTemporalRuns,
    HighCardinalityOrTruncated
};

struct TemporalRunSpec {
    RangeSignalSpec signal;

    // Maximum number of consecutive runs retained in encounter order.
    // A value less than one retains no runs and truncates non-empty input.
    int maximumRetainedRuns = 64;
};

struct RunEvidence {
    qint64 value = 0;
    int firstAcceptedSampleIndex = -1;
    int lastAcceptedSampleIndex = -1;
    int sampleCount = 0;
};

struct Result {
    RangeSignalSpec signal;
    int acceptedSampleCount = 0;
    int valueChangeCount = 0;
    QVector<RunEvidence> runs;
    bool completed = true;
    Classification classification = Classification::NoSamples;
};

// Extracts values through RangeStatistics::extractSignalValues() and records
// consecutive equal-value runs from that accepted extracted-value sequence.
//
// Accepted sample indexes refer to positions in the extracted sequence. Frames
// rejected by RangeStatistics due to CAN-ID mismatch or insufficient payload
// support do not consume an index.
//
// CandidateTemporalRuns only identifies observed value changes in a completed,
// bounded capture analysis. It is not proof of a vehicle state, dwell time,
// event, mode, or other vehicle semantic.
Result analyze(const QVector<CANFrame> &frames, const TemporalRunSpec &spec);

} // namespace TemporalAnalysis

#endif // TEMPORALANALYSIS_H
