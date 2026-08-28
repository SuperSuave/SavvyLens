#ifndef TST_CANDIDATEANALYSIS_H
#define TST_CANDIDATEANALYSIS_H

#include <QObject>

class TestCandidateAnalysis : public QObject
{
    Q_OBJECT

private slots:
    void emptyInputIsConsistentAcrossAnalyses();
    void appliesOneCandidateToEveryAnalyzer();
    void forwardsIndependentEvidenceLimits();
    void preservesSignedExtraction();
    void filteringAndShortFramesUseAcceptedSampleSequence();
    void doesNotMutateInputFrames();
};

#endif // TST_CANDIDATEANALYSIS_H