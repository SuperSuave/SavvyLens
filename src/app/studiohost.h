#ifndef STUDIOHOST_H
#define STUDIOHOST_H

#include <QWidget>

class QCloseEvent;
class QQuickWidget;

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
    QQuickWidget *quickWidget_ = nullptr;
};

#endif // STUDIOHOST_H
