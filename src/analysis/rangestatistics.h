#ifndef RANGESTATISTICS_H
#define RANGESTATISTICS_H

// SavvyLens headers
#include "can/can_structs.h"

// Qt headers
#include <QByteArray>
#include <QSet>
#include <QString>
#include <QVector>
#include <QtGlobal>

// C++ standard library
#include <cstdint>
#include <functional>

struct RangeSignalSpec
{
    quint32 canId = 0;
    int startBit = 0;
    int bitLength = 8;
    bool isLittleEndian = true;
    bool isSigned = false;

    bool isValid() const;
    bool operator==(const RangeSignalSpec &other) const;
    bool operator!=(const RangeSignalSpec &other) const;
    QString displayName() const;
};

struct RangeSignalCandidate
{
    RangeSignalSpec spec;
    qint64 minValue = 0;
    qint64 maxValue = 0;
    qint64 rangeSpan = 0;
    double rangeCoverage = 0.0;
    int uniqueValueCount = 0;
    int sampleCount = 0;
    int firstOrderSpikes = 0;
    int secondOrderSpikes = 0;
    double smoothnessScore = 0.0;
    bool isRanging = false;
    QVector<qint64> sampleValues;

    QString summaryText() const;
};

struct ByteRangeStats
{
    int byteIndex = 0;
    quint8 minUnsigned = 0xFF;
    quint8 maxUnsigned = 0x00;
    qint8 minSigned = 127;
    qint8 maxSigned = -128;
    int uniqueValues = 0;
    quint8 bitChangeMask = 0x00;
    quint8 bitSetMask = 0x00;
    quint8 bitClearMask = 0x00;
    bool isConstant = true;
    int sampleCount = 0;

    quint8 unsignedSpan() const;
    int signedSpan() const;
};

struct RangeScanConfig
{
    enum EndianPreference
    {
        LittleEndianOnly,
        BigEndianOnly,
        TryBothEndian
    };

    enum SignedPreference
    {
        UnsignedOnly,
        SignedOnly,
        TryBothSigned
    };

    int minBitLength = 8;
    int maxBitLength = 32;
    int bitGranularity = 8;
    EndianPreference endianMode = TryBothEndian;
    SignedPreference signedMode = TryBothSigned;
    int sensitivity = 128;
    int maxCandidates = 500;
    bool populateSamples = true;
};

struct RangeSignalPayloadSupport
{
    bool isSupported = false;
    int highestPayloadByteIndex = -1;
};

class RangeStatistics
{
public:
    // Value extraction primitives
    static RangeSignalPayloadSupport payloadSupport(int payloadLengthBytes,
                                                    const RangeSignalSpec &spec);
                                                    
    static qint64 extractValue(const QByteArray &payload,
                               int startBit,
                               int bitLength,
                               bool isLittleEndian,
                               bool isSigned);

    static qint64 extractValue(const QByteArray &payload,
                               const RangeSignalSpec &spec);

    static QVector<qint64> extractSignalValues(const QVector<CANFrame> &frames,
                                              const RangeSignalSpec &spec);

    // Byte-level statistics for one exact CAN ID, including CAN ID 0x000.
    static ByteRangeStats computeByteStats(const QVector<CANFrame> &frames,
                                           int byteIndex,
                                           quint32 canId);

    static QVector<ByteRangeStats> computeAllByteStats(const QVector<CANFrame> &frames,
                                                      quint32 canId);

    // Single signal evaluation
    static RangeSignalCandidate evaluateSignal(const QVector<CANFrame> &frames,
                                               const RangeSignalSpec &spec,
                                               int sensitivity = 128,
                                               bool includeSamples = true);

    static RangeSignalCandidate evaluateValues(const QVector<qint64> &values,
                                               const RangeSignalSpec &spec,
                                               int sensitivity = 128,
                                               bool includeSamples = true);

    // Candidate scanning
    static QVector<RangeSignalCandidate> scanCandidates(
        const QVector<CANFrame> &frames,
        quint32 canId,
        const RangeScanConfig &config = RangeScanConfig(),
        std::function<bool(int currentStep, int totalSteps)> progressCallback = nullptr);

    static QVector<RangeSignalCandidate> scanMultipleIds(
        const QVector<CANFrame> &frames,
        const QSet<quint32> &canIds,
        const RangeScanConfig &config = RangeScanConfig(),
        std::function<bool(int currentStep, int totalSteps)> progressCallback = nullptr);

    // Math & smoothing helpers
    static double linearInterpolate(double a, double b, double factor);

    static bool isSmoothRange(const QVector<qint64> &values,
                             int bitLength,
                             bool isSigned,
                             int sensitivity,
                             int &outFirstOrderSpikes,
                             int &outSecondOrderSpikes,
                             double &outSmoothnessScore,
                             qint64 &outRangeSpan,
                             double &outRangeCoverage);
};

#endif // RANGESTATISTICS_H
