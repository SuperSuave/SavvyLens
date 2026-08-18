#include "frameaggregatestore.h"

// SavvyLens headers
#include "can/can_structs.h"

// C++ standard-library headers
#include <functional>

bool FrameAggregateKey::operator==(const FrameAggregateKey &other) const noexcept
{
    return bus == other.bus && frameId == other.frameId && frameType == other.frameType && hasExtendedFrameFormat == other.hasExtendedFrameFormat && isReceived == other.isReceived;
}

std::size_t FrameAggregateStore::KeyHash::operator()(
    const FrameAggregateKey &key) const noexcept
{
    std::size_t seed = 0;

    const auto combine = [&seed](std::size_t value)
    {
        seed ^= value + 0x9e3779b9u + (seed << 6u) + (seed >> 2u);
    };

    combine(std::hash<int>{}(key.bus));
    combine(std::hash<std::uint32_t>{}(key.frameId));
    combine(std::hash<int>{}(static_cast<int>(key.frameType)));
    combine(std::hash<bool>{}(key.hasExtendedFrameFormat));
    combine(std::hash<bool>{}(key.isReceived));

    return seed;
}

void FrameAggregateStore::ingest(const CANFrame &frame)
{
    const FrameAggregateKey key = makeKey(frame);

    auto result = aggregates.emplace(
        key,
        FrameAggregate{
            key,
            0,
            FrameAggregateLastFrame{
                QByteArray(),
                0,
                0}});

    FrameAggregate &aggregate = result.first->second;

    ++aggregate.occurrenceCount;
    aggregate.lastIngested.payload = frame.payload();
    aggregate.lastIngested.sourceTimedelta = frame.timedelta;
    aggregate.lastIngested.sourceFrameCount = frame.frameCount;
}

QVector<FrameAggregateKey> FrameAggregateStore::keys() const
{
    QVector<FrameAggregateKey> result;
    result.reserve(static_cast<int>(aggregates.size()));

    for (const auto &entry : aggregates)
    {
        result.append(entry.first);
    }

    return result;
}

void FrameAggregateStore::clear() noexcept
{
    aggregates.clear();
}

std::size_t FrameAggregateStore::size() const noexcept
{
    return aggregates.size();
}

bool FrameAggregateStore::empty() const noexcept
{
    return aggregates.empty();
}

const FrameAggregate *FrameAggregateStore::find(
    const FrameAggregateKey &key) const noexcept
{
    const auto iterator = aggregates.find(key);

    if (iterator == aggregates.end())
    {
        return nullptr;
    }

    return &iterator->second;
}

FrameAggregateKey FrameAggregateStore::makeKey(const CANFrame &frame)
{
    return FrameAggregateKey{
        frame.bus,
        static_cast<std::uint32_t>(frame.frameId()),
        frame.frameType(),
        frame.hasExtendedFrameFormat(),
        frame.isReceived};
}