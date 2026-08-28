// SavvyLens headers
#include "tst_payloaddiff.h"
#include "analysis/payloaddiff.h"

// Qt headers
#include <QtTest>

void TestPayloadDiff::identicalPayloadsHaveNoChanges()
{
    const QByteArray payload = QByteArray::fromHex("10203040");

    const PayloadDiff diff = PayloadDiffCalculator::compare(payload, payload);

    QCOMPARE(diff.previousLength, 4);
    QCOMPARE(diff.currentLength, 4);
    QVERIFY(!diff.lengthChanged);
    QVERIFY(!diff.hasChanges);
    QCOMPARE(diff.changedByteMask, QByteArray::fromHex("00000000"));
    QCOMPARE(diff.changedBitMask, QByteArray::fromHex("00000000"));
}

void TestPayloadDiff::changedByteProducesByteAndBitMasks()
{
    const QByteArray previous = QByteArray::fromHex("102030");
    const QByteArray current = QByteArray::fromHex("102530");

    const PayloadDiff diff = PayloadDiffCalculator::compare(previous, current);

    QCOMPARE(diff.previousLength, 3);
    QCOMPARE(diff.currentLength, 3);
    QVERIFY(!diff.lengthChanged);
    QVERIFY(diff.hasChanges);
    QCOMPARE(diff.changedByteMask, QByteArray::fromHex("00FF00"));
    QCOMPARE(diff.changedBitMask, QByteArray::fromHex("000500"));
}

void TestPayloadDiff::multipleChangedBytesProduceIndependentMasks()
{
    const QByteArray previous = QByteArray::fromHex("00F055AA");
    const QByteArray current = QByteArray::fromHex("0FF050A0");

    const PayloadDiff diff = PayloadDiffCalculator::compare(previous, current);

    QVERIFY(!diff.lengthChanged);
    QVERIFY(diff.hasChanges);
    QCOMPARE(diff.changedByteMask, QByteArray::fromHex("FF00FFFF"));
    QCOMPARE(diff.changedBitMask, QByteArray::fromHex("0F00050A"));
}

void TestPayloadDiff::payloadGrowthMarksIntroducedBytes()
{
    const QByteArray previous = QByteArray::fromHex("102030");
    const QByteArray current = QByteArray::fromHex("10203040");

    const PayloadDiff diff = PayloadDiffCalculator::compare(previous, current);

    QCOMPARE(diff.previousLength, 3);
    QCOMPARE(diff.currentLength, 4);
    QVERIFY(diff.lengthChanged);
    QVERIFY(diff.hasChanges);
    QCOMPARE(diff.changedByteMask, QByteArray::fromHex("000000FF"));
    QCOMPARE(diff.changedBitMask, QByteArray::fromHex("000000FF"));
}

void TestPayloadDiff::payloadShrinkMarksRemovedBytes()
{
    const QByteArray previous = QByteArray::fromHex("10203040");
    const QByteArray current = QByteArray::fromHex("102030");

    const PayloadDiff diff = PayloadDiffCalculator::compare(previous, current);

    QCOMPARE(diff.previousLength, 4);
    QCOMPARE(diff.currentLength, 3);
    QVERIFY(diff.lengthChanged);
    QVERIFY(diff.hasChanges);
    QCOMPARE(diff.changedByteMask, QByteArray::fromHex("000000FF"));
    QCOMPARE(diff.changedBitMask, QByteArray::fromHex("000000FF"));
}

void TestPayloadDiff::emptyPayloadsHaveNoChanges()
{
    const PayloadDiff diff = PayloadDiffCalculator::compare(
        QByteArray(),
        QByteArray());

    QCOMPARE(diff.previousLength, 0);
    QCOMPARE(diff.currentLength, 0);
    QVERIFY(!diff.lengthChanged);
    QVERIFY(!diff.hasChanges);
    QCOMPARE(diff.changedByteMask, QByteArray());
    QCOMPARE(diff.changedBitMask, QByteArray());
}
