#ifndef ANALYSISSESSION_H
#define ANALYSISSESSION_H

// SavvyLens headers
#include "analysis/analysismarkerstore.h"
#include "analysis/frameaggregatestore.h"
#include "analysis/framecomparison.h"
#include "analysis/framehistory.h"
#include "analysis/selectioncontext.h"

// Qt headers
#include <QElapsedTimer>
#include <QHash>
#include <QVector>

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

    QVector<FrameAggregateKey> aggregateKeys() const;

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

    bool hasActivityTimestamp(
        const FrameAggregateKey &key) const noexcept;

    qint64 activityAgeMilliseconds(
        const FrameAggregateKey &key) const noexcept;

    bool addMarker(
        const SelectionContext &context,
        const QString &label = QString());

    const QVector<AnalysisMarker> &markers() const noexcept;

private:
    FrameAggregateStore aggregateStore;
    FrameHistory frameHistory;
    AnalysisMarkerStore markerStore;

    QElapsedTimer activityClock;
    QHash<FrameAggregateKey, qint64> lastActivityMilliseconds;
};

#endif // ANALYSISSESSION_H
