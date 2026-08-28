#include <QtTest>
#include <QFile>
#include <QTextStream>
#include <QTemporaryFile>
#include <QElapsedTimer>
#include <QDebug>

#include "src/io/framefileio.h"
#include "tst_framefileio.h"

void TestFrameFileIO::benchmarkLoadVehicleSpyFile()
{
    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());

    QTextStream out(&tempFile);
    out << "LINE,ABS TIME(SEC),REL TIME (SEC),STATUS,ER,TX,DESCRIPTION,NETWORK,NODE,ARB ID,REMOTE,XTD,B1,B2,B3,B4,B5,B6,B7,B8,VALUE,TRIGGER,SIGNALS\n";
    out << "LINE,ABS TIME(SEC),REL TIME (SEC),STATUS,ER,TX,DESCRIPTION,NETWORK,NODE,ARB ID,REMOTE,XTD,B1,B2,B3,B4,B5,B6,B7,B8,VALUE,TRIGGER,SIGNALS\n";

    const int numLines = 100000;
    for (int i = 0; i < numLines; ++i)
    {
        out << i + 2 << ",2550.368293675,0.003818174999651092,67371008,F,F,HS CAN $119,HS CAN,,119,F,T,01,02,03,04,05,06,07,08,,,\n";
    }
    out.flush();
    tempFile.close();

    QVector<CANFrame> frames;
    QElapsedTimer timer;
    timer.start();

    bool result = FrameFileIO::loadVehicleSpyFile(tempFile.fileName(), &frames);
    qint64 elapsedMs = timer.elapsed();

    QVERIFY(result);
    QCOMPARE(frames.size(), numLines);

    // Verify parsed data correctness
    const CANFrame &f0 = frames.first();
    QCOMPARE(f0.frameId(), static_cast<uint32_t>(0x119));
    QCOMPARE(f0.isReceived, true);
    QCOMPARE(f0.hasExtendedFrameFormat(), true);
    QCOMPARE(f0.payload().size(), 8);
    QCOMPARE(static_cast<uint8_t>(f0.payload().at(0)), static_cast<uint8_t>(0x01));
    QCOMPARE(static_cast<uint8_t>(f0.payload().at(7)), static_cast<uint8_t>(0x08));

    qDebug() << "BENCHMARK_RESULT: loadVehicleSpyFile processed" << numLines << "lines in" << elapsedMs << "ms";
}
