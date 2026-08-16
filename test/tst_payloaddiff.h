#ifndef TST_PAYLOADDIFF_H
#define TST_PAYLOADDIFF_H

// Qt headers
#include <QObject>

class TestPayloadDiff : public QObject
{
    Q_OBJECT

private slots:
    void identicalPayloadsHaveNoChanges();
    void changedByteProducesByteAndBitMasks();
    void multipleChangedBytesProduceIndependentMasks();
    void payloadGrowthMarksIntroducedBytes();
    void payloadShrinkMarksRemovedBytes();
    void emptyPayloadsHaveNoChanges();
};

#endif // TST_PAYLOADDIFF_H
