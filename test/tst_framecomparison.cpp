// SavvyLens headers
#include "tst_framecomparison.h"
#include "src/analysis/framecomparison.h"

// Qt headers
#include <QtTest>

namespace
{

    FrameHistorySnapshot makeSnapshot(const QByteArray &payload,
                                      std::uint64_t timedelta = 0,
                                      std::uint32_t frameCount = 1)
    {
        FrameHistorySnapshot snapshot;
        snapshot.payload = payload;
        snapshot.sourceTimedelta = timedelta;
        snapshot.sourceFrameCount = frameCount;

        return snapshot;
    }

}

void TestFrameComparison::identicalPayloadsHaveNoChanges()
{
    const FrameHistorySnapshot previous =
        makeSnapshot(QByteArray::fromHex("102030"));
    const FrameHistorySnapshot current =
        makeSnapshot(QByteArray::fromHex("102030"));

    const FrameComparison comparison =
        FrameComparisonCalculator::compare(previous, current);

    QVERIFY(!comparison.payloadDiff.hasChanges);
    QVERIFY(!comparison.payloadDiff.lengthChanged);
    QCOMPARE(comparison.payloadDiff.changedByteMask,
             QByteArray::fromHex("000000"));
    QCOMPARE(comparison.payloadDiff.changedBitMask,
             QByteArray::fromHex("000000"));
}

void TestFrameComparison::changedPayloadProducesExpectedMasks()
{
    const FrameHistorySnapshot previous =
        makeSnapshot(QByteArray::fromHex("102030"));
    const FrameHistorySnapshot current =
        makeSnapshot(QByteArray::fromHex("102530"));

    const FrameComparison comparison =
        FrameComparisonCalculator::compare(previous, current);

    QVERIFY(comparison.payloadDiff.hasChanges);
    QVERIFY(!comparison.payloadDiff.lengthChanged);
    QCOMPARE(comparison.payloadDiff.changedByteMask,
             QByteArray::fromHex("00FF00"));
    QCOMPARE(comparison.payloadDiff.changedBitMask,
             QByteArray::fromHex("000500"));
}

void TestFrameComparison::lengthChangesAreReported()
{
    const FrameHistorySnapshot previous =
        makeSnapshot(QByteArray::fromHex("102030"));
    const FrameHistorySnapshot current =
        makeSnapshot(QByteArray::fromHex("10203040"));

    const FrameComparison comparison =
        FrameComparisonCalculator::compare(previous, current);

    QVERIFY(comparison.payloadDiff.hasChanges);
    QVERIFY(comparison.payloadDiff.lengthChanged);
    QCOMPARE(comparison.payloadDiff.previousLength, 3);
    QCOMPARE(comparison.payloadDiff.currentLength, 4);
    QCOMPARE(comparison.payloadDiff.changedByteMask,
             QByteArray::fromHex("000000FF"));
    QCOMPARE(comparison.payloadDiff.changedBitMask,
             QByteArray::fromHex("000000FF"));
}

void TestFrameComparison::snapshotMetadataIsPreserved()
{
    const FrameHistorySnapshot previous =
        makeSnapshot(QByteArray::fromHex("10"), 25, 3);
    const FrameHistorySnapshot current =
        makeSnapshot(QByteArray::fromHex("20"), 50, 7);

    const FrameComparison comparison =
        FrameComparisonCalculator::compare(previous, current);

    QCOMPARE(comparison.previous.payload, QByteArray::fromHex("10"));
    QCOMPARE(comparison.previous.sourceTimedelta,
             static_cast<std::uint64_t>(25));
    QCOMPARE(comparison.previous.sourceFrameCount,
             static_cast<std::uint32_t>(3));

    QCOMPARE(comparison.current.payload, QByteArray::fromHex("20"));
    QCOMPARE(comparison.current.sourceTimedelta,
             static_cast<std::uint64_t>(50));
    QCOMPARE(comparison.current.sourceFrameCount,
             static_cast<std::uint32_t>(7));
}

void TestFrameComparison::comparisonOwnsCopiedSnapshots()
{
    FrameHistorySnapshot previous =
        makeSnapshot(QByteArray::fromHex("10"), 25, 3);
    FrameHistorySnapshot current =
        makeSnapshot(QByteArray::fromHex("20"), 50, 7);

    const FrameComparison comparison =
        FrameComparisonCalculator::compare(previous, current);

    previous.payload = QByteArray::fromHex("FF");
    previous.sourceTimedelta = 99;
    previous.sourceFrameCount = 88;

    current.payload = QByteArray::fromHex("EE");
    current.sourceTimedelta = 77;
    current.sourceFrameCount = 66;

    QCOMPARE(comparison.previous.payload, QByteArray::fromHex("10"));
    QCOMPARE(comparison.previous.sourceTimedelta,
             static_cast<std::uint64_t>(25));
    QCOMPARE(comparison.previous.sourceFrameCount,
             static_cast<std::uint32_t>(3));

    QCOMPARE(comparison.current.payload, QByteArray::fromHex("20"));
    QCOMPARE(comparison.current.sourceTimedelta,
             static_cast<std::uint64_t>(50));
    QCOMPARE(comparison.current.sourceFrameCount,
             static_cast<std::uint32_t>(7));
}
