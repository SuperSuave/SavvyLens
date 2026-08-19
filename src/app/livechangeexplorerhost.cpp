#include "livechangeexplorerhost.h"

// SavvyLens headers
#include "analysis/livechangeexplorermodel.h"

// Qt headers
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWidget>
#include <QUrl>
#include <QVariant>
#include <QVBoxLayout>

LiveChangeExplorerHost::LiveChangeExplorerHost(
    LiveChangeExplorerModel *model,
    QWidget *parent)
    : QWidget(parent, Qt::Window)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    quickWidget_ = new QQuickWidget(this);
    quickWidget_->setResizeMode(QQuickWidget::SizeRootObjectToView);

    quickWidget_->engine()->addImportPath(
        QStringLiteral("qrc:/qml"));

    quickWidget_->rootContext()->setContextProperty(
        QStringLiteral("liveChangeExplorerModel"),
        model);

    quickWidget_->rootContext()->setContextProperty(
        QStringLiteral("liveChangeExplorerHost"),
        this);

    quickWidget_->setSource(
        QUrl(QStringLiteral("qrc:/qml/LiveChangeExplorer.qml")));

    layout->addWidget(quickWidget_);
}

void LiveChangeExplorerHost::openFrameInfoForRow(int row)
{
    if (row < 0)
    {
        return;
    }

    emit openFrameInfoRequested(row);
}

void LiveChangeExplorerHost::openGraphingForRow(int row)
{
    if (row < 0)
    {
        return;
    }

    emit openGraphingRequested(row);
}

void LiveChangeExplorerHost::createMarkerForRow(int row, const QString &label)
{
    if (row < 0)
    {
        return;
    }

    emit createMarkerRequested(row, label);
}

void LiveChangeExplorerHost::openAnalysisMarkers()
{
    emit openAnalysisMarkersRequested();
}

void LiveChangeExplorerHost::showAnalysisMarkers(
    const QVariantList &markers)
{
    if (quickWidget_ == nullptr || quickWidget_->rootObject() == nullptr)
    {
        return;
    }

    QMetaObject::invokeMethod(
        quickWidget_->rootObject(),
        "openAnalysisMarkersDialog",
        Q_ARG(QVariant, QVariant::fromValue(markers)));
}