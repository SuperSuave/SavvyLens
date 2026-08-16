// SavvyLens headers
#include "analysis/frameaggregatestore.h"
#include "can/can_structs.h"
#include "tst_frameaggregatestore.h"

// Qt headers
#include <QtTest>

namespace
{

    CANFrame makeFrame(quint32 frameId,
                       int bus,
                       bool isReceived,
                       bool isExtended,
                       QCanBusFrame::FrameType frameType,
                       const QByteArray &payload,
                       std::uint64_t timedelta = 0,
                       std::uint32_t frameCount = 1)
    {
        CANFrame frame;

        frame.setFrameId(frameId);
        frame.bus = bus;
        frame.isReceived = isReceived;
        frame.setExtendedFrameFormat(isExtended);
        frame.setFrameType(frameType);
        frame.setPayload(payload);
        frame.timedelta = timedelta;
        frame.frameCount = frameCount;

        return frame;
    }

}

void TestFrameAggregateStore::sameKeyIncrementsOccurrenceCount()
{
    FrameAggregateStore store;

    const CANFrame first = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame, QByteArray::fromHex("01"));
    const CANFrame second = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame, QByteArray::fromHex("02"));

    store.ingest(first);
    store.ingest(second);

    const FrameAggregateKey key = FrameAggregateStore::makeKey(first);
    const FrameAggregate *aggregate = store.find(key);

    QVERIFY(aggregate != nullptr);
    QCOMPARE(store.size(), static_cast<std::size_t>(1));
    QCOMPARE(aggregate->occurrenceCount, static_cast<std::uint64_t>(2));
}

void TestFrameAggregateStore::sameIdOnDifferentBusesCreatesSeparateAggregates()
{
    FrameAggregateStore store;

    const CANFrame busOne = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame, QByteArray::fromHex("01"));
    const CANFrame busTwo = makeFrame(
        0x123, 2, true, false, QCanBusFrame::DataFrame, QByteArray::fromHex("01"));

    store.ingest(busOne);
    store.ingest(busTwo);

    QVERIFY(store.find(FrameAggregateStore::makeKey(busOne)) != nullptr);
    QVERIFY(store.find(FrameAggregateStore::makeKey(busTwo)) != nullptr);
    QCOMPARE(store.size(), static_cast<std::size_t>(2));
}

void TestFrameAggregateStore::receiveAndTransmitCreateSeparateAggregates()
{
    FrameAggregateStore store;

    const CANFrame received = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame, QByteArray::fromHex("01"));
    const CANFrame transmitted = makeFrame(
        0x123, 1, false, false, QCanBusFrame::DataFrame, QByteArray::fromHex("01"));

    store.ingest(received);
    store.ingest(transmitted);

    QVERIFY(store.find(FrameAggregateStore::makeKey(received)) != nullptr);
    QVERIFY(store.find(FrameAggregateStore::makeKey(transmitted)) != nullptr);
    QCOMPARE(store.size(), static_cast<std::size_t>(2));
}

void TestFrameAggregateStore::standardAndExtendedCreateSeparateAggregates()
{
    FrameAggregateStore store;

    const CANFrame standard = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame, QByteArray::fromHex("01"));
    const CANFrame extended = makeFrame(
        0x123, 1, true, true, QCanBusFrame::DataFrame, QByteArray::fromHex("01"));

    store.ingest(standard);
    store.ingest(extended);

    QVERIFY(store.find(FrameAggregateStore::makeKey(standard)) != nullptr);
    QVERIFY(store.find(FrameAggregateStore::makeKey(extended)) != nullptr);
    QCOMPARE(store.size(), static_cast<std::size_t>(2));
}

void TestFrameAggregateStore::differentFrameTypesCreateSeparateAggregates()
{
    FrameAggregateStore store;

    const CANFrame data = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame, QByteArray::fromHex("01"));
    const CANFrame remote = makeFrame(
        0x123, 1, true, false, QCanBusFrame::RemoteRequestFrame, QByteArray());

    store.ingest(data);
    store.ingest(remote);

    QVERIFY(store.find(FrameAggregateStore::makeKey(data)) != nullptr);
    QVERIFY(store.find(FrameAggregateStore::makeKey(remote)) != nullptr);
    QCOMPARE(store.size(), static_cast<std::size_t>(2));
}

void TestFrameAggregateStore::lastIngestedCopiesPayloadAndMetadata()
{
    FrameAggregateStore store;

    CANFrame first = makeFrame(
        0x123,
        1,
        true,
        false,
        QCanBusFrame::DataFrame,
        QByteArray::fromHex("0102"),
        25,
        3);

    CANFrame second = makeFrame(
        0x123,
        1,
        true,
        false,
        QCanBusFrame::DataFrame,
        QByteArray::fromHex("AABBCC"),
        50,
        7);

    store.ingest(first);
    store.ingest(second);

    second.setPayload(QByteArray::fromHex("FF"));
    second.timedelta = 100;
    second.frameCount = 99;

    const FrameAggregate *aggregate =
        store.find(FrameAggregateStore::makeKey(first));

    QVERIFY(aggregate != nullptr);
    QCOMPARE(aggregate->occurrenceCount, static_cast<std::uint64_t>(2));
    QCOMPARE(aggregate->lastIngested.payload, QByteArray::fromHex("AABBCC"));
    QCOMPARE(aggregate->lastIngested.sourceTimedelta, static_cast<std::uint64_t>(50));
    QCOMPARE(aggregate->lastIngested.sourceFrameCount, static_cast<std::uint32_t>(7));
}

void TestFrameAggregateStore::clearRemovesAllAggregates()
{
    FrameAggregateStore store;

    const CANFrame frame = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame, QByteArray::fromHex("01"));

    store.ingest(frame);

    QVERIFY(!store.empty());
    QCOMPARE(store.size(), static_cast<std::size_t>(1));

    store.clear();

    QVERIFY(store.empty());
    QCOMPARE(store.size(), static_cast<std::size_t>(0));
    QVERIFY(store.find(FrameAggregateStore::makeKey(frame)) == nullptr);
}
