#ifndef TST_TEMPORALANALYSIS_H
#define TST_TEMPORALANALYSIS_H

#include <QObject>

class TestTemporalAnalysis : public QObject {
  Q_OBJECT

private slots:
  void emptyInput();
  void singleAcceptedSampleIsStatic();
  void staticSequenceProducesOneRun();
  void multipleSelfRunsProduceOrderedRuns();
  void toggleSequenceProducesOrderedRuns();
  void signedValuesAreRetained();
  void filteringAndShortFramesDoNotConsumeAcceptedIndexes();
  void exactRunLimitCompletes();
  void limitOverflowIsBoundedAndTruncated();
  void zeroLimitIsBoundedAndTruncatedForNonEmptyInput();
  void negativeLimitIsBoundedAndTruncatedForNonEmptyInput();
  void retainedRunContinuesUntilItsBoundary();
  void doesNotMutateInputFrames();
};

#endif // TST_TEMPORALANALYSIS_H
