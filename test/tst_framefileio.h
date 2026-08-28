#ifndef TST_FRAMEFILEIO_H
#define TST_FRAMEFILEIO_H

#include <QObject>

class TestFrameFileIO: public QObject
{
    Q_OBJECT

private slots:
    void benchmarkLoadVehicleSpyFile();
};

#endif // TST_FRAMEFILEIO_H
