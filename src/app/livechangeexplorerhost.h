#ifndef LIVECHANGEEXPLORERHOST_H
#define LIVECHANGEEXPLORERHOST_H

// Qt headers
#include <QString>
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
    void createMarkerForRow(int row, const QString &label);
    void openAnalysisMarkers();

signals:
    void openFrameInfoRequested(int row);
    void openGraphingRequested(int row);
    void createMarkerRequested(int row, const QString &label);
    void openAnalysisMarkersRequested();

private:
    QQuickWidget *quickWidget_ = nullptr;
};

#endif // LIVECHANGEEXPLORERHOST_H
