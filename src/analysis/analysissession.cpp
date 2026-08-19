// SavvyLens headers
#include "analysissession.h"
#include "can/can_structs.h"

AnalysisSession::AnalysisSession(
    std::size_t maximumSnapshotsPerKey)
    : frameHistory(maximumSnapshotsPerKey)
{
}

void AnalysisSession::ingest(const CANFrame &frame)
{
    aggregateStore.ingest(frame);
    frameHistory.ingest(frame);
}

void AnalysisSession::clear() noexcept
{
    aggregateStore.clear();
    frameHistory.clear();
    markerStore.clear();
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

    *comparison = FrameComparisonCalculator::compare(*previous, *current);

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
