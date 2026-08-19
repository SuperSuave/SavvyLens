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

public slots:
    void openFrameInfoForRow(int row);
    void openGraphingForRow(int row);
    void createMarkerForRow(int row);

signals:
    void openFrameInfoRequested(int row);
    void openGraphingRequested(int row);
    void createMarkerRequested(int row);
};

#endif // LIVECHANGEEXPLORERHOST_H
