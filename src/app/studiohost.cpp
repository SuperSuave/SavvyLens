#include "studiohost.h"

// SavvyLens headers
#include "stateexplorerpresentation.h"
#include "analysis/livechangeexplorermodel.h"

// Qt headers
#include <QByteArray>
#include <QCanBusFrame>
#include <QCloseEvent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWidget>
#include <QUrl>
#include <QVariant>
#include <QVBoxLayout>

namespace
{
    CANFrame demoFrame(quint32 canId, quint8 value)
    {
        CANFrame frame;
        frame.setFrameId(canId);
        frame.setPayload(QByteArray(1, static_cast<char>(value)));
        return frame;
    }
}

StudioHost::StudioHost(
    LiveChangeExplorerModel *liveChangeExplorerModel,
    QWidget *parent)
    : QWidget(parent, Qt::Window)
{
    qRegisterMetaType<SelectionContext>("SelectionContext");

    setWindowTitle(tr("SavvyLens Studio"));
    resize(1800, 1075);
    setMinimumSize(1280, 760);

    stateExplorerPresentation_ =
        new StateExplorerPresentation(this);
    loadStateExplorerDemo();

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    quickWidget_ = new QQuickWidget(this);
    quickWidget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    quickWidget_->engine()->addImportPath(QStringLiteral("qrc:/qml"));

    quickWidget_->rootContext()->setContextProperty(
        QStringLiteral("liveChangeExplorerModel"),
        liveChangeExplorerModel);

    quickWidget_->rootContext()->setContextProperty(
        QStringLiteral("studioHost"),
        this);

    quickWidget_->rootContext()->setContextProperty(
        QStringLiteral("stateExplorerPresentation"),
        stateExplorerPresentation_);

    quickWidget_->setSource(
        QUrl(QStringLiteral("qrc:/qml/Studio.qml")));

    layout->addWidget(quickWidget_);
}

QStringList StudioHost::demoScenarioNames() const
{
    return {
        QStringLiteral("Default (Multi-State)"),
        QStringLiteral("Static Value"),
        QStringLiteral("Sequential Counter"),
        QStringLiteral("Empty / Unmatched ID")
    };
}

bool StudioHost::loadDemoScenario(int scenarioIndex)
{
    constexpr quint32 demoCanId = 0x321;

    stateExplorerFrames_.clear();
    hasSnapshotKey_ = false;

    switch (scenarioIndex)
    {
    case 0:
        stateExplorerFrames_.append(demoFrame(demoCanId, 0));
        stateExplorerFrames_.append(demoFrame(demoCanId, 0));
        stateExplorerFrames_.append(demoFrame(demoCanId, 1));
        stateExplorerFrames_.append(demoFrame(demoCanId, 1));
        stateExplorerFrames_.append(demoFrame(demoCanId, 2));
        stateExplorerFrames_.append(demoFrame(demoCanId, 2));
        stateExplorerFrames_.append(demoFrame(demoCanId, 1));
        stateExplorerFrames_.append(demoFrame(demoCanId, 1));
        stateExplorerFrames_.append(demoFrame(demoCanId, 0));
        break;

    case 1:
        stateExplorerFrames_.append(demoFrame(demoCanId, 5));
        stateExplorerFrames_.append(demoFrame(demoCanId, 5));
        stateExplorerFrames_.append(demoFrame(demoCanId, 5));
        stateExplorerFrames_.append(demoFrame(demoCanId, 5));
        stateExplorerFrames_.append(demoFrame(demoCanId, 5));
        break;

    case 2:
        for (quint8 value = 0; value <= 7; ++value)
        {
            stateExplorerFrames_.append(demoFrame(demoCanId, value));
        }
        break;

    case 3:
        break;

    default:
        return false;
    }

    stateExplorerPresentation_->setDeterministicDemoSource();

    if (hasCandidate_ && currentCandidate_.isValid())
    {
        stateExplorerPresentation_->setEvidence(
            stateExplorerFrames_,
            stateExplorerConfig(currentCandidate_));
    }
    else
    {
        analyzeStateExplorerCandidate(
            demoCanId,
            0,
            8,
            true,
            false);
    }

    return true;
}

bool StudioHost::analyzeStateExplorerCandidate(quint32 canId,
                                               int startBit,
                                               int bitLength,
                                               bool isLittleEndian,
                                               bool isSigned)
{
    RangeSignalSpec candidate;
    candidate.canId = canId;
    candidate.startBit = startBit;
    candidate.bitLength = bitLength;
    candidate.isLittleEndian = isLittleEndian;
    candidate.isSigned = isSigned;

    if (!candidate.isValid())
        return false;

    currentCandidate_ = candidate;
    hasCandidate_ = true;

    stateExplorerPresentation_->setEvidence(
        stateExplorerFrames_,
        stateExplorerConfig(candidate));
    return true;
}

void StudioHost::closeStudio()
{
    close();
}

void StudioHost::closeEvent(QCloseEvent *event)
{
    emit studioClosed();
    QWidget::closeEvent(event);
}

CandidateAnalysis::Config StudioHost::stateExplorerConfig(
    const RangeSignalSpec &candidate) const
{
    CandidateAnalysis::Config config;
    config.candidate = candidate;
    config.discreteState.maxDistinctValues = 32;
    config.maximumDistinctTransitions = 64;
    config.maximumRetainedRuns = 64;
    return config;
}

void StudioHost::loadStateExplorerDemo()
{
    loadDemoScenario(0);
}

void StudioHost::openTrafficWorkspace()
{
    openWorkspace(QStringLiteral("traffic"));
}

void StudioHost::exploreLiveChangeRowInStateExplorer(int row)
{
    if (row < 0)
    {
        return;
    }

    emit exploreLiveChangeRowRequested(row);
}

bool StudioHost::retakeStateExplorerSnapshot()
{
    if (!hasSnapshotKey_)
    {
        return false;
    }

    emit refreshStateExplorerSnapshotRequested(currentSnapshotKey_);
    return true;
}

void StudioHost::openFrameInfoForStateExplorerContext(
    const SelectionContext &context)
{
    emit openFrameInfoRequested(context);
}

void StudioHost::openGraphingForStateExplorerContext(
    const SelectionContext &context)
{
    emit openGraphingRequested(context);
}

void StudioHost::openFrameInfoForState(int stateIndex)
{
    if (stateExplorerPresentation_ != nullptr)
    {
        openFrameInfoForStateExplorerContext(
            stateExplorerPresentation_->selectionContextForState(stateIndex));
    }
}

void StudioHost::openGraphingForState(int stateIndex)
{
    if (stateExplorerPresentation_ != nullptr)
    {
        openGraphingForStateExplorerContext(
            stateExplorerPresentation_->selectionContextForState(stateIndex));
    }
}

void StudioHost::openFrameInfoForTransition(int transitionIndex)
{
    if (stateExplorerPresentation_ != nullptr)
    {
        openFrameInfoForStateExplorerContext(
            stateExplorerPresentation_->selectionContextForTransition(transitionIndex));
    }
}

void StudioHost::openGraphingForTransition(int transitionIndex)
{
    if (stateExplorerPresentation_ != nullptr)
    {
        openGraphingForStateExplorerContext(
            stateExplorerPresentation_->selectionContextForTransition(transitionIndex));
    }
}

void StudioHost::loadStateExplorerSnapshot(
    const FrameAggregateKey &key,
    const QVector<CANFrame> &frames,
    const SelectionContext &context)
{
    currentSnapshotKey_ = key;
    hasSnapshotKey_ = true;
    stateExplorerFrames_ = frames;

    const QString sourceLabel = stateExplorerSnapshotSourceLabel(key);
    const QString sourceScopeText = tr(
        "Bounded retained evidence snapshot. It is not live traffic and may "
        "exclude older matching frames that aged out of the session-wide "
        "rolling buffer.");

    stateExplorerPresentation_->setLiveChangeExplorerSnapshotSource(
        sourceLabel,
        sourceScopeText);

    openWorkspace(QStringLiteral("explore"));

    if (context.bitRange().isValid() || context.hasSingleCanId() || !context.canIds().isEmpty())
    {
        seedStateExplorerCandidateFromContext(context);
    }
    else if (quickWidget_ != nullptr &&
             quickWidget_->rootObject() != nullptr)
    {
        QMetaObject::invokeMethod(
            quickWidget_->rootObject(),
            "seedStateExplorerCandidate",
            Q_ARG(QVariant, QVariant::fromValue(
                QStringLiteral("0x%1")
                    .arg(key.frameId, 3, 16, QLatin1Char('0'))
                    .toUpper()
                    .replace(0, 2, QStringLiteral("0x")))));
    }
}

void StudioHost::seedStateExplorerCandidateFromContext(
    const SelectionContext &context)
{
    if (quickWidget_ == nullptr ||
        quickWidget_->rootObject() == nullptr)
    {
        return;
    }

    quint32 frameId = 0;
    if (context.hasSingleCanId())
    {
        frameId = context.canId();
    }
    else if (!context.canIds().isEmpty())
    {
        frameId = *context.canIds().constBegin();
    }
    else if (hasSnapshotKey_)
    {
        frameId = currentSnapshotKey_.frameId;
    }

    const QString canIdText = QStringLiteral("0x%1")
                                  .arg(frameId, 3, 16, QLatin1Char('0'))
                                  .toUpper()
                                  .replace(0, 2, QStringLiteral("0x"));

    int startBit = 0;
    int bitLength = 8;
    bool isLittleEndian = true;
    bool isSigned = false;

    if (context.bitRange().isValid())
    {
        startBit = context.bitRange().startBit;
        bitLength = context.bitRange().bitLength;
    }

    openWorkspace(QStringLiteral("explore"));

    QMetaObject::invokeMethod(
        quickWidget_->rootObject(),
        "seedStateExplorerCandidateWithSpec",
        Q_ARG(QVariant, QVariant::fromValue(canIdText)),
        Q_ARG(QVariant, QVariant::fromValue(startBit)),
        Q_ARG(QVariant, QVariant::fromValue(bitLength)),
        Q_ARG(QVariant, QVariant::fromValue(isLittleEndian)),
        Q_ARG(QVariant, QVariant::fromValue(isSigned)));
}

void StudioHost::openWorkspace(const QString &workspaceId)
{
    if (quickWidget_ == nullptr ||
        quickWidget_->rootObject() == nullptr)
    {
        return;
    }

    QMetaObject::invokeMethod(
        quickWidget_->rootObject(),
        "openWorkspace",
        Q_ARG(QVariant, workspaceId));
}

QString StudioHost::stateExplorerSnapshotSourceLabel(
    const FrameAggregateKey &key) const
{
    const QString canId = QStringLiteral("0x%1")
                              .arg(key.frameId, 3, 16, QLatin1Char('0'))
                              .toUpper()
                              .replace(0, 2, QStringLiteral("0x"));

    const QString direction = key.isReceived
                                  ? QStringLiteral("Rx")
                                  : QStringLiteral("Tx");

    const QString format = key.hasExtendedFrameFormat
                               ? QStringLiteral("Extended")
                               : QStringLiteral("Standard");

    QString frameType = QStringLiteral("Unknown");

    switch (key.frameType)
    {
    case QCanBusFrame::DataFrame:
        frameType = QStringLiteral("Data");
        break;

    case QCanBusFrame::RemoteRequestFrame:
        frameType = QStringLiteral("RTR");
        break;

    case QCanBusFrame::ErrorFrame:
        frameType = QStringLiteral("Error");
        break;

    case QCanBusFrame::InvalidFrame:
        frameType = QStringLiteral("Invalid");
        break;

    case QCanBusFrame::UnknownFrame:
    default:
        break;
    }

    return QStringLiteral(
               "Source: Live Change Explorer snapshot · "
               "Bus %1 · %2 · %3 · %4 · %5")
        .arg(key.bus)
        .arg(direction)
        .arg(format)
        .arg(frameType)
        .arg(canId);
}