#include "app/studiohost.h"

// SavvyLens headers
#include "app/stateexplorerpresentation.h"

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

void StudioHost::closeStudio()
{
    close();
}

void StudioHost::closeEvent(QCloseEvent *event)
{
    emit studioClosed();
    QWidget::closeEvent(event);
}

void StudioHost::loadStateExplorerDemo()
{
    constexpr quint32 demoCanId = 0x321;

    QVector<CANFrame> frames;
    frames.append(demoFrame(demoCanId, 0));
    frames.append(demoFrame(demoCanId, 0));
    frames.append(demoFrame(demoCanId, 1));
    frames.append(demoFrame(demoCanId, 1));
    frames.append(demoFrame(demoCanId, 2));
    frames.append(demoFrame(demoCanId, 2));
    frames.append(demoFrame(demoCanId, 1));
    frames.append(demoFrame(demoCanId, 1));
    frames.append(demoFrame(demoCanId, 0));

    CandidateAnalysis::Config config;
    config.candidate.canId = demoCanId;
    config.candidate.startBit = 0;
    config.candidate.bitLength = 8;
    config.candidate.isLittleEndian = true;
    config.candidate.isSigned = false;

    config.discreteState.maxDistinctValues = 32;
    config.maximumDistinctTransitions = 64;
    config.maximumRetainedRuns = 64;

    stateExplorerPresentation_->setEvidence(frames, config);
}
