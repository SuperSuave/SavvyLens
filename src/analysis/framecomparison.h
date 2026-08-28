#ifndef FRAMECOMPARISON_H
#define FRAMECOMPARISON_H

// SavvyLens headers
#include "analysis/framehistory.h"
#include "analysis/payloaddiff.h"

struct FrameComparison
{
    FrameHistorySnapshot previous;
    FrameHistorySnapshot current;
    PayloadDiff payloadDiff;
};

class FrameComparisonCalculator
{
public:
    static FrameComparison compare(
        const FrameHistorySnapshot &previous,
        const FrameHistorySnapshot &current);
};

#endif // FRAMECOMPARISON_H
