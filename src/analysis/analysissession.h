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
        std::size_t maximumSnapshotsPerKey = 2,
        std::size_t maximumStateExplorerFrames = 4096);

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

    bool stateExplorerSnapshot(
        const FrameAggregateKey &key,
        QVector<CANFrame> *frames) const;

    static FrameAggregateKey makeKey(const CANFrame &frame);

    bool hasActivityTimestamp(
        const FrameAggregateKey &key) const noexcept;

    qint64 activityAgeMilliseconds(
        const FrameAggregateKey &key) const noexcept;

    bool addMarker(
        const SelectionContext &context,
        const QString &label = QString());

    const QVector<AnalysisMarker> &markers() const noexcept;

    std::size_t maximumStateExplorerFrames;
    QVector<CANFrame> stateExplorerFrames;

private:
    // A bounded, chronological, session-wide raw-frame retention window used
    // only to create one-shot State Explorer snapshots. The capacity applies
    // across all aggregate identities, not per FrameAggregateKey.
    std::size_t maximumStateExplorerFrames_;

    // Contains C++-owned value copies of the newest retained raw frames.
    // A State Explorer snapshot filters this rolling window by complete
    // FrameAggregateKey identity and may therefore be empty when a valid
    // aggregate's older matching frames have aged out.
    QVector<CANFrame> stateExplorerFrames_;

    FrameAggregateStore aggregateStore;
    FrameHistory frameHistory;
    AnalysisMarkerStore markerStore;

    QElapsedTimer activityClock;
    QHash<FrameAggregateKey, qint64> lastActivityMilliseconds;
};

#endif // ANALYSISSESSION_H
