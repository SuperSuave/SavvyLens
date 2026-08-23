#ifndef TST_ANALYSISSESSION_H
#define TST_ANALYSISSESSION_H

// Qt headers
#include <QObject>

class TestAnalysisSession : public QObject
{
    Q_OBJECT

private slots:
    void firstFrameCreatesAggregateAndLatestSnapshot();
    void secondSameKeyFrameCreatesComparison();
    void differentKeysRemainIndependent();
    void comparisonUnavailableAfterFirstFrame();
    void comparisonRejectsNullOutputPointer();
    void depthOneNeverCreatesComparison();
    void clearRemovesAggregateAndHistoryState();
    void activityAgeIsAvailableAfterIngest();
    void clearRemovesActivityAgeState();
    void comparisonOwnsDataAfterLaterIngest();
    void stateExplorerSnapshotFiltersByCompleteAggregateKey();
    void stateExplorerSnapshotOwnsFramesAfterLaterIngest();
    void stateExplorerSnapshotSupportsCanIdZero();
    void stateExplorerSnapshotAllowsEmptyFrameVector();
    void clearRemovesStateExplorerSnapshotFrames();

};

#endif // TST_ANALYSISSESSION_H
