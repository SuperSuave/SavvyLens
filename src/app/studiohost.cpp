#include "studiohost.h"

// SavvyLens headers
#include "stateexplorerpresentation.h"

// Qt headers
#include <QByteArray>
#include <QCloseEvent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWidget>
#include <QUrl>
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

StudioHost::StudioHost(QWidget *parent)
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
        QStringLiteral("studioHost"),
        this);

    quickWidget_->rootContext()->setContextProperty(
        QStringLiteral("stateExplorerPresentation"),
        stateExplorerPresentation_);

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
        stateExplorerDemoFrames_,
        stateExplorerDemoConfig(candidate));
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

CandidateAnalysis::Config StudioHost::stateExplorerDemoConfig(
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

    stateExplorerDemoFrames_.clear();
    stateExplorerDemoFrames_.append(demoFrame(demoCanId, 0));
    stateExplorerDemoFrames_.append(demoFrame(demoCanId, 0));
    stateExplorerDemoFrames_.append(demoFrame(demoCanId, 1));
    stateExplorerDemoFrames_.append(demoFrame(demoCanId, 1));
    stateExplorerDemoFrames_.append(demoFrame(demoCanId, 2));
    stateExplorerDemoFrames_.append(demoFrame(demoCanId, 2));
    stateExplorerDemoFrames_.append(demoFrame(demoCanId, 1));
    stateExplorerDemoFrames_.append(demoFrame(demoCanId, 1));
    stateExplorerDemoFrames_.append(demoFrame(demoCanId, 0));

    analyzeStateExplorerCandidate(
        demoCanId,
        0,
        8,
        true,
        false);
}
