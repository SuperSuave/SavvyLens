#include "framehistory.h"

// SavvyLens headers
#include "can/can_structs.h"


// C++ standard-library headers
#include <functional>


FrameHistory::FrameHistory(std::size_t maximumSnapshotsPerKey)
    : maximumSnapshotsPerKey(maximumSnapshotsPerKey)
{
}


std::size_t FrameHistory::KeyHash::operator()(
    const FrameAggregateKey &key) const noexcept
{
    std::size_t seed = 0;

    const auto combine = [&seed](std::size_t value) {
        seed ^= value + 0x9e3779b9u + (seed << 6u) + (seed >> 2u);
    };

    combine(std::hash<int>{}(key.bus));
    combine(std::hash<std::uint32_t>{}(key.frameId));
    combine(std::hash<int>{}(static_cast<int>(key.frameType)));
    combine(std::hash<bool>{}(key.hasExtendedFrameFormat));
    combine(std::hash<bool>{}(key.isReceived));

    return seed;
}


void FrameHistory::ingest(const CANFrame &frame)
{
    if (maximumSnapshotsPerKey == 0)
    {
        return;
    }

    const FrameAggregateKey key = makeKey(frame);

    SnapshotList &history = histories[key];

    FrameHistorySnapshot snapshot;
    snapshot.payload = frame.payload();
    snapshot.sourceTimedelta = frame.timedelta;
    snapshot.sourceFrameCount = frame.frameCount;

    history.push_back(snapshot);

    while (history.size() > maximumSnapshotsPerKey)
    {
        history.pop_front();
    }
}


void FrameHistory::clear() noexcept
{
    histories.clear();
}


std::size_t FrameHistory::keyCount() const noexcept
{
    return histories.size();
}


bool FrameHistory::empty() const noexcept
{
    return histories.empty();
}


std::size_t FrameHistory::snapshotCount(
    const FrameAggregateKey &key) const noexcept
{
    const auto iterator = histories.find(key);

    if (iterator == histories.end())
    {
        return 0;
    }

    return iterator->second.size();
}


const FrameHistorySnapshot *FrameHistory::latest(
    const FrameAggregateKey &key) const noexcept
{
    const auto iterator = histories.find(key);

    if (iterator == histories.end() || iterator->second.empty())
    {
        return nullptr;
    }

    return &iterator->second.back();
}


const FrameHistorySnapshot *FrameHistory::previous(
    const FrameAggregateKey &key) const noexcept
{
    const auto iterator = histories.find(key);

    if (iterator == histories.end() || iterator->second.size() < 2)
    {
        return nullptr;
    }

    return &iterator->second[iterator->second.size() - 2];
}


FrameAggregateKey FrameHistory::makeKey(const CANFrame &frame)
{
    return FrameAggregateKey{
        frame.bus,
        static_cast<std::uint32_t>(frame.frameId()),
        frame.frameType(),
        frame.hasExtendedFrameFormat(),
        frame.isReceived
    };
}