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
    void explicitCandidateRefreshesIdentityAndEvidence();
    void invalidCandidateIsRejectedBeforePresentationRefresh();
    void canIdZeroRemainsSupported();
    void explicitEndianAndSignednessAreForwarded();
    void validCandidateWithNoAcceptedSamplesIsReadable();
    void sourcePresentationDistinguishesDemoAndSnapshot();
    void sourcePresentationChangesWithoutRefreshingEvidence();
    void preservesCandidateParametersAcrossSnapshotRefetches();
    void rangeSummarySurfacesMetricsAndStringFormatting();
    void rangeSummaryHandlesZeroSampleEvidenceSafely();
};

#endif // TST_STATEEXPLORERPRESENTATION_H
