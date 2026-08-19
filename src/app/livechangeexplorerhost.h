#ifndef LIVECHANGEEXPLORERHOST_H
#define LIVECHANGEEXPLORERHOST_H

#include <QWidget>

class QQuickWidget;

class LiveChangeExplorerModel;

class LiveChangeExplorerHost final : public QWidget
{
    Q_OBJECT

public:
    explicit LiveChangeExplorerHost(
        LiveChangeExplorerModel *model,
        QWidget *parent = nullptr);

    void showAnalysisMarkers(const QVariantList &markers);

public slots:
    void openFrameInfoForRow(int row);
    void openGraphingForRow(int row);
    void createMarkerForRow(int row);
    void openAnalysisMarkers();

signals:
    void openFrameInfoRequested(int row);
    void openGraphingRequested(int row);
    void createMarkerRequested(int row);
    void openAnalysisMarkersRequested();

private:
    QQuickWidget *quickWidget_ = nullptr;
};

#endif // LIVECHANGEEXPLORERHOST_H
