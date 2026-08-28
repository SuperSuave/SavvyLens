#include "tst_signalresolver.h"
#include "src/can/can_structs.h"
#include "src/signals/signalresolver.h"

// Qt headers
#include <QtTest>

using namespace Signals;

void TestSignalResolver::decode8BitUnsignedIntel()
{
    CANFrame frame;
    QByteArray payload;
    payload.append(static_cast<char>(0x55)); // 85 decimal
    frame.setPayload(payload);

    SignalDefinition sig;
    sig.startBit = 0;
    sig.bitLength = 8;
    sig.byteOrder = ByteOrder::LittleEndian;
    sig.valueType = ValueType::Unsigned;
    sig.scale = 1.0;
    sig.offset = 0.0;

    ResolveResult res = SignalResolver::decodeSignal(frame, sig);
    QVERIFY(res.valid);
    QCOMPARE(res.rawValue, 85.0);
    QCOMPARE(res.physicalValue, 85.0);
}

void TestSignalResolver::decode16BitSignedLittleEndian()
{
    CANFrame frame;
    QByteArray payload;
    // -400 in 16-bit 2's complement = 0xFE70 (Little Endian bytes: 0x70, 0xFE)
    payload.append(static_cast<char>(0x70));
    payload.append(static_cast<char>(0xFE));
    frame.setPayload(payload);

    SignalDefinition sig;
    sig.startBit = 0;
    sig.bitLength = 16;
    sig.byteOrder = ByteOrder::LittleEndian;
    sig.valueType = ValueType::Signed;
    sig.scale = 0.1;
    sig.offset = 0.0;

    ResolveResult res = SignalResolver::decodeSignal(frame, sig);
    QVERIFY(res.valid);
    QCOMPARE(res.rawValue, -400.0);
    QCOMPARE(res.physicalValue, -40.0);
}

void TestSignalResolver::decodeMotorolaBigEndianSignal()
{
    CANFrame frame;
    QByteArray payload;
    payload.append(static_cast<char>(0x12));
    payload.append(static_cast<char>(0x34));
    frame.setPayload(payload);

    SignalDefinition sig;
    sig.startBit = 7; // Motorola start bit MSB in byte 0
    sig.bitLength = 16;
    sig.byteOrder = ByteOrder::BigEndian;
    sig.valueType = ValueType::Unsigned;

    ResolveResult res = SignalResolver::decodeSignal(frame, sig);
    QVERIFY(res.valid);
    QCOMPARE(res.rawValue, static_cast<double>(0x1234));
}

void TestSignalResolver::rejectShortFrame()
{
    CANFrame frame;
    QByteArray payload;
    payload.append(static_cast<char>(0x12)); // Only 1 byte
    frame.setPayload(payload);

    SignalDefinition sig;
    sig.startBit = 0;
    sig.bitLength = 16; // Requires 2 bytes

    ResolveResult res = SignalResolver::decodeSignal(frame, sig);
    QVERIFY(!res.valid);
}
