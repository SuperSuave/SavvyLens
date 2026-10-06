#include <QtTest>
#include <QFile>
#include <QTextStream>
#include <QTemporaryFile>
#include <QElapsedTimer>
#include <QDebug>
#include <QDir>

#include "io/framefileio.h"
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

namespace {

QString writeTempLog(QTemporaryFile &file, const QByteArray &contents)
{
    file.setFileTemplate(QDir::tempPath() + "/savvylens_candump_XXXXXX.log");
    if (!file.open()) return QString();
    file.write(contents);
    file.close();
    return file.fileName();
}

uint8_t byteAt(const CANFrame &frame, int idx)
{
    return static_cast<uint8_t>(frame.payload().at(idx));
}

} // namespace

void TestFrameFileIO::loadCanDumpClassicFrames()
{
    QTemporaryFile file;
    const QString name = writeTempLog(file,
        "(1551774790.942758) can1 7A8#F4DCD1830E020000\n"
        "(1551774790.950000) vcan0 12345678#0102\n"
        "(1551774790.960000) can0 123#R4\n"
        "(1551774790.970000) can12 456#\n");
    QVERIFY(!name.isEmpty());

    QVector<CANFrame> frames;
    QVERIFY(FrameFileIO::loadCanDumpFile(name, &frames));
    QCOMPARE(frames.size(), 4);

    QCOMPARE(frames[0].frameId(), 0x7A8u);
    QCOMPARE(frames[0].bus, 1);
    QCOMPARE(frames[0].hasExtendedFrameFormat(), false);
    QCOMPARE(frames[0].hasFlexibleDataRateFormat(), false);
    QCOMPARE(frames[0].frameType(), QCanBusFrame::DataFrame);
    QCOMPARE(frames[0].payload(), QByteArray::fromHex("F4DCD1830E020000"));
    QCOMPARE(frames[0].timeStamp().microSeconds(), 1551774790942758ll);

    QCOMPARE(frames[1].frameId(), 0x12345678u);
    QCOMPARE(frames[1].bus, 0);
    QCOMPARE(frames[1].hasExtendedFrameFormat(), true);
    QCOMPARE(frames[1].payload(), QByteArray::fromHex("0102"));

    QCOMPARE(frames[2].frameId(), 0x123u);
    QCOMPARE(frames[2].frameType(), QCanBusFrame::RemoteRequestFrame);
    QCOMPARE(frames[2].payload().size(), 4);

    QCOMPARE(frames[3].frameId(), 0x456u);
    QCOMPARE(frames[3].bus, 12);
    QCOMPARE(frames[3].payload().size(), 0);
}

void TestFrameFileIO::loadCanDumpFdFrames()
{
    // lines taken from a real CAN FD capture made with candump -l
    QTemporaryFile file;
    const QString name = writeTempLog(file,
        "(1791230352.302949) can0 04A##7C5273900000008000580C37FEC7F5E003338323753580000907FD49E00000000\n"
        "(1791230352.306672) can0 06F##7C0288A0000000000\n"
        "(1791230352.307000) can0 18DAF110##1" + QByteArray(64 * 2, 'A') + "\n");
    QVERIFY(!name.isEmpty());

    QVector<CANFrame> frames;
    QVERIFY(FrameFileIO::loadCanDumpFile(name, &frames));
    QCOMPARE(frames.size(), 3);

    const CANFrame &f0 = frames[0];
    QCOMPARE(f0.frameId(), 0x04Au);
    QCOMPARE(f0.hasExtendedFrameFormat(), false);
    QCOMPARE(f0.hasFlexibleDataRateFormat(), true);
    QCOMPARE(f0.hasBitrateSwitch(), true);
    QCOMPARE(f0.hasErrorStateIndicator(), true);
    QCOMPARE(f0.payload().size(), 32);
    QCOMPARE(f0.payload(), QByteArray::fromHex("C5273900000008000580C37FEC7F5E003338323753580000907FD49E00000000"));
    QCOMPARE(byteAt(f0, 0), static_cast<uint8_t>(0xC5));
    QCOMPARE(byteAt(f0, 31), static_cast<uint8_t>(0x00));

    const CANFrame &f1 = frames[1];
    QCOMPARE(f1.frameId(), 0x06Fu);
    QCOMPARE(f1.hasFlexibleDataRateFormat(), true);
    QCOMPARE(f1.payload(), QByteArray::fromHex("C0288A0000000000"));

    const CANFrame &f2 = frames[2];
    QCOMPARE(f2.frameId(), 0x18DAF110u);
    QCOMPARE(f2.hasExtendedFrameFormat(), true);
    QCOMPARE(f2.hasFlexibleDataRateFormat(), true);
    QCOMPARE(f2.hasBitrateSwitch(), true);
    QCOMPARE(f2.hasErrorStateIndicator(), false);
    QCOMPARE(f2.payload().size(), 64);
    QCOMPARE(byteAt(f2, 63), static_cast<uint8_t>(0xAA));
}

void TestFrameFileIO::loadCanDumpFdFlags_data()
{
    QTest::addColumn<QByteArray>("flag");
    QTest::addColumn<bool>("brs");
    QTest::addColumn<bool>("esi");

    QTest::newRow("none") << QByteArray("0") << false << false;
    QTest::newRow("brs") << QByteArray("1") << true << false;
    QTest::newRow("esi") << QByteArray("2") << false << true;
    QTest::newRow("brs+esi") << QByteArray("3") << true << true;
    QTest::newRow("fdf") << QByteArray("4") << false << false;
    QTest::newRow("fdf+brs") << QByteArray("5") << true << false;
    QTest::newRow("fdf+brs+esi") << QByteArray("7") << true << true;
}

void TestFrameFileIO::loadCanDumpFdFlags()
{
    QFETCH(QByteArray, flag);
    QFETCH(bool, brs);
    QFETCH(bool, esi);

    QTemporaryFile file;
    const QString name = writeTempLog(file, "(1.000000) can0 123##" + flag + "0102030405060708090a0b0c\n");
    QVERIFY(!name.isEmpty());

    QVector<CANFrame> frames;
    QVERIFY(FrameFileIO::loadCanDumpFile(name, &frames));
    QCOMPARE(frames.size(), 1);
    QCOMPARE(frames[0].hasFlexibleDataRateFormat(), true);
    QCOMPARE(frames[0].hasBitrateSwitch(), brs);
    QCOMPARE(frames[0].hasErrorStateIndicator(), esi);
    QCOMPARE(frames[0].payload(), QByteArray::fromHex("0102030405060708090A0B0C"));
}

void TestFrameFileIO::loadCanDumpExpandedFormat()
{
    QTemporaryFile file;
    const QString name = writeTempLog(file,
        "(1551774790.942758) can1 7A8 [8] F4 DC D1 83 0E 02 00 00\n"
        "(1551774790.950000) can0 04A [12] 01 02 03 04 05 06 07 08 09 0A 0B 0C\n");
    QVERIFY(!name.isEmpty());

    QVERIFY(FrameFileIO::isCanDumpFile(name));

    QVector<CANFrame> frames;
    QVERIFY(FrameFileIO::loadCanDumpFile(name, &frames));
    QCOMPARE(frames.size(), 2);
    QCOMPARE(frames[0].frameId(), 0x7A8u);
    QCOMPARE(frames[0].bus, 1);
    QCOMPARE(frames[0].hasFlexibleDataRateFormat(), false);
    QCOMPARE(frames[0].payload(), QByteArray::fromHex("F4DCD1830E020000"));
    QCOMPARE(frames[1].frameId(), 0x04Au);
    QCOMPARE(frames[1].hasFlexibleDataRateFormat(), true);
    QCOMPARE(frames[1].payload(), QByteArray::fromHex("0102030405060708090A0B0C"));
}

void TestFrameFileIO::loadCanDumpMixedClassicAndFd()
{
    // FD flags must not leak from one line into the next
    QTemporaryFile file;
    const QString name = writeTempLog(file,
        "(1.000000) can0 100##30102030405060708090A0B0C\n"
        "(1.100000) can0 101#0102\n"
        "(1.200000) can0 102##00102\n");
    QVERIFY(!name.isEmpty());

    QVector<CANFrame> frames;
    QVERIFY(FrameFileIO::loadCanDumpFile(name, &frames));
    QCOMPARE(frames.size(), 3);
    QCOMPARE(frames[0].hasFlexibleDataRateFormat(), true);
    QCOMPARE(frames[0].hasBitrateSwitch(), true);
    QCOMPARE(frames[0].hasErrorStateIndicator(), true);
    QCOMPARE(frames[1].hasFlexibleDataRateFormat(), false);
    QCOMPARE(frames[1].hasBitrateSwitch(), false);
    QCOMPARE(frames[1].hasErrorStateIndicator(), false);
    QCOMPARE(frames[2].hasFlexibleDataRateFormat(), true);
    QCOMPARE(frames[2].hasBitrateSwitch(), false);
    QCOMPARE(frames[2].payload(), QByteArray::fromHex("0102"));
}

void TestFrameFileIO::loadCanDumpSkipsMalformedLines()
{
    QTemporaryFile file;
    const QString name = writeTempLog(file,
        "(1.000000) can0 100#0102\n"
        "\n"
        "(1.100000) can0 XYZ#0102\n"                    // bad ID
        "(1.200000) can0 101#010\n"                     // odd number of hex digits
        "(1.300000) can0 102#010203040506070809\n"      // classic frame longer than 8 bytes
        "(1.400000) can0 103##1010203040506070809\n"    // 9 bytes is not a valid FD length
        "(1.500000) can0 104##G01\n"                    // bad flags nibble
        "(1.600000) can0 105#0G\n"                      // bad hex
        "(1.700000) can0 106#0304\n");
    QVERIFY(!name.isEmpty());

    QVector<CANFrame> frames;
    QVERIFY(FrameFileIO::loadCanDumpFile(name, &frames));
    QCOMPARE(frames.size(), 2);
    QCOMPARE(frames[0].frameId(), 0x100u);
    QCOMPARE(frames[1].frameId(), 0x106u);
    QCOMPARE(frames[1].payload(), QByteArray::fromHex("0304"));
}

void TestFrameFileIO::detectCanDumpFile_data()
{
    QTest::addColumn<QByteArray>("contents");
    QTest::addColumn<bool>("expected");

    QTest::newRow("classic") << QByteArray("(1.000000) can0 123#0102030405060708\n") << true;
    QTest::newRow("fd 32 bytes") << QByteArray("(1791230352.302949) can0 04A##7C5273900000008000580C37FEC7F5E003338323753580000907FD49E00000000\n") << true;
    QTest::newRow("fd 64 bytes") << QByteArray("(1.000000) can0 123##1" + QByteArray(128, '0') + "\n") << true;
    QTest::newRow("classic over 8 bytes") << QByteArray("(1.000000) can0 123#010203040506070809\n") << false;
    QTest::newRow("invalid fd length") << QByteArray("(1.000000) can0 123##1010203040506070809\n") << false;
    QTest::newRow("no timestamp") << QByteArray("can0 123#0102\n") << false;
    QTest::newRow("empty") << QByteArray("") << false;
}

void TestFrameFileIO::detectCanDumpFile()
{
    QFETCH(QByteArray, contents);
    QFETCH(bool, expected);

    QTemporaryFile file;
    const QString name = writeTempLog(file, contents);
    QVERIFY(!name.isEmpty());
    QCOMPARE(FrameFileIO::isCanDumpFile(name), expected);
}

void TestFrameFileIO::saveCanDumpRoundTrip()
{
    QVector<CANFrame> original;

    CANFrame classic;
    classic.setFrameId(0x123);
    classic.setPayload(QByteArray::fromHex("0102030405060708"));
    classic.setTimeStamp(QCanBusFrame::TimeStamp(0, 1000000));
    original.append(classic);

    CANFrame fd;
    fd.setFrameId(0x04A);
    fd.setPayload(QByteArray::fromHex("C5273900000008000580C37FEC7F5E003338323753580000907FD49E00000000"));
    fd.setFlexibleDataRateFormat(true);
    fd.setBitrateSwitch(true);
    fd.setErrorStateIndicator(false);
    fd.setTimeStamp(QCanBusFrame::TimeStamp(0, 2000000));
    original.append(fd);

    CANFrame fdExt;
    fdExt.setFrameId(0x18DAF110);
    fdExt.setExtendedFrameFormat(true);
    fdExt.setPayload(QByteArray::fromHex("0102"));
    fdExt.setFlexibleDataRateFormat(true);
    fdExt.setErrorStateIndicator(true);
    fdExt.setTimeStamp(QCanBusFrame::TimeStamp(0, 3000000));
    original.append(fdExt);

    QTemporaryFile file;
    file.setFileTemplate(QDir::tempPath() + "/savvylens_candump_XXXXXX.log");
    QVERIFY(file.open());
    const QString name = file.fileName();
    file.close();

    QVERIFY(FrameFileIO::saveCanDumpFile(name, &original));

    QFile saved(name);
    QVERIFY(saved.open(QIODevice::ReadOnly | QIODevice::Text));
    const QList<QByteArray> lines = saved.readAll().split('\n');
    QVERIFY(lines.size() >= 3);
    QVERIFY(lines[0].endsWith(" 123#0102030405060708"));
    QVERIFY(lines[1].endsWith(" 04A##5C5273900000008000580C37FEC7F5E003338323753580000907FD49E00000000"));
    QVERIFY(lines[2].endsWith(" 18DAF110##60102"));

    QVector<CANFrame> loaded;
    QVERIFY(FrameFileIO::isCanDumpFile(name));
    QVERIFY(FrameFileIO::loadCanDumpFile(name, &loaded));
    QCOMPARE(loaded.size(), original.size());
    for (int i = 0; i < original.size(); i++)
    {
        QCOMPARE(loaded[i].frameId(), original[i].frameId());
        QCOMPARE(loaded[i].hasExtendedFrameFormat(), original[i].hasExtendedFrameFormat());
        QCOMPARE(loaded[i].hasFlexibleDataRateFormat(), original[i].hasFlexibleDataRateFormat());
        QCOMPARE(loaded[i].hasBitrateSwitch(), original[i].hasBitrateSwitch());
        QCOMPARE(loaded[i].hasErrorStateIndicator(), original[i].hasErrorStateIndicator());
        QCOMPARE(loaded[i].payload(), original[i].payload());
        QCOMPARE(loaded[i].timeStamp().microSeconds(), original[i].timeStamp().microSeconds());
    }
}
