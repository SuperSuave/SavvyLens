#include "stateexplorerpresentation.h"

// Qt headers
#include <QVariantMap>

StateExplorerPresentation::StateExplorerPresentation(QObject *parent)
    : QObject(parent)
{
}

void StateExplorerPresentation::setDeterministicDemoSource()
{
    sourceLabel_ = tr("Source: Deterministic demo evidence");
    sourceScopeText_ = tr(
        "Deterministic in-memory demo evidence. It is not captured traffic "
        "and does not update from live traffic or session history.");
    usesLiveChangeSnapshot_ = false;
    emit changed();
}

void StateExplorerPresentation::setLiveChangeExplorerSnapshotSource(
    const QString &sourceLabel,
    const QString &sourceScopeText)
{
    sourceLabel_ = sourceLabel;
    sourceScopeText_ = sourceScopeText;
    usesLiveChangeSnapshot_ = true;
    emit changed();
}

QString StateExplorerPresentation::sourceLabel() const
{
    return sourceLabel_;
}

QString StateExplorerPresentation::sourceScopeText() const
{
    return sourceScopeText_;
}

bool StateExplorerPresentation::usesLiveChangeSnapshot() const
{
    return usesLiveChangeSnapshot_;
}

SelectionContext StateExplorerPresentation::selectionContextForState(
    int stateIndex) const
{
    SelectionContext context;
    if (!hasResult_ || stateIndex < 0 || stateIndex >= result_.discreteState.observedValues.size())
    {
        return context;
    }

    const DiscreteStateObservation &observation =
        result_.discreteState.observedValues.at(stateIndex);

    context.setCanId(result_.candidate.canId);
    if (result_.candidate.startBit >= 0 && result_.candidate.bitLength > 0)
    {
        context.setBitRange(result_.candidate.startBit, result_.candidate.bitLength);
    }
    if (bus_ >= 0)
    {
        context.setBus(bus_);
    }
    if (observation.firstSampleIndex >= 0)
    {
        context.setFrameIndex(observation.firstSampleIndex);
    }

    return context;
}

SelectionContext StateExplorerPresentation::selectionContextForTransition(
    int transitionIndex) const
{
    SelectionContext context;
    if (!hasResult_ || transitionIndex < 0 || transitionIndex >= result_.transitions.transitions.size())
    {
        return context;
    }

    const TransitionAnalysis::TransitionEvidence &transition =
        result_.transitions.transitions.at(transitionIndex);

    context.setCanId(result_.candidate.canId);
    if (result_.candidate.startBit >= 0 && result_.candidate.bitLength > 0)
    {
        context.setBitRange(result_.candidate.startBit, result_.candidate.bitLength);
    }
    if (bus_ >= 0)
    {
        context.setBus(bus_);
    }
    if (transition.firstAcceptedSampleIndex >= 0)
    {
        context.setFrameIndex(transition.firstAcceptedSampleIndex);
    }

    return context;
}

void StateExplorerPresentation::setEvidence(
    const QVector<CANFrame> &frames,
    const CandidateAnalysis::Config &config)
{
    bus_ = !frames.isEmpty() ? frames.first().bus : -1;
    result_ = CandidateAnalysis::analyze(frames, config);
    
    RangeSignalSpec rangeSpec;
    rangeSpec.canId = config.candidate.canId;
    rangeSpec.startBit = config.candidate.startBit;
    rangeSpec.bitLength = config.candidate.bitLength;
    rangeSpec.isLittleEndian = config.candidate.isLittleEndian;
    rangeSpec.isSigned = config.candidate.isSigned;
    
    rangeStats_ = RangeStatistics::evaluateSignal(frames, rangeSpec);
    
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

QString StateExplorerPresentation::minValueText() const
{
    return QString::number(rangeStats_.minValue);
}

QString StateExplorerPresentation::maxValueText() const
{
    return QString::number(rangeStats_.maxValue);
}

QString StateExplorerPresentation::rangeSpanText() const
{
    return QString::number(rangeStats_.rangeSpan);
}

QString StateExplorerPresentation::rangeCoverageText() const
{
    return QString::number(rangeStats_.rangeCoverage * 100.0, 'f', 1) + QStringLiteral("%");
}

int StateExplorerPresentation::uniqueValueCount() const
{
    return rangeStats_.uniqueValueCount;
}

QString StateExplorerPresentation::smoothnessScoreText() const
{
    return QString::number(rangeStats_.smoothnessScore, 'f', 1);
}

bool StateExplorerPresentation::isRanging() const
{
    return rangeStats_.isRanging;
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
