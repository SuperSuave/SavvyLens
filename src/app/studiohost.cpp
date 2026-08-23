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

    quickWidget_->rootContext()->setContextProperty(
        QStringLiteral("stateExplorerSourceLabel"),
        stateExplorerSourceLabel_);

    quickWidget_->rootContext()->setContextProperty(
        QStringLiteral("stateExplorerUsesLiveChangeSnapshot"),
        stateExplorerUsesLiveChangeSnapshot_);

    quickWidget_->setSource(
        QUrl(QStringLiteral("qrc:/qml/Studio.qml")));

    layout->addWidget(quickWidget_);
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
    constexpr quint32 demoCanId = 0x321;

    stateExplorerFrames_.clear();
    stateExplorerFrames_.append(demoFrame(demoCanId, 0));
    stateExplorerFrames_.append(demoFrame(demoCanId, 0));
    stateExplorerFrames_.append(demoFrame(demoCanId, 1));
    stateExplorerFrames_.append(demoFrame(demoCanId, 1));
    stateExplorerFrames_.append(demoFrame(demoCanId, 2));
    stateExplorerFrames_.append(demoFrame(demoCanId, 2));
    stateExplorerFrames_.append(demoFrame(demoCanId, 1));
    stateExplorerFrames_.append(demoFrame(demoCanId, 1));
    stateExplorerFrames_.append(demoFrame(demoCanId, 0));

    stateExplorerSourceLabel_ =
        QStringLiteral("Source: Deterministic demo evidence");
    stateExplorerUsesLiveChangeSnapshot_ = false;

    analyzeStateExplorerCandidate(
        demoCanId,
        0,
        8,
        true,
        false);
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

void StudioHost::loadStateExplorerSnapshot(
    const FrameAggregateKey &key,
    const QVector<CANFrame> &frames)
{
    stateExplorerFrames_ = frames;
    stateExplorerSourceLabel_ =
        stateExplorerSnapshotSourceLabel(key);
    stateExplorerUsesLiveChangeSnapshot_ = true;

    if (quickWidget_ != nullptr &&
        quickWidget_->rootObject() != nullptr)
    {
        quickWidget_->rootObject()->setProperty(
            "stateExplorerSourceLabel",
            stateExplorerSourceLabel_);

        quickWidget_->rootObject()->setProperty(
            "stateExplorerUsesLiveChangeSnapshot",
            stateExplorerUsesLiveChangeSnapshot_);
    }

    openWorkspace(QStringLiteral("explore"));

    if (quickWidget_ != nullptr &&
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