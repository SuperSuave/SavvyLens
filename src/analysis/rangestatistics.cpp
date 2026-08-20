#include "rangestatistics.h"

// Qt headers
#include <QtMath>

// C++ standard library
#include <algorithm>
#include <array>
#include <cmath>

bool RangeSignalSpec::isValid() const
{
    return bitLength > 0 && bitLength <= 64 && startBit >= 0;
}

bool RangeSignalSpec::operator==(const RangeSignalSpec &other) const
{
    return canId == other.canId &&
           startBit == other.startBit &&
           bitLength == other.bitLength &&
           isLittleEndian == other.isLittleEndian &&
           isSigned == other.isSigned;
}

bool RangeSignalSpec::operator!=(const RangeSignalSpec &other) const
{
    return !(*this == other);
}

QString RangeSignalSpec::displayName() const
{
    QString name = QString("ID: %1 startBit: %2 len: %3")
                       .arg(QString::number(canId, 16).toUpper())
                       .arg(startBit)
                       .arg(bitLength);

    name += isSigned ? " Signed" : " Unsigned";
    name += isLittleEndian ? " LittleEndian" : " BigEndian";
    return name;
}

QString RangeSignalCandidate::summaryText() const
{
    return QString("%1 [Range: %2..%3 (Span: %4, %5%)] (Smoothness: %6)")
        .arg(spec.displayName())
        .arg(minValue)
        .arg(maxValue)
        .arg(rangeSpan)
        .arg(QString::number(rangeCoverage * 100.0, 'f', 1))
        .arg(QString::number(smoothnessScore, 'f', 2));
}

quint8 ByteRangeStats::unsignedSpan() const
{
    if (sampleCount == 0 || maxUnsigned < minUnsigned) {
        return 0;
    }
    return maxUnsigned - minUnsigned;
}

int ByteRangeStats::signedSpan() const
{
    if (sampleCount == 0 || maxSigned < minSigned) {
        return 0;
    }
    return static_cast<int>(maxSigned) - static_cast<int>(minSigned);
}

RangeSignalPayloadSupport RangeStatistics::payloadSupport(
    int payloadLengthBytes,
    const RangeSignalSpec &spec)
{
    RangeSignalPayloadSupport support;

    if (payloadLengthBytes <= 0 || !spec.isValid())
    {
        return support;
    }

    if (spec.isLittleEndian)
    {
        const int payloadBits = payloadLengthBytes * 8;

        if (spec.startBit + spec.bitLength > payloadBits)
        {
            return support;
        }

        support.isSupported = true;
        support.highestPayloadByteIndex =
            (spec.startBit + spec.bitLength - 1) / 8;

        return support;
    }

    int bit = spec.startBit;
    int highestByteIndex = -1;

    for (int bitPosition = 0;
         bitPosition < spec.bitLength;
         ++bitPosition)
    {
        const int byteIndex = bit / 8;

        if (byteIndex < 0 || byteIndex >= payloadLengthBytes)
        {
            return support;
        }

        highestByteIndex = qMax(highestByteIndex, byteIndex);

        const int bitInByte = bit % 8;

        if (bitInByte == 0)
        {
            bit += 15;
        }
        else
        {
            --bit;
        }
    }

    support.isSupported = true;
    support.highestPayloadByteIndex = highestByteIndex;

    return support;
}

qint64 RangeStatistics::extractValue(
    const QByteArray &payload,
    int startBit,
    int bitLength,
    bool isLittleEndian,
    bool isSigned)
{
    if (bitLength <= 0 || bitLength > 64 || startBit < 0 || payload.isEmpty()) {
        return 0;
    }

    uint64_t result = 0;
    const int totalPayloadBits = payload.size() * 8;

    if (isLittleEndian) {
        if (startBit + bitLength > totalPayloadBits) {
            return 0;
        }

        for (int bitpos = 0; bitpos < bitLength; ++bitpos) {
            int bit = startBit + bitpos;
            int bytePos = bit / 8;
            int bitInByte = bit % 8;

            if (bytePos < payload.size()) {
                if (payload.at(bytePos) & (1 << bitInByte)) {
                    result |= (1ULL << bitpos);
                }
            }
        }
    } else { // Big Endian / Motorola (CAN DBC standard)
        int bit = startBit;
        for (int bitpos = 0; bitpos < bitLength; ++bitpos) {
            int bytePos = bit / 8;
            int bitInByte = bit % 8;

            if (bytePos >= payload.size() || bytePos < 0) {
                return 0;
            }

            if (payload.at(bytePos) & (1 << bitInByte)) {
                result |= (1ULL << (bitLength - bitpos - 1));
            }

            if (bitInByte == 0) {
                bit += 15;
            } else {
                bit -= 1;
            }
        }
    }

    if (isSigned && bitLength < 64) {
        uint64_t signBit = 1ULL << (bitLength - 1);
        if (result & signBit) {
            result |= (~0ULL << bitLength);
        }
    }

    return static_cast<qint64>(result);
}

qint64 RangeStatistics::extractValue(const QByteArray &payload,
                                    const RangeSignalSpec &spec)
{
    return extractValue(payload, spec.startBit, spec.bitLength, spec.isLittleEndian, spec.isSigned);
}

QVector<qint64> RangeStatistics::extractSignalValues(
    const QVector<CANFrame> &frames,
    const RangeSignalSpec &spec)
{
    QVector<qint64> values;
    values.reserve(frames.size());

    for (const CANFrame &frame : frames)
    {
        if (frame.frameId() != spec.canId)
        {
            continue;
        }

        const RangeSignalPayloadSupport support = payloadSupport(
            frame.payload().size(),
            spec);

        if (!support.isSupported)
        {
            continue;
        }

        values.append(extractValue(frame.payload(), spec));
    }

    return values;
}

ByteRangeStats RangeStatistics::computeByteStats(const QVector<CANFrame> &frames,
                                                 int byteIndex,
                                                 quint32 canId)
{
    ByteRangeStats stats;
    stats.byteIndex = byteIndex;

    if (byteIndex < 0 || frames.isEmpty()) {
        return stats;
    }

    std::array<bool, 256> seenValues{};
    quint8 firstByteVal = 0;
    bool hasFirst = false;

    for (const CANFrame &frame : frames) {
        if (frame.frameId() != canId) {
            continue;
        }

        const QByteArray &payload = frame.payload();
        if (byteIndex >= payload.size()) {
            continue;
        }

        quint8 uVal = static_cast<quint8>(payload.at(byteIndex));
        qint8 sVal = static_cast<qint8>(payload.at(byteIndex));

        if (!hasFirst) {
            firstByteVal = uVal;
            hasFirst = true;
            stats.minUnsigned = uVal;
            stats.maxUnsigned = uVal;
            stats.minSigned = sVal;
            stats.maxSigned = sVal;
            stats.bitSetMask = uVal;
            stats.bitClearMask = static_cast<quint8>(~uVal);
        } else {
            if (uVal < stats.minUnsigned) stats.minUnsigned = uVal;
            if (uVal > stats.maxUnsigned) stats.maxUnsigned = uVal;
            if (sVal < stats.minSigned) stats.minSigned = sVal;
            if (sVal > stats.maxSigned) stats.maxSigned = sVal;

            stats.bitChangeMask |= static_cast<quint8>(firstByteVal ^ uVal);
            stats.bitSetMask |= uVal;
            stats.bitClearMask |= static_cast<quint8>(~uVal);
        }

        if (!seenValues[uVal]) {
            seenValues[uVal] = true;
            stats.uniqueValues++;
        }

        stats.sampleCount++;
    }

    stats.isConstant = (stats.uniqueValues <= 1);
    return stats;
}

QVector<ByteRangeStats> RangeStatistics::computeAllByteStats(const QVector<CANFrame> &frames,
                                                            quint32 canId)
{
    int maxLen = 0;
    for (const CANFrame &frame : frames) {
        if (frame.frameId() == canId) {
            maxLen = std::max(maxLen, frame.payload().size());
        }
    }

    QVector<ByteRangeStats> allStats;
    allStats.reserve(maxLen);

    for (int i = 0; i < maxLen; ++i) {
        allStats.append(computeByteStats(frames, i, canId));
    }

    return allStats;
}

double RangeStatistics::linearInterpolate(double a, double b, double factor)
{
    return (a * (1.0 - factor)) + (b * factor);
}

bool RangeStatistics::isSmoothRange(const QVector<qint64> &values,
                                   int bitLength,
                                   bool isSigned,
                                   int sensitivity,
                                   int &outFirstOrderSpikes,
                                   int &outSecondOrderSpikes,
                                   double &outSmoothnessScore,
                                   qint64 &outRangeSpan,
                                   double &outRangeCoverage)
{
    outFirstOrderSpikes = 0;
    outSecondOrderSpikes = 0;
    outSmoothnessScore = 0.0;
    outRangeSpan = 0;
    outRangeCoverage = 0.0;

    const int numFrames = values.size();
    if (numFrames < 2 || bitLength <= 0) {
        return false;
    }

    qint64 lowestVal = values.at(0);
    qint64 highestVal = values.at(0);

    for (int i = 1; i < numFrames; ++i) {
        qint64 v = values.at(i);
        if (v < lowestVal) lowestVal = v;
        if (v > highestVal) highestVal = v;
    }

    if (lowestVal == highestVal) {
        return false; // Static signal is not a range signal
    }

    outRangeSpan = highestVal - lowestVal;

    uint64_t maxRange;
    if (bitLength >= 63) {
        maxRange = isSigned ? (1ULL << 62) : (1ULL << 63);
    } else {
        maxRange = isSigned ? (1ULL << (bitLength - 1)) : (1ULL << bitLength);
    }
    if (maxRange == 0) {
        maxRange = 1;
    }

    outRangeCoverage = static_cast<double>(outRangeSpan) / static_cast<double>(maxRange);

    double lerpPoint = qBound(0.0, (static_cast<double>(sensitivity) - 10.0) / 240.0, 1.0);
    double minPercent = 0.005;
    double maxPercent = 0.10;
    if (bitLength >= 16) {
        minPercent = 0.001; // 0.1% (65 counts for 16-bit)
        maxPercent = 0.02;  // 2.0% (1310 counts for 16-bit)
    }

    double reqPercent = linearInterpolate(minPercent, maxPercent, lerpPoint);
    qint64 requiredRange = static_cast<qint64>(maxRange * reqPercent);
    if (requiredRange < 2) {
        requiredRange = 2;
    }
    if (bitLength >= 24) {
        requiredRange = qMin(requiredRange, static_cast<qint64>(linearInterpolate(20.0, 500.0, lerpPoint)));
    }

    if (outRangeSpan < requiredRange) {
        return false; // Doesn't span enough range
    }

    // 1st order differences (velocity)
    QVector<qint64> diff1;
    diff1.reserve(numFrames - 1);
    for (int i = 1; i < numFrames; ++i) {
        diff1.append(values.at(i) - values.at(i - 1));
    }

    // 2nd order differences (acceleration)
    QVector<qint64> diff2;
    diff2.reserve(diff1.size() - 1);
    for (int i = 1; i < diff1.size(); ++i) {
        diff2.append(diff1.at(i) - diff1.at(i - 1));
    }

    double comparisonValue1 = linearInterpolate(static_cast<double>(outRangeSpan) * 0.55, 0.0, lerpPoint);
    for (qint64 d1 : diff1) {
        if (std::abs(d1) > comparisonValue1) {
            outFirstOrderSpikes++;
        }
    }

    double maxOvers1 = linearInterpolate(static_cast<double>(numFrames) / 30.0, 2.0, lerpPoint);

    double comparisonValue2 = linearInterpolate(static_cast<double>(outRangeSpan) * 0.20, 1.0, lerpPoint);
    for (qint64 d2 : diff2) {
        if (std::abs(d2) > comparisonValue2) {
            outSecondOrderSpikes++;
        }
    }

    double maxOvers2 = linearInterpolate(8.0, 2.0, lerpPoint);

    // Compute normalized smoothness score
    double spikeRatio1 = diff1.isEmpty() ? 0.0 : static_cast<double>(outFirstOrderSpikes) / static_cast<double>(diff1.size());
    double spikeRatio2 = diff2.isEmpty() ? 0.0 : static_cast<double>(outSecondOrderSpikes) / static_cast<double>(diff2.size());
    outSmoothnessScore = qBound(0.0, 1.0 - (spikeRatio1 * 0.6 + spikeRatio2 * 0.4), 1.0);

    bool isGood = (outFirstOrderSpikes <= static_cast<int>(std::ceil(maxOvers1))) &&
                  (outSecondOrderSpikes <= static_cast<int>(std::ceil(maxOvers2)));

    return isGood;
}

RangeSignalCandidate RangeStatistics::evaluateValues(const QVector<qint64> &values,
                                                    const RangeSignalSpec &spec,
                                                    int sensitivity,
                                                    bool includeSamples)
{
    RangeSignalCandidate candidate;
    candidate.spec = spec;
    candidate.sampleCount = values.size();

    if (values.isEmpty()) {
        return candidate;
    }

    if (includeSamples) {
        candidate.sampleValues = values;
    }

    qint64 lowestVal = values.at(0);
    qint64 highestVal = values.at(0);
    QSet<qint64> uniqueSet;

    for (qint64 val : values) {
        if (val < lowestVal) lowestVal = val;
        if (val > highestVal) highestVal = val;
        uniqueSet.insert(val);
    }

    candidate.minValue = lowestVal;
    candidate.maxValue = highestVal;
    candidate.uniqueValueCount = uniqueSet.size();

    candidate.isRanging = isSmoothRange(values,
                                       spec.bitLength,
                                       spec.isSigned,
                                       sensitivity,
                                       candidate.firstOrderSpikes,
                                       candidate.secondOrderSpikes,
                                       candidate.smoothnessScore,
                                       candidate.rangeSpan,
                                       candidate.rangeCoverage);

    return candidate;
}

RangeSignalCandidate RangeStatistics::evaluateSignal(const QVector<CANFrame> &frames,
                                                    const RangeSignalSpec &spec,
                                                    int sensitivity,
                                                    bool includeSamples)
{
    QVector<qint64> values = extractSignalValues(frames, spec);
    return evaluateValues(values, spec, sensitivity, includeSamples);
}

QVector<RangeSignalCandidate> RangeStatistics::scanCandidates(
    const QVector<CANFrame> &frames,
    quint32 canId,
    const RangeScanConfig &config,
    std::function<bool(int currentStep, int totalSteps)> progressCallback)
{
    QVector<RangeSignalCandidate> candidates;

    QVector<CANFrame> idFrames;
    idFrames.reserve(frames.size());
    for (const CANFrame &frame : frames) {
        if (frame.frameId() == canId) {
            idFrames.append(frame);
        }
    }

    if (idFrames.size() < 2) {
        return candidates;
    }

    int minimumPayloadBytes = idFrames.first().payload().size();

    for (const CANFrame &frame : idFrames)
    {
        minimumPayloadBytes = qMin(
            minimumPayloadBytes,
            frame.payload().size());
    }

    const int payloadBits = minimumPayloadBytes * 8;

    if (payloadBits <= 0)
    {
        return candidates;
    }

    // Determine candidate iterations
    int minSig = std::max(1, config.minBitLength);
    int maxSig = std::min(payloadBits, config.maxBitLength);
    int granularity = std::max(1, config.bitGranularity);

    QVector<int> sigSizes;
    for (int s = maxSig; s >= minSig; s -= granularity) {
        sigSizes.append(s);
    }

    int totalSteps = 0;
    for (int sigSize : sigSizes) {
        for (int startBit = 0; startBit <= payloadBits - sigSize; startBit += granularity) {
            int endianVariants = (config.endianMode == RangeScanConfig::TryBothEndian) ? 2 : 1;
            int signedVariants = (config.signedMode == RangeScanConfig::TryBothSigned) ? 2 : 1;
            totalSteps += endianVariants * signedVariants;
        }
    }

    int currentStep = 0;

    for (int sigSize : sigSizes) {
        for (int startBit = 0; startBit <= payloadBits - sigSize; startBit += granularity) {
            QVector<bool> endianList;
            if (config.endianMode == RangeScanConfig::LittleEndianOnly || config.endianMode == RangeScanConfig::TryBothEndian) {
                endianList.append(true);
            }
            if (config.endianMode == RangeScanConfig::BigEndianOnly || config.endianMode == RangeScanConfig::TryBothEndian) {
                endianList.append(false);
            }

            QVector<bool> signedList;
            if (config.signedMode == RangeScanConfig::SignedOnly || config.signedMode == RangeScanConfig::TryBothSigned) {
                signedList.append(true);
            }
            if (config.signedMode == RangeScanConfig::UnsignedOnly || config.signedMode == RangeScanConfig::TryBothSigned) {
                signedList.append(false);
            }

            for (bool isLittleEndian : endianList) {
                for (bool isSigned : signedList) {
                    currentStep++;
                    if (progressCallback && !progressCallback(currentStep, totalSteps)) {
                        return candidates; // Cancelled
                    }

                    RangeSignalSpec spec;
                    spec.canId = canId;
                    spec.startBit = startBit;
                    spec.bitLength = sigSize;
                    spec.isLittleEndian = isLittleEndian;
                    spec.isSigned = isSigned;

                    const RangeSignalPayloadSupport support = payloadSupport(
                        minimumPayloadBytes,
                        spec);

                    if (!support.isSupported)
                    {
                        continue;
                    }

                    RangeSignalCandidate candidate = evaluateSignal(
                        idFrames,
                        spec,
                        config.sensitivity,
                        config.populateSamples);
                    if (candidate.isRanging) {
                        candidates.append(candidate);
                        if (candidates.size() >= config.maxCandidates) {
                            return candidates;
                        }
                    }
                }
            }
        }
    }

    return candidates;
}

QVector<RangeSignalCandidate> RangeStatistics::scanMultipleIds(
    const QVector<CANFrame> &frames,
    const QSet<quint32> &canIds,
    const RangeScanConfig &config,
    std::function<bool(int currentStep, int totalSteps)> progressCallback)
{
    QVector<RangeSignalCandidate> allCandidates;
    int totalIds = canIds.size();
    int currentIdIdx = 0;

    for (quint32 id : canIds) {
        currentIdIdx++;
        auto idProgress = [&](int step, int total) -> bool {
            if (!progressCallback) return true;
            int overallStep = ((currentIdIdx - 1) * 1000) + (step * 1000 / qMax(1, total));
            return progressCallback(overallStep, totalIds * 1000);
        };

        QVector<RangeSignalCandidate> idCandidates = scanCandidates(frames, id, config, idProgress);
        allCandidates.append(idCandidates);
        if (allCandidates.size() >= config.maxCandidates) {
            allCandidates.resize(config.maxCandidates);
            break;
        }
    }

    return allCandidates;
}
