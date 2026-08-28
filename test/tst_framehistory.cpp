// SavvyLens headers
#include "tst_framehistory.h"
#include "analysis/framehistory.h"
#include "can/can_structs.h"

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

void TestFrameHistory::firstFrameHasLatestButNoPrevious()
{
    FrameHistory history;

    const CANFrame frame = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("1020"), 10, 2);

    history.ingest(frame);

    const FrameAggregateKey key = FrameHistory::makeKey(frame);
    const FrameHistorySnapshot *latest = history.latest(key);

    QVERIFY(latest != nullptr);
    QVERIFY(history.previous(key) == nullptr);
    QCOMPARE(history.keyCount(), static_cast<std::size_t>(1));
    QCOMPARE(history.snapshotCount(key), static_cast<std::size_t>(1));
    QCOMPARE(latest->payload, QByteArray::fromHex("1020"));
    QCOMPARE(latest->sourceTimedelta, static_cast<std::uint64_t>(10));
    QCOMPARE(latest->sourceFrameCount, static_cast<std::uint32_t>(2));
}

void TestFrameHistory::secondFrameExposesLatestAndPrevious()
{
    FrameHistory history;

    const CANFrame first = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("1020"));
    const CANFrame second = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("1025"));

    history.ingest(first);
    history.ingest(second);

    const FrameAggregateKey key = FrameHistory::makeKey(first);
    const FrameHistorySnapshot *latest = history.latest(key);
    const FrameHistorySnapshot *previous = history.previous(key);

    QVERIFY(latest != nullptr);
    QVERIFY(previous != nullptr);
    QCOMPARE(history.snapshotCount(key), static_cast<std::size_t>(2));
    QCOMPARE(previous->payload, QByteArray::fromHex("1020"));
    QCOMPARE(latest->payload, QByteArray::fromHex("1025"));
}

void TestFrameHistory::depthTwoDiscardsOldestSnapshot()
{
    FrameHistory history(2);

    const CANFrame first = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("01"));
    const CANFrame second = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("02"));
    const CANFrame third = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("03"));

    history.ingest(first);
    history.ingest(second);
    history.ingest(third);

    const FrameAggregateKey key = FrameHistory::makeKey(first);
    const FrameHistorySnapshot *latest = history.latest(key);
    const FrameHistorySnapshot *previous = history.previous(key);

    QVERIFY(latest != nullptr);
    QVERIFY(previous != nullptr);
    QCOMPARE(history.snapshotCount(key), static_cast<std::size_t>(2));
    QCOMPARE(previous->payload, QByteArray::fromHex("02"));
    QCOMPARE(latest->payload, QByteArray::fromHex("03"));
}

void TestFrameHistory::aggregateKeyPartsKeepHistoriesIndependent()
{
    FrameHistory history;

    const CANFrame base = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("01"));
    const CANFrame otherBus = makeFrame(
        0x123, 2, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("02"));
    const CANFrame transmitted = makeFrame(
        0x123, 1, false, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("03"));
    const CANFrame extended = makeFrame(
        0x123, 1, true, true, QCanBusFrame::DataFrame,
        QByteArray::fromHex("04"));
    const CANFrame remote = makeFrame(
        0x123, 1, true, false, QCanBusFrame::RemoteRequestFrame,
        QByteArray());

    history.ingest(base);
    history.ingest(otherBus);
    history.ingest(transmitted);
    history.ingest(extended);
    history.ingest(remote);

    QCOMPARE(history.keyCount(), static_cast<std::size_t>(5));

    QVERIFY(history.latest(FrameHistory::makeKey(base)) != nullptr);
    QVERIFY(history.latest(FrameHistory::makeKey(otherBus)) != nullptr);
    QVERIFY(history.latest(FrameHistory::makeKey(transmitted)) != nullptr);
    QVERIFY(history.latest(FrameHistory::makeKey(extended)) != nullptr);
    QVERIFY(history.latest(FrameHistory::makeKey(remote)) != nullptr);
}

void TestFrameHistory::snapshotsOwnCopiedData()
{
    FrameHistory history;

    CANFrame frame = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("1020"), 10, 2);

    history.ingest(frame);

    frame.setPayload(QByteArray::fromHex("FFFF"));
    frame.timedelta = 99;
    frame.frameCount = 88;

    const FrameHistorySnapshot *latest =
        history.latest(FrameHistory::makeKey(frame));

    QVERIFY(latest != nullptr);
    QCOMPARE(latest->payload, QByteArray::fromHex("1020"));
    QCOMPARE(latest->sourceTimedelta, static_cast<std::uint64_t>(10));
    QCOMPARE(latest->sourceFrameCount, static_cast<std::uint32_t>(2));
}

void TestFrameHistory::depthOneHasNoPrevious()
{
    FrameHistory history(1);

    const CANFrame first = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("01"));
    const CANFrame second = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("02"));

    history.ingest(first);
    history.ingest(second);

    const FrameAggregateKey key = FrameHistory::makeKey(first);

    QCOMPARE(history.snapshotCount(key), static_cast<std::size_t>(1));
    QVERIFY(history.previous(key) == nullptr);
    QCOMPARE(history.latest(key)->payload, QByteArray::fromHex("02"));
}

void TestFrameHistory::zeroDepthRetainsNoSnapshots()
{
    FrameHistory history(0);

    const CANFrame frame = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("01"));

    history.ingest(frame);

    const FrameAggregateKey key = FrameHistory::makeKey(frame);

    QVERIFY(history.empty());
    QCOMPARE(history.keyCount(), static_cast<std::size_t>(0));
    QCOMPARE(history.snapshotCount(key), static_cast<std::size_t>(0));
    QVERIFY(history.latest(key) == nullptr);
    QVERIFY(history.previous(key) == nullptr);
}

void TestFrameHistory::clearRemovesAllHistories()
{
    FrameHistory history;

    const CANFrame frame = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("01"));

    history.ingest(frame);

    QVERIFY(!history.empty());
    QCOMPARE(history.keyCount(), static_cast<std::size_t>(1));

    history.clear();

    QVERIFY(history.empty());
    QCOMPARE(history.keyCount(), static_cast<std::size_t>(0));
    QVERIFY(history.latest(FrameHistory::makeKey(frame)) == nullptr);
}
