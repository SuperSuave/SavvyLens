#include "tst_rangestatistics.h"
#include "analysis/rangestatistics.h"
#include "can/can_structs.h"

#include <QtTest>
#include <cmath>

namespace
{
CANFrame makeFrame(quint32 frameId, const QByteArray &payload)
{
    CANFrame frame;
    frame.setFrameId(frameId);
    frame.bus = 0;
    frame.isReceived = true;
    frame.setPayload(payload);
    return frame;
}
} // namespace

void TestRangeStatistics::initTestCase()
{
}

void TestRangeStatistics::cleanupTestCase()
{
}

void TestRangeStatistics::byteStatsOnConstantPayload()
{
    QVector<CANFrame> frames;
    QByteArray staticPayload = QByteArray::fromHex("11223344");
    for (int i = 0; i < 20; ++i) {
        frames.append(makeFrame(0x100, staticPayload));
    }

    ByteRangeStats b0 = RangeStatistics::computeByteStats(frames, 0, 0x100);
    QCOMPARE(b0.byteIndex, 0);
    QCOMPARE(b0.minUnsigned, static_cast<quint8>(0x11));
    QCOMPARE(b0.maxUnsigned, static_cast<quint8>(0x11));
    QCOMPARE(b0.minSigned, static_cast<qint8>(0x11));
    QCOMPARE(b0.maxSigned, static_cast<qint8>(0x11));
    QCOMPARE(b0.uniqueValues, 1);
    QCOMPARE(b0.isConstant, true);
    QCOMPARE(b0.bitChangeMask, static_cast<quint8>(0x00));
    QCOMPARE(b0.sampleCount, 20);

    QVector<ByteRangeStats> all = RangeStatistics::computeAllByteStats(frames, 0x100);
    QCOMPARE(all.size(), 4);
    QCOMPARE(all.at(1).minUnsigned, static_cast<quint8>(0x22));
    QCOMPARE(all.at(2).minUnsigned, static_cast<quint8>(0x33));
    QCOMPARE(all.at(3).minUnsigned, static_cast<quint8>(0x44));
}

void TestRangeStatistics::byteStatsOnRampingPayload()
{
    QVector<CANFrame> frames;
    for (int i = 0; i < 100; ++i) {
        QByteArray payload;
        payload.append(static_cast<char>(i));
        payload.append(static_cast<char>(0x55));
        frames.append(makeFrame(0x150, payload));
    }

    ByteRangeStats b0 = RangeStatistics::computeByteStats(frames, 0, 0x150);
    QCOMPARE(b0.minUnsigned, static_cast<quint8>(0));
    QCOMPARE(b0.maxUnsigned, static_cast<quint8>(99));
    QCOMPARE(b0.uniqueValues, 100);
    QCOMPARE(b0.isConstant, false);
    QVERIFY(b0.bitChangeMask != 0);
    QCOMPARE(b0.unsignedSpan(), static_cast<quint8>(99));
}

void TestRangeStatistics::multiByteEndiannessExtraction()
{
    // Intel 16-bit: 0x1234 -> LSB first: 0x34, 0x12
    QByteArray intelPayload = QByteArray::fromHex("3412");
    RangeSignalSpec intelSpec;
    intelSpec.startBit = 0;
    intelSpec.bitLength = 16;
    intelSpec.isLittleEndian = true;
    intelSpec.isSigned = false;
    qint64 valIntel = RangeStatistics::extractValue(intelPayload, intelSpec);
    QCOMPARE(valIntel, static_cast<qint64>(0x1234));

    // Motorola 16-bit: 0x1234 -> MSB first: 0x12, 0x34 (DBC startBit 7 is MSB of byte 0)
    QByteArray motoPayload = QByteArray::fromHex("1234");
    RangeSignalSpec motoSpec;
    motoSpec.startBit = 7;
    motoSpec.bitLength = 16;
    motoSpec.isLittleEndian = false;
    motoSpec.isSigned = false;
    qint64 valMoto = RangeStatistics::extractValue(motoPayload, motoSpec);
    QCOMPARE(valMoto, static_cast<qint64>(0x1234));
}

void TestRangeStatistics::signedSignalNegativeRange()
{
    QByteArray p1 = QByteArray::fromHex("FF");
    RangeSignalSpec s8;
    s8.startBit = 0;
    s8.bitLength = 8;
    s8.isLittleEndian = true;
    s8.isSigned = true;
    QCOMPARE(RangeStatistics::extractValue(p1, s8), static_cast<qint64>(-1));

    QByteArray p2 = QByteArray::fromHex("80");
    QCOMPARE(RangeStatistics::extractValue(p2, s8), static_cast<qint64>(-128));

    QByteArray p3 = QByteArray::fromHex("7F");
    QCOMPARE(RangeStatistics::extractValue(p3, s8), static_cast<qint64>(127));

    // 16-bit signed -100 = 0xFF9C (little endian: 0x9C, 0xFF)
    QByteArray p16 = QByteArray::fromHex("9CFF");
    RangeSignalSpec s16;
    s16.startBit = 0;
    s16.bitLength = 16;
    s16.isLittleEndian = true;
    s16.isSigned = true;
    QCOMPARE(RangeStatistics::extractValue(p16, s16), static_cast<qint64>(-100));
}

void TestRangeStatistics::evaluateLinearRampSignal()
{
    QVector<CANFrame> frames;
    RangeSignalSpec spec;
    spec.canId = 0x200;
    spec.startBit = 0;
    spec.bitLength = 16;
    spec.isLittleEndian = true;
    spec.isSigned = false;

    for (int i = 0; i < 100; ++i) {
        quint16 val = static_cast<quint16>(1000 + i * 10);
        QByteArray payload;
        payload.append(static_cast<char>(val & 0xFF));
        payload.append(static_cast<char>((val >> 8) & 0xFF));
        frames.append(makeFrame(0x200, payload));
    }

    RangeSignalCandidate candidate = RangeStatistics::evaluateSignal(frames, spec, 128, true);
    QCOMPARE(candidate.isRanging, true);
    QCOMPARE(candidate.minValue, static_cast<qint64>(1000));
    QCOMPARE(candidate.maxValue, static_cast<qint64>(1990));
    QCOMPARE(candidate.rangeSpan, static_cast<qint64>(990));
    QCOMPARE(candidate.firstOrderSpikes, 0);
    QCOMPARE(candidate.secondOrderSpikes, 0);
    QVERIFY(candidate.smoothnessScore >= 0.95);
    QCOMPARE(candidate.sampleValues.size(), 100);
}

void TestRangeStatistics::evaluateSineWaveSignal()
{
    QVector<CANFrame> frames;
    RangeSignalSpec spec;
    spec.canId = 0x300;
    spec.startBit = 0;
    spec.bitLength = 16;
    spec.isLittleEndian = true;
    spec.isSigned = false;

    const double pi = 3.14159265358979323846;
    for (int i = 0; i < 120; ++i) {
        double angle = (2.0 * pi * i) / 60.0;
        quint16 val = static_cast<quint16>(500.0 + 400.0 * std::sin(angle));
        QByteArray payload;
        payload.append(static_cast<char>(val & 0xFF));
        payload.append(static_cast<char>((val >> 8) & 0xFF));
        frames.append(makeFrame(0x300, payload));
    }

    RangeSignalCandidate candidate = RangeStatistics::evaluateSignal(frames, spec, 128);
    QCOMPARE(candidate.isRanging, true);
    QVERIFY(candidate.smoothnessScore >= 0.85);
    QVERIFY(candidate.rangeSpan >= 700);
}

void TestRangeStatistics::rejectStaticSignal()
{
    QVector<CANFrame> frames;
    RangeSignalSpec spec;
    spec.canId = 0x400;
    spec.startBit = 0;
    spec.bitLength = 8;
    spec.isLittleEndian = true;
    spec.isSigned = false;

    for (int i = 0; i < 50; ++i) {
        frames.append(makeFrame(0x400, QByteArray::fromHex("55")));
    }

    RangeSignalCandidate candidate = RangeStatistics::evaluateSignal(frames, spec);
    QCOMPARE(candidate.isRanging, false);
    QCOMPARE(candidate.rangeSpan, static_cast<qint64>(0));
}

void TestRangeStatistics::rejectRandomNoiseSignal()
{
    QVector<CANFrame> frames;
    RangeSignalSpec spec;
    spec.canId = 0x500;
    spec.startBit = 0;
    spec.bitLength = 16;
    spec.isLittleEndian = true;
    spec.isSigned = false;

    // Erratic jumpy values (simulating hash or high noise)
    quint16 noiseValues[] = {
        100, 50000, 200, 60000, 50, 45000, 300, 55000,
        150, 48000, 80, 52000, 400, 62000, 120, 43000,
        250, 59000, 90, 51000, 350, 58000, 110, 47000,
        180, 61000, 70, 46000, 290, 53000, 160, 49000
    };

    for (quint16 val : noiseValues) {
        QByteArray payload;
        payload.append(static_cast<char>(val & 0xFF));
        payload.append(static_cast<char>((val >> 8) & 0xFF));
        frames.append(makeFrame(0x500, payload));
    }

    RangeSignalCandidate candidate = RangeStatistics::evaluateSignal(frames, spec, 128);
    QCOMPARE(candidate.isRanging, false);
    QVERIFY(candidate.firstOrderSpikes > 10);
    QVERIFY(candidate.smoothnessScore < 0.5);
}

void TestRangeStatistics::candidateScannerFindsRamp()
{
    QVector<CANFrame> frames;
    for (int i = 0; i < 100; ++i) {
        QByteArray payload(8, 0);
        // Byte 0-1: constant
        payload[0] = static_cast<char>(0xAA);
        payload[1] = static_cast<char>(0x55);

        // Byte 2-3: 16-bit little endian ramp (500 to 1490)
        quint16 ramp = static_cast<quint16>(500 + i * 10);
        payload[2] = static_cast<char>(ramp & 0xFF);
        payload[3] = static_cast<char>((ramp >> 8) & 0xFF);

        // Byte 4-7: static
        payload[4] = 0x10;
        payload[5] = 0x20;
        payload[6] = 0x30;
        payload[7] = 0x40;

        frames.append(makeFrame(0x600, payload));
    }

    RangeScanConfig config;
    config.minBitLength = 16;
    config.maxBitLength = 16;
    config.bitGranularity = 8;
    config.endianMode = RangeScanConfig::LittleEndianOnly;
    config.signedMode = RangeScanConfig::UnsignedOnly;

    QVector<RangeSignalCandidate> candidates = RangeStatistics::scanCandidates(frames, 0x600, config);
    QVERIFY(!candidates.isEmpty());

    bool foundRamp = false;
    for (const RangeSignalCandidate &c : candidates) {
        if (c.spec.startBit == 16 && c.spec.bitLength == 16 && c.spec.isLittleEndian) {
            foundRamp = true;
            QCOMPARE(c.minValue, static_cast<qint64>(500));
            QCOMPARE(c.maxValue, static_cast<qint64>(1490));
            QCOMPARE(c.isRanging, true);
        }
    }
    QVERIFY(foundRamp);
}

void TestRangeStatistics::candidateScannerCancellation()
{
    QVector<CANFrame> frames;
    for (int i = 0; i < 50; ++i) {
        frames.append(makeFrame(0x700, QByteArray::fromHex("0102030405060708")));
    }

    RangeScanConfig config;
    config.minBitLength = 8;
    config.maxBitLength = 64;
    config.bitGranularity = 1;

    int stepsRecorded = 0;
    auto cancelEarly = [&stepsRecorded](int current, int total) -> bool {
        Q_UNUSED(total);
        stepsRecorded = current;
        return current < 5; // Cancel after 5 steps
    };

    QVector<RangeSignalCandidate> candidates = RangeStatistics::scanCandidates(frames, 0x700, config, cancelEarly);
    QCOMPARE(stepsRecorded, 5);
}
void TestRangeStatistics::canIdZeroIsAnExactFilter()
{
    QVector<CANFrame> frames;

    frames.append(makeFrame(0x000, QByteArray::fromHex("01")));
    frames.append(makeFrame(0x000, QByteArray::fromHex("02")));
    frames.append(makeFrame(0x123, QByteArray::fromHex("AA")));
    frames.append(makeFrame(0x123, QByteArray::fromHex("BB")));

    RangeSignalSpec spec;
    spec.canId = 0x000;
    spec.startBit = 0;
    spec.bitLength = 8;
    spec.isLittleEndian = true;
    spec.isSigned = false;

    const QVector<qint64> values =
        RangeStatistics::extractSignalValues(frames, spec);

    QCOMPARE(values.size(), 2);
    QCOMPARE(values.at(0), static_cast<qint64>(0x01));
    QCOMPARE(values.at(1), static_cast<qint64>(0x02));

    const ByteRangeStats stats =
        RangeStatistics::computeByteStats(frames, 0, 0x000);

    QCOMPARE(stats.sampleCount, 2);
    QCOMPARE(stats.minUnsigned, static_cast<quint8>(0x01));
    QCOMPARE(stats.maxUnsigned, static_cast<quint8>(0x02));
}

void TestRangeStatistics::shortPayloadsAreSkipped()
{
    QVector<CANFrame> frames;

    frames.append(makeFrame(0x321, QByteArray::fromHex("3412")));
    frames.append(makeFrame(0x321, QByteArray::fromHex("7856")));
    frames.append(makeFrame(0x321, QByteArray::fromHex("9A")));

    RangeSignalSpec spec;
    spec.canId = 0x321;
    spec.startBit = 0;
    spec.bitLength = 16;
    spec.isLittleEndian = true;
    spec.isSigned = false;

    const QVector<qint64> values =
        RangeStatistics::extractSignalValues(frames, spec);

    QCOMPARE(values.size(), 2);
    QCOMPARE(values.at(0), static_cast<qint64>(0x1234));
    QCOMPARE(values.at(1), static_cast<qint64>(0x5678));

    const RangeSignalCandidate candidate =
        RangeStatistics::evaluateSignal(frames, spec);

    QCOMPARE(candidate.sampleCount, 2);
    QCOMPARE(candidate.minValue, static_cast<qint64>(0x1234));
    QCOMPARE(candidate.maxValue, static_cast<qint64>(0x5678));
}

void TestRangeStatistics::candidateScannerUsesMinimumPayloadLength()
{
    QVector<CANFrame> frames;

    for (int i = 0; i < 20; ++i)
    {
        QByteArray longPayload(8, 0);
        longPayload[0] = static_cast<char>(i * 10);
        longPayload[1] = static_cast<char>(i * 10);

        frames.append(makeFrame(0x322, longPayload));
        frames.append(makeFrame(
            0x322,
            QByteArray(1, static_cast<char>(i * 10))));
    }

    RangeScanConfig config;
    config.minBitLength = 16;
    config.maxBitLength = 16;
    config.bitGranularity = 8;
    config.endianMode = RangeScanConfig::LittleEndianOnly;
    config.signedMode = RangeScanConfig::UnsignedOnly;

    const QVector<RangeSignalCandidate> candidates =
        RangeStatistics::scanCandidates(frames, 0x322, config);

    QCOMPARE(candidates.size(), 0);
}

void TestRangeStatistics::payloadSupportRejectsInvalidMotorolaLayout()
{
    RangeSignalSpec spec;
    spec.canId = 0x323;
    spec.startBit = 0;
    spec.bitLength = 16;
    spec.isLittleEndian = false;
    spec.isSigned = false;

    const RangeSignalPayloadSupport support =
        RangeStatistics::payloadSupport(1, spec);

    QVERIFY(!support.isSupported);
}
