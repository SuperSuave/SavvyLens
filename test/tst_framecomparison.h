#ifndef TST_FRAMECOMPARISON_H
#define TST_FRAMECOMPARISON_H

// Qt headers
#include <QObject>

class TestFrameComparison : public QObject
{
    Q_OBJECT

private slots:
    void identicalPayloadsHaveNoChanges();
    void changedPayloadProducesExpectedMasks();
    void lengthChangesAreReported();
    void snapshotMetadataIsPreserved();
    void comparisonOwnsCopiedSnapshots();
};

#endif // TST_FRAMECOMPARISON_H
