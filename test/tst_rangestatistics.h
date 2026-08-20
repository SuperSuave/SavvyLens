#ifndef TST_RANGESTATISTICS_H
#define TST_RANGESTATISTICS_H

#include <QObject>

class TestRangeStatistics : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void byteStatsOnConstantPayload();
    void byteStatsOnRampingPayload();
    void multiByteEndiannessExtraction();
    void signedSignalNegativeRange();
    void evaluateLinearRampSignal();
    void evaluateSineWaveSignal();
    void rejectStaticSignal();
    void rejectRandomNoiseSignal();
    void candidateScannerFindsRamp();
    void candidateScannerCancellation();
};

#endif // TST_RANGESTATISTICS_H
