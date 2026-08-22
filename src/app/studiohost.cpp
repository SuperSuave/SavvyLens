#include "studiohost.h"

// Qt headers
#include <QCloseEvent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWidget>
#include <QUrl>
#include <QVBoxLayout>

StudioHost::StudioHost(QWidget *parent)
    : QWidget(parent, Qt::Window)
{
    setWindowTitle(tr("SavvyLens Studio"));
    resize(1800, 1075);
    setMinimumSize(1280, 760);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    quickWidget_ = new QQuickWidget(this);
    quickWidget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    quickWidget_->engine()->addImportPath(QStringLiteral("qrc:/qml"));

    quickWidget_->rootContext()->setContextProperty(
        QStringLiteral("studioHost"),
        this);

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