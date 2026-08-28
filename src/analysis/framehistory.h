#ifndef FRAMEHISTORY_H
#define FRAMEHISTORY_H

// SavvyLens headers
#include "analysis/frameaggregatestore.h"

// Qt headers
#include <QByteArray>

// C++ standard-library headers
#include <cstddef>
#include <cstdint>
#include <deque>
#include <unordered_map>

class CANFrame;

struct FrameHistorySnapshot
{
    QByteArray payload;
    std::uint64_t sourceTimedelta = 0;
    std::uint32_t sourceFrameCount = 0;
};

class FrameHistory
{
public:
    explicit FrameHistory(std::size_t maximumSnapshotsPerKey = 2);

    void ingest(const CANFrame &frame);
    void clear() noexcept;

    std::size_t keyCount() const noexcept;
    bool empty() const noexcept;

    std::size_t snapshotCount(const FrameAggregateKey &key) const noexcept;

    const FrameHistorySnapshot *latest(
        const FrameAggregateKey &key) const noexcept;

    const FrameHistorySnapshot *previous(
        const FrameAggregateKey &key) const noexcept;

    static FrameAggregateKey makeKey(const CANFrame &frame);

private:
    struct KeyHash
    {
        std::size_t operator()(const FrameAggregateKey &key) const noexcept;
    };

    using SnapshotList = std::deque<FrameHistorySnapshot>;

    std::size_t maximumSnapshotsPerKey;

    std::unordered_map<
        FrameAggregateKey,
        SnapshotList,
        KeyHash>
        histories;
};

#endif // FRAMEHISTORY_H
