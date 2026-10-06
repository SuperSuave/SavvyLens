#ifndef TST_FRAMEFILEIO_H
#define TST_FRAMEFILEIO_H

#include <QObject>

class TestFrameFileIO: public QObject
{
    Q_OBJECT

private slots:
    void benchmarkLoadVehicleSpyFile();
    void loadCanDumpClassicFrames();
    void loadCanDumpFdFrames();
    void loadCanDumpFdFlags_data();
    void loadCanDumpFdFlags();
    void loadCanDumpExpandedFormat();
    void loadCanDumpMixedClassicAndFd();
    void loadCanDumpSkipsMalformedLines();
    void detectCanDumpFile_data();
    void detectCanDumpFile();
    void saveCanDumpRoundTrip();
};

#endif // TST_FRAMEFILEIO_H
