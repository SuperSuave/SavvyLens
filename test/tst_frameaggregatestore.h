#ifndef TST_FRAMEAGGREGATESTORE_H
#define TST_FRAMEAGGREGATESTORE_H

// Qt headers
#include <QObject>

class TestFrameAggregateStore : public QObject
{
    Q_OBJECT

private slots:
    void sameKeyIncrementsOccurrenceCount();
    void sameIdOnDifferentBusesCreatesSeparateAggregates();
    void receiveAndTransmitCreateSeparateAggregates();
    void standardAndExtendedCreateSeparateAggregates();
    void differentFrameTypesCreateSeparateAggregates();
    void lastIngestedCopiesPayloadAndMetadata();
    void clearRemovesAllAggregates();
};

#endif // TST_FRAMEAGGREGATESTORE_H
