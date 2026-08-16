#ifndef LIVECHANGEEXPLORERHOST_H
#define LIVECHANGEEXPLORERHOST_H

#include <QWidget>

class LiveChangeExplorerHost final : public QWidget
{
    Q_OBJECT

public:
    explicit LiveChangeExplorerHost(QWidget *parent = nullptr);
};

#endif // LIVECHANGEEXPLORERHOST_H
