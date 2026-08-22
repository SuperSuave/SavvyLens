#ifndef STATEEXPLORERPRESENTATION_H
#define STATEEXPLORERPRESENTATION_H

// SavvyLens headers
#include "analysis/candidateanalysis.h"

// Qt headers
#include <QObject>
#include <QVariantList>

class StateExplorerPresentation final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString canIdText READ canIdText NOTIFY changed)
    Q_PROPERTY(int startBit READ startBit NOTIFY changed)
    Q_PROPERTY(int bitLength READ bitLength NOTIFY changed)
    Q_PROPERTY(QString endianText READ endianText NOTIFY changed)
    Q_PROPERTY(QString signednessText READ signednessText NOTIFY changed)
    Q_PROPERTY(QString candidateDisplayName READ candidateDisplayName NOTIFY changed)

    Q_PROPERTY(int acceptedSampleCount READ acceptedSampleCount NOTIFY changed)
    Q_PROPERTY(bool hasEvidence READ hasEvidence NOTIFY changed)

    Q_PROPERTY(QString discreteClassificationText READ discreteClassificationText
                   NOTIFY changed)
    Q_PROPERTY(bool discreteCompleted READ discreteCompleted NOTIFY changed)
    Q_PROPERTY(bool discreteTruncated READ discreteTruncated NOTIFY changed)
    Q_PROPERTY(QVariantList observedStates READ observedStates NOTIFY changed)

    Q_PROPERTY(QString transitionClassificationText READ transitionClassificationText
                   NOTIFY changed)
    Q_PROPERTY(bool transitionCompleted READ transitionCompleted NOTIFY changed)
    Q_PROPERTY(bool transitionTruncated READ transitionTruncated NOTIFY changed)
    Q_PROPERTY(int transitionValueChangeCount READ transitionValueChangeCount
                   NOTIFY changed)
    Q_PROPERTY(QVariantList observedTransitions READ observedTransitions
                   NOTIFY changed)

    Q_PROPERTY(QString temporalClassificationText READ temporalClassificationText
                   NOTIFY changed)
    Q_PROPERTY(bool temporalCompleted READ temporalCompleted NOTIFY changed)
    Q_PROPERTY(bool temporalTruncated READ temporalTruncated NOTIFY changed)
    Q_PROPERTY(int temporalValueChangeCount READ temporalValueChangeCount
                   NOTIFY changed)
    Q_PROPERTY(QVariantList observedRuns READ observedRuns NOTIFY changed)

public:
    explicit StateExplorerPresentation(QObject *parent = nullptr);

    void setEvidence(const QVector<CANFrame> &frames,
                     const CandidateAnalysis::Config &config);

    QString canIdText() const;
    int startBit() const;
    int bitLength() const;
    QString endianText() const;
    QString signednessText() const;
    QString candidateDisplayName() const;

    int acceptedSampleCount() const;
    bool hasEvidence() const;

    QString discreteClassificationText() const;
    bool discreteCompleted() const;
    bool discreteTruncated() const;
    QVariantList observedStates() const;

    QString transitionClassificationText() const;
    bool transitionCompleted() const;
    bool transitionTruncated() const;
    int transitionValueChangeCount() const;
    QVariantList observedTransitions() const;

    QString temporalClassificationText() const;
    bool temporalCompleted() const;
    bool temporalTruncated() const;
    int temporalValueChangeCount() const;
    QVariantList observedRuns() const;

signals:
    void changed();

private:
    CandidateAnalysis::Result result_;
    bool hasResult_ = false;

    static QString canIdTextFor(quint32 canId);
    static QString discreteClassificationTextFor(
        DiscreteStateClassification classification);
    static QString transitionClassificationTextFor(
        TransitionAnalysis::Classification classification);
    static QString temporalClassificationTextFor(
        TemporalAnalysis::Classification classification);

    static QVariantList stateRows(
        const QVector<DiscreteStateObservation> &observations);
    static QVariantList transitionRows(
        const QVector<TransitionAnalysis::TransitionEvidence> &transitions);
    static QVariantList runRows(
        const QVector<TemporalAnalysis::RunEvidence> &runs);
};

#endif // STATEEXPLORERPRESENTATION_H
