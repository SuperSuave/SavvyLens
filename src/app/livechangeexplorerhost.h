#ifndef LIVECHANGEEXPLORERHOST_H
#define LIVECHANGEEXPLORERHOST_H

#include <QWidget>

class LiveChangeExplorerModel;

class LiveChangeExplorerHost final : public QWidget
{
    Q_OBJECT

public:
    explicit LiveChangeExplorerHost(
        LiveChangeExplorerModel *model,
        QWidget *parent = nullptr);
};

#endif // LIVECHANGEEXPLORERHOST_H
