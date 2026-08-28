#include "framecomparison.h"

FrameComparison FrameComparisonCalculator::compare(
    const FrameHistorySnapshot &previous,
    const FrameHistorySnapshot &current)
{
    FrameComparison result;

    result.previous = previous;
    result.current = current;
    result.payloadDiff = PayloadDiffCalculator::compare(
        previous.payload,
        current.payload);

    return result;
}