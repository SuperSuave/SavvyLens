#ifndef TST_STATEEXPLORERPRESENTATION_H
#define TST_STATEEXPLORERPRESENTATION_H

// Qt headers
#include <QObject>

class TestStateExplorerPresentation : public QObject
{
    Q_OBJECT

private slots:
    void mapsCandidateIdentityAndAcceptedSamples();
    void preservesEvidenceOrderingAndStringValues();
    void mapsNoAcceptedSamplesSafely();
    void mapsTruncatedEvidenceFlags();
};

#endif // TST_STATEEXPLORERPRESENTATION_H
