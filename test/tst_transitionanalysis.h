#ifndef TST_TRANSITIONANALYSIS_H
#define TST_TRANSITIONANALYSIS_H

#include <QObject>

class TestTransitionAnalysis : public QObject {
  Q_OBJECT

private slots:
  void emptyInput();
  void singleAcceptedSampleIsStatic();
  void staticSequenceHasNoTransitions();
  void selfRunsDoNotCreateTransitions();
  void toggleSequenceAggregatesDirectedTransitions();
  void transitionsUseDeterministicOrdering();
  void signedValuesAreRetained();
  void filteringAndShortFramesDoNotConsumeAcceptedIndexes();
  void exactTransitionLimitCompletes();
  void limitOverflowIsBoundedAndTruncated();
  void zeroLimitIsBoundedAndTruncatedForChanges();
  void doesNotMutateInputFrames();
};

#endif // TST_TRANSITIONANALYSIS_H
