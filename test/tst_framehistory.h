#ifndef TST_FRAMEHISTORY_H
#define TST_FRAMEHISTORY_H

// Qt headers
#include <QObject>

class TestFrameHistory : public QObject
{
    Q_OBJECT

private slots:
    void firstFrameHasLatestButNoPrevious();
    void secondFrameExposesLatestAndPrevious();
    void depthTwoDiscardsOldestSnapshot();
    void aggregateKeyPartsKeepHistoriesIndependent();
    void snapshotsOwnCopiedData();
    void depthOneHasNoPrevious();
    void zeroDepthRetainsNoSnapshots();
    void clearRemovesAllHistories();
};

#endif // TST_FRAMEHISTORY_H
