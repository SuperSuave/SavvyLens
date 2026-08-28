#ifndef FRAMEAGGREGATESTORE_H
#define FRAMEAGGREGATESTORE_H

// Qt headers
#include <QByteArray>
#include <QCanBusFrame>
#include <QtGlobal>
#include <QVector>

// C++ standard-library headers
#include <cstddef>
#include <cstdint>
#include <unordered_map>

class CANFrame;

struct FrameAggregateKey
{
    int bus;
    std::uint32_t frameId;
    QCanBusFrame::FrameType frameType;
    bool hasExtendedFrameFormat;
    bool isReceived;

    bool operator==(const FrameAggregateKey &other) const noexcept;
};

inline uint qHash(
    const FrameAggregateKey &key,
    uint seed = 0) noexcept
{
    seed = ::qHash(key.bus, seed);
    seed = ::qHash(key.frameId, seed);
    seed = ::qHash(static_cast<int>(key.frameType), seed);
    seed = ::qHash(key.hasExtendedFrameFormat, seed);
    seed = ::qHash(key.isReceived, seed);

    return seed;
}

struct FrameAggregateLastFrame
{
    QByteArray payload;
    std::uint64_t sourceTimedelta;
    std::uint32_t sourceFrameCount;
};

struct FrameAggregate
{
    FrameAggregateKey key;
    std::uint64_t occurrenceCount;
    qint64 firstObservedActivityMilliseconds;
    qint64 lastObservedActivityMilliseconds;
    FrameAggregateLastFrame lastIngested;
};

class FrameAggregateStore
{
public:
    FrameAggregateStore() = default;
    ~FrameAggregateStore() = default;

    FrameAggregateStore(const FrameAggregateStore &) = default;
    FrameAggregateStore &operator=(const FrameAggregateStore &) = default;
    FrameAggregateStore(FrameAggregateStore &&) noexcept = default;
    FrameAggregateStore &operator=(FrameAggregateStore &&) noexcept = default;

    void ingest(const CANFrame &frame, qint64 observedActivityMilliseconds);
    void clear() noexcept;

    std::size_t size() const noexcept;
    bool empty() const noexcept;

    const FrameAggregate *find(const FrameAggregateKey &key) const noexcept;

    static FrameAggregateKey makeKey(const CANFrame &frame);
    QVector<FrameAggregateKey> keys() const;

private:
    struct KeyHash
    {
        std::size_t operator()(const FrameAggregateKey &key) const noexcept;
    };

    std::unordered_map<FrameAggregateKey, FrameAggregate, KeyHash> aggregates;
};

#endif // FRAMEAGGREGATESTORE_H
