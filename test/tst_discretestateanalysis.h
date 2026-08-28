#ifndef TST_DISCRETESTATEANALYSIS_H
#define TST_DISCRETESTATEANALYSIS_H

// Qt headers
#include <QObject>

class TestDiscreteStateAnalysis : public QObject {
    Q_OBJECT

private slots:
    void emptyInput();
    void staticSignal();
    void toggleSignalTracksCountsAndIndexes();
    void observationsAreOrderedByValue();
    void signedNegativeValues();
    void filtersFramesByCanId();
    void skipsUnsupportedMixedPayloadLengths();
    void exactDistinctValueLimitCompletes();
    void oneDistinctValueOverLimitTruncates();
    void zeroDistinctValueLimitTruncatesNonEmptyInput();
    void doesNotMutateInputFrames();
};

#endif // TST_DISCRETESTATEANALYSIS_H
