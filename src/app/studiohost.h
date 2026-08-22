#ifndef STUDIOHOST_H
#define STUDIOHOST_H

// Qt headers
#include <QWidget>

class QCloseEvent;
class QQuickWidget;
class StateExplorerPresentation;

class StudioHost final : public QWidget
{
    Q_OBJECT

public:
    explicit StudioHost(QWidget *parent = nullptr);

public slots:
    void closeStudio();

signals:
    void studioClosed();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void loadStateExplorerDemo();

    QQuickWidget *quickWidget_ = nullptr;
    StateExplorerPresentation *stateExplorerPresentation_ = nullptr;
};

#endif // STUDIOHOST_H
