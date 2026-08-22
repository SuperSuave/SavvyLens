#include "stateexplorerpresentation.h"

// Qt headers
#include <QVariantMap>

StateExplorerPresentation::StateExplorerPresentation(QObject *parent)
    : QObject(parent)
{
}

void StateExplorerPresentation::setEvidence(
    const QVector<CANFrame> &frames,
    const CandidateAnalysis::Config &config)
{
    result_ = CandidateAnalysis::analyze(frames, config);
    hasResult_ = true;
    emit changed();
}

QString StateExplorerPresentation::canIdText() const
{
    return canIdTextFor(result_.candidate.canId);
}

int StateExplorerPresentation::startBit() const
{
    return result_.candidate.startBit;
}

int StateExplorerPresentation::bitLength() const
{
    return result_.candidate.bitLength;
}

QString StateExplorerPresentation::endianText() const
{
    return result_.candidate.isLittleEndian
               ? tr("Little endian")
               : tr("Big endian");
}

QString StateExplorerPresentation::signednessText() const
{
    return result_.candidate.isSigned
               ? tr("Signed")
               : tr("Unsigned");
}

QString StateExplorerPresentation::candidateDisplayName() const
{
    return result_.candidate.displayName();
}

int StateExplorerPresentation::acceptedSampleCount() const
{
    return result_.discreteState.sampleCount;
}

bool StateExplorerPresentation::hasEvidence() const
{
    return hasResult_ && acceptedSampleCount() > 0;
}

QString StateExplorerPresentation::discreteClassificationText() const
{
    return discreteClassificationTextFor(result_.discreteState.classification);
}

bool StateExplorerPresentation::discreteCompleted() const
{
    return result_.discreteState.outcome == DiscreteStateAnalysisOutcome::Completed;
}

bool StateExplorerPresentation::discreteTruncated() const
{
    return !discreteCompleted();
}

QVariantList StateExplorerPresentation::observedStates() const
{
    return stateRows(result_.discreteState.observedValues);
}

QString StateExplorerPresentation::transitionClassificationText() const
{
    return transitionClassificationTextFor(result_.transitions.classification);
}

bool StateExplorerPresentation::transitionCompleted() const
{
    return result_.transitions.completed;
}

bool StateExplorerPresentation::transitionTruncated() const
{
    return !transitionCompleted();
}

int StateExplorerPresentation::transitionValueChangeCount() const
{
    return result_.transitions.valueChangeCount;
}

QVariantList StateExplorerPresentation::observedTransitions() const
{
    return transitionRows(result_.transitions.transitions);
}

QString StateExplorerPresentation::temporalClassificationText() const
{
    return temporalClassificationTextFor(result_.temporalRuns.classification);
}

bool StateExplorerPresentation::temporalCompleted() const
{
    return result_.temporalRuns.completed;
}

bool StateExplorerPresentation::temporalTruncated() const
{
    return !temporalCompleted();
}

int StateExplorerPresentation::temporalValueChangeCount() const
{
    return result_.temporalRuns.valueChangeCount;
}

QVariantList StateExplorerPresentation::observedRuns() const
{
    return runRows(result_.temporalRuns.runs);
}

QString StateExplorerPresentation::canIdTextFor(quint32 canId)
{
    return QStringLiteral("0x%1")
        .arg(canId, 3, 16, QLatin1Char('0'))
        .toUpper()
        .replace(0, 2, QStringLiteral("0x"));
}

QString StateExplorerPresentation::discreteClassificationTextFor(
    DiscreteStateClassification classification)
{
    switch (classification)
    {
    case DiscreteStateClassification::NoSamples:
        return QObject::tr("No accepted samples");
    case DiscreteStateClassification::Static:
        return QObject::tr("Static observed value");
    case DiscreteStateClassification::CandidateDiscrete:
        return QObject::tr("Bounded observed values");
    case DiscreteStateClassification::HighCardinalityOrTruncated:
        return QObject::tr("High cardinality or incomplete evidence");
    }

    return QObject::tr("Unknown");
}

QString StateExplorerPresentation::transitionClassificationTextFor(
    TransitionAnalysis::Classification classification)
{
    switch (classification)
    {
    case TransitionAnalysis::Classification::NoSamples:
        return QObject::tr("No accepted samples");
    case TransitionAnalysis::Classification::Static:
        return QObject::tr("No observed value changes");
    case TransitionAnalysis::Classification::CandidateTransitions:
        return QObject::tr("Observed directed changes");
    case TransitionAnalysis::Classification::HighCardinalityOrTruncated:
        return QObject::tr("High cardinality or incomplete evidence");
    }

    return QObject::tr("Unknown");
}

QString StateExplorerPresentation::temporalClassificationTextFor(
    TemporalAnalysis::Classification classification)
{
    switch (classification)
    {
    case TemporalAnalysis::Classification::NoSamples:
        return QObject::tr("No accepted samples");
    case TemporalAnalysis::Classification::Static:
        return QObject::tr("One observed run");
    case TemporalAnalysis::Classification::CandidateTemporalRuns:
        return QObject::tr("Observed consecutive runs");
    case TemporalAnalysis::Classification::HighCardinalityOrTruncated:
        return QObject::tr("High cardinality or incomplete evidence");
    }

    return QObject::tr("Unknown");
}

QVariantList StateExplorerPresentation::stateRows(
    const QVector<DiscreteStateObservation> &observations)
{
    QVariantList rows;

    for (const DiscreteStateObservation &observation : observations)
    {
        QVariantMap row;
        row.insert(QStringLiteral("valueText"),
                   QString::number(observation.value));
        row.insert(QStringLiteral("occurrenceCount"),
                   observation.occurrenceCount);
        row.insert(QStringLiteral("firstSampleIndex"),
                   observation.firstSampleIndex);
        row.insert(QStringLiteral("lastSampleIndex"),
                   observation.lastSampleIndex);
        rows.append(row);
    }

    return rows;
}

QVariantList StateExplorerPresentation::transitionRows(
    const QVector<TransitionAnalysis::TransitionEvidence> &transitions)
{
    QVariantList rows;

    for (const TransitionAnalysis::TransitionEvidence &transition : transitions)
    {
        QVariantMap row;
        row.insert(QStringLiteral("fromValueText"),
                   QString::number(transition.fromValue));
        row.insert(QStringLiteral("toValueText"),
                   QString::number(transition.toValue));
        row.insert(QStringLiteral("occurrenceCount"),
                   transition.occurrenceCount);
        row.insert(QStringLiteral("firstSampleIndex"),
                   transition.firstAcceptedSampleIndex);
        row.insert(QStringLiteral("lastSampleIndex"),
                   transition.lastAcceptedSampleIndex);
        rows.append(row);
    }

    return rows;
}

QVariantList StateExplorerPresentation::runRows(
    const QVector<TemporalAnalysis::RunEvidence> &runs)
{
    QVariantList rows;

    for (const TemporalAnalysis::RunEvidence &run : runs)
    {
        QVariantMap row;
        row.insert(QStringLiteral("valueText"),
                   QString::number(run.value));
        row.insert(QStringLiteral("sampleCount"),
                   run.sampleCount);
        row.insert(QStringLiteral("firstSampleIndex"),
                   run.firstAcceptedSampleIndex);
        row.insert(QStringLiteral("lastSampleIndex"),
                   run.lastAcceptedSampleIndex);
        rows.append(row);
    }

    return rows;
}
