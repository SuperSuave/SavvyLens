#include "livechangeexplorerhost.h"

// SavvyLens headers
#include "analysis/livechangeexplorermodel.h"

// Qt headers
#include <QQuickWidget>
#include <QQmlContext>
#include <QUrl>
#include <QVBoxLayout>

LiveChangeExplorerHost::LiveChangeExplorerHost(
    LiveChangeExplorerModel *model,
    QWidget *parent)
    : QWidget(parent, Qt::Window)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    auto *quickWidget = new QQuickWidget(this);
    quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

    quickWidget->rootContext()->setContextProperty(
        QStringLiteral("liveChangeExplorerModel"),
        model);

    quickWidget->setSource(
        QUrl(QStringLiteral("qrc:/qml/LiveChangeExplorer.qml")));

    layout->addWidget(quickWidget);
}
