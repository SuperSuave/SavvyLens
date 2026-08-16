#ifndef ANALYSISSESSION_H
#define ANALYSISSESSION_H

// SavvyLens headers
#include "analysis/frameaggregatestore.h"
#include "analysis/framecomparison.h"
#include "analysis/framehistory.h"

// C++ standard-library headers
#include <cstddef>

class CANFrame;

class AnalysisSession
{
public:
    explicit AnalysisSession(
        std::size_t maximumSnapshotsPerKey = 2);

    void ingest(const CANFrame &frame);
    void clear() noexcept;

    std::size_t aggregateCount() const noexcept;
    bool empty() const noexcept;

    const FrameAggregate *findAggregate(
        const FrameAggregateKey &key) const noexcept;

    const FrameHistorySnapshot *latestSnapshot(
        const FrameAggregateKey &key) const noexcept;

    const FrameHistorySnapshot *previousSnapshot(
        const FrameAggregateKey &key) const noexcept;

    bool compareLatest(
        const FrameAggregateKey &key,
        FrameComparison *comparison) const;

    static FrameAggregateKey makeKey(const CANFrame &frame);

private:
    FrameAggregateStore aggregateStore;
    FrameHistory frameHistory;
};

#endif // ANALYSISSESSION_H
