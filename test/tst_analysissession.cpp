// SavvyLens headers
#include "tst_analysissession.h"
#include "analysis/analysissession.h"
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

void TestAnalysisSession::firstFrameCreatesAggregateAndLatestSnapshot()
{
    AnalysisSession session;

    const CANFrame frame = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("1020"), 25, 3);

    session.ingest(frame);

    const FrameAggregateKey key = AnalysisSession::makeKey(frame);
    const FrameAggregate *aggregate = session.findAggregate(key);
    const FrameHistorySnapshot *latest = session.latestSnapshot(key);

    QVERIFY(!session.empty());
    QCOMPARE(session.aggregateCount(), static_cast<std::size_t>(1));
    QVERIFY(aggregate != nullptr);
    QVERIFY(latest != nullptr);
    QVERIFY(session.previousSnapshot(key) == nullptr);

    QCOMPARE(aggregate->occurrenceCount, static_cast<std::uint64_t>(1));
    QCOMPARE(latest->payload, QByteArray::fromHex("1020"));
    QCOMPARE(latest->sourceTimedelta, static_cast<std::uint64_t>(25));
    QCOMPARE(latest->sourceFrameCount, static_cast<std::uint32_t>(3));
}

void TestAnalysisSession::secondSameKeyFrameCreatesComparison()
{
    AnalysisSession session;

    const CANFrame first = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("1020"));
    const CANFrame second = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("1025"));

    session.ingest(first);
    session.ingest(second);

    const FrameAggregateKey key = AnalysisSession::makeKey(first);
    const FrameAggregate *aggregate = session.findAggregate(key);
    FrameComparison comparison;

    QVERIFY(aggregate != nullptr);
    QCOMPARE(aggregate->occurrenceCount, static_cast<std::uint64_t>(2));
    QVERIFY(session.compareLatest(key, &comparison));
    QCOMPARE(comparison.previous.payload, QByteArray::fromHex("1020"));
    QCOMPARE(comparison.current.payload, QByteArray::fromHex("1025"));
    QCOMPARE(comparison.payloadDiff.changedByteMask,
             QByteArray::fromHex("00FF"));
    QCOMPARE(comparison.payloadDiff.changedBitMask,
             QByteArray::fromHex("0005"));
}

void TestAnalysisSession::differentKeysRemainIndependent()
{
    AnalysisSession session;

    const CANFrame busOneFirst = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("01"));
    const CANFrame busOneSecond = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("02"));
    const CANFrame busTwo = makeFrame(
        0x123, 2, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("03"));

    session.ingest(busOneFirst);
    session.ingest(busOneSecond);
    session.ingest(busTwo);

    const FrameAggregateKey busOneKey =
        AnalysisSession::makeKey(busOneFirst);
    const FrameAggregateKey busTwoKey =
        AnalysisSession::makeKey(busTwo);

    FrameComparison busOneComparison;
    FrameComparison busTwoComparison;

    QCOMPARE(session.aggregateCount(), static_cast<std::size_t>(2));
    QVERIFY(session.compareLatest(busOneKey, &busOneComparison));
    QVERIFY(!session.compareLatest(busTwoKey, &busTwoComparison));

    QCOMPARE(busOneComparison.previous.payload, QByteArray::fromHex("01"));
    QCOMPARE(busOneComparison.current.payload, QByteArray::fromHex("02"));
    QCOMPARE(session.latestSnapshot(busTwoKey)->payload,
             QByteArray::fromHex("03"));
}

void TestAnalysisSession::comparisonUnavailableAfterFirstFrame()
{
    AnalysisSession session;

    const CANFrame frame = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("01"));

    session.ingest(frame);

    FrameComparison comparison;

    QVERIFY(!session.compareLatest(
        AnalysisSession::makeKey(frame),
        &comparison));
}

void TestAnalysisSession::comparisonRejectsNullOutputPointer()
{
    AnalysisSession session;

    const CANFrame first = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("01"));
    const CANFrame second = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("02"));

    session.ingest(first);
    session.ingest(second);

    QVERIFY(!session.compareLatest(
        AnalysisSession::makeKey(first),
        nullptr));
}

void TestAnalysisSession::depthOneNeverCreatesComparison()
{
    AnalysisSession session(1);

    const CANFrame first = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("01"));
    const CANFrame second = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("02"));

    session.ingest(first);
    session.ingest(second);

    FrameComparison comparison;

    QVERIFY(!session.compareLatest(
        AnalysisSession::makeKey(first),
        &comparison));
    QCOMPARE(session.latestSnapshot(
                        AnalysisSession::makeKey(first))
                 ->payload,
             QByteArray::fromHex("02"));
}

void TestAnalysisSession::clearRemovesAggregateAndHistoryState()
{
    AnalysisSession session;

    const CANFrame frame = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("01"));

    session.ingest(frame);

    const FrameAggregateKey key = AnalysisSession::makeKey(frame);

    session.clear();

    QVERIFY(session.empty());
    QCOMPARE(session.aggregateCount(), static_cast<std::size_t>(0));
    QVERIFY(session.findAggregate(key) == nullptr);
    QVERIFY(session.latestSnapshot(key) == nullptr);
    QVERIFY(session.previousSnapshot(key) == nullptr);
}

void TestAnalysisSession::activityAgeIsAvailableAfterIngest()
{
    AnalysisSession session;

    const CANFrame frame = makeFrame(
        0x123,
        1,
        true,
        false,
        QCanBusFrame::DataFrame,
        QByteArray::fromHex("01"));

    const FrameAggregateKey key = AnalysisSession::makeKey(frame);

    QVERIFY(!session.hasActivityTimestamp(key));
    QCOMPARE(session.activityAgeMilliseconds(key), static_cast<qint64>(-1));

    session.ingest(frame);

    QVERIFY(session.hasActivityTimestamp(key));

    const qint64 activityAgeMilliseconds =
        session.activityAgeMilliseconds(key);

    QVERIFY(activityAgeMilliseconds >= 0);
}

void TestAnalysisSession::clearRemovesActivityAgeState()
{
    AnalysisSession session;

    const CANFrame frame = makeFrame(
        0x123,
        1,
        true,
        false,
        QCanBusFrame::DataFrame,
        QByteArray::fromHex("01"));

    const FrameAggregateKey key = AnalysisSession::makeKey(frame);

    session.ingest(frame);

    QVERIFY(session.hasActivityTimestamp(key));

    session.clear();

    QVERIFY(!session.hasActivityTimestamp(key));
    QCOMPARE(session.activityAgeMilliseconds(key), static_cast<qint64>(-1));
}

void TestAnalysisSession::comparisonOwnsDataAfterLaterIngest()
{
    AnalysisSession session;

    const CANFrame first = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("01"));
    const CANFrame second = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("02"));
    const CANFrame third = makeFrame(
        0x123, 1, true, false, QCanBusFrame::DataFrame,
        QByteArray::fromHex("03"));

    session.ingest(first);
    session.ingest(second);

    const FrameAggregateKey key = AnalysisSession::makeKey(first);
    FrameComparison comparison;

    QVERIFY(session.compareLatest(key, &comparison));

    session.ingest(third);

    QCOMPARE(comparison.previous.payload, QByteArray::fromHex("01"));
    QCOMPARE(comparison.current.payload, QByteArray::fromHex("02"));
    QCOMPARE(comparison.payloadDiff.changedByteMask,
             QByteArray::fromHex("FF"));
    QCOMPARE(comparison.payloadDiff.changedBitMask,
             QByteArray::fromHex("03"));
}
