#include "livechangeexplorerhost.h"

#include <QQuickWidget>
#include <QUrl>
#include <QVBoxLayout>

LiveChangeExplorerHost::LiveChangeExplorerHost(QWidget *parent)
    : QWidget(parent, Qt::Window)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    auto *quickWidget = new QQuickWidget(this);
    quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    quickWidget->setSource(
        QUrl(QStringLiteral("qrc:/qml/LiveChangeExplorer.qml")));

    layout->addWidget(quickWidget);
}
