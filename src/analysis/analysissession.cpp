// SavvyLens headers
#include "analysissession.h"
#include "can/can_structs.h"

AnalysisSession::AnalysisSession(
    std::size_t maximumSnapshotsPerKey,
    std::size_t maximumStateExplorerFrames)
    : maximumStateExplorerFrames_(maximumStateExplorerFrames),
      frameHistory(maximumSnapshotsPerKey)
{
    activityClock.start();
}

void AnalysisSession::ingest(const CANFrame &frame)
{
    if (!activityClock.isValid())
    {
        activityClock.start();
    }

    const FrameAggregateKey key = makeKey(frame);

    const qint64 observedActivityMilliseconds =
        activityClock.elapsed();

    aggregateStore.ingest(frame, observedActivityMilliseconds);

    frameHistory.ingest(frame);

    stateExplorerFrames_.append(frame);

    while (static_cast<std::size_t>(stateExplorerFrames_.size())
        > maximumStateExplorerFrames_)
    {
        stateExplorerFrames_.removeFirst();
    }

    lastActivityMilliseconds.insert(key, observedActivityMilliseconds);
}

void AnalysisSession::clear() noexcept
{
    aggregateStore.clear();
    frameHistory.clear();
    markerStore.clear();
    stateExplorerFrames_.clear();
    lastActivityMilliseconds.clear();

    activityClock.restart();
}

std::size_t AnalysisSession::aggregateCount() const noexcept
{
    return aggregateStore.size();
}

bool AnalysisSession::empty() const noexcept
{
    return aggregateStore.empty();
}

QVector<FrameAggregateKey> AnalysisSession::aggregateKeys() const
{
    return aggregateStore.keys();
}

const FrameAggregate *AnalysisSession::findAggregate(
    const FrameAggregateKey &key) const noexcept
{
    return aggregateStore.find(key);
}

const FrameHistorySnapshot *AnalysisSession::latestSnapshot(
    const FrameAggregateKey &key) const noexcept
{
    return frameHistory.latest(key);
}

const FrameHistorySnapshot *AnalysisSession::previousSnapshot(
    const FrameAggregateKey &key) const noexcept
{
    return frameHistory.previous(key);
}

bool AnalysisSession::compareLatest(
    const FrameAggregateKey &key,
    FrameComparison *comparison) const
{
    if (comparison == nullptr)
    {
        return false;
    }

    const FrameHistorySnapshot *previous = frameHistory.previous(key);
    const FrameHistorySnapshot *current = frameHistory.latest(key);

    if (previous == nullptr || current == nullptr)
    {
        return false;
    }

    *comparison = FrameComparisonCalculator::compare(
        *previous,
        *current);

    return true;
}

bool AnalysisSession::stateExplorerSnapshot(
    const FrameAggregateKey &key,
    QVector<CANFrame> *frames) const
{
    if (frames == nullptr)
    {
        return false;
    }

    frames->clear();

    if (aggregateStore.find(key) == nullptr)
    {
        return false;
    }

    frames->reserve(stateExplorerFrames_.size());

    for (const CANFrame &frame : stateExplorerFrames_)
    {
        if (makeKey(frame) == key)
        {
            frames->append(frame);
        }
    }

    return true;
}

FrameAggregateKey AnalysisSession::makeKey(const CANFrame &frame)
{
    return FrameAggregateKey{
        frame.bus,
        static_cast<std::uint32_t>(frame.frameId()),
        frame.frameType(),
        frame.hasExtendedFrameFormat(),
        frame.isReceived};
}

bool AnalysisSession::hasActivityTimestamp(
    const FrameAggregateKey &key) const noexcept
{
    return lastActivityMilliseconds.contains(key);
}

qint64 AnalysisSession::activityAgeMilliseconds(
    const FrameAggregateKey &key) const noexcept
{
    if (!activityClock.isValid())
    {
        return -1;
    }

    const auto iterator = lastActivityMilliseconds.constFind(key);

    if (iterator == lastActivityMilliseconds.constEnd())
    {
        return -1;
    }

    return activityClock.elapsed() - iterator.value();
}

bool AnalysisSession::addMarker(
    const SelectionContext &context,
    const QString &label)
{
    if (!context.hasBus() || !context.hasSingleCanId())
    {
        return false;
    }

    markerStore.addMarker(AnalysisMarker(context, label));

    return true;
}

const QVector<AnalysisMarker> &AnalysisSession::markers() const noexcept
{
    return markerStore.markers();
}
