#include "signalresolver.h"

// C++ standard-library headers
#include <cmath>
#include <cstring>

namespace Signals {

bool SignalResolver::extractRawBits(const uint8_t *payload, std::size_t payloadLength,
                                    const SignalDefinition &signal, uint64_t &rawBitsOut)
{
    if (!payload || !signal.isValid()) {
        return false;
    }

    rawBitsOut = 0;

    if (signal.byteOrder == ByteOrder::LittleEndian) {
        const int maxBitIndex = signal.startBit + signal.bitLength - 1;
        const std::size_t requiredBytes = static_cast<std::size_t>((maxBitIndex / 8) + 1);
        if (payloadLength < requiredBytes) {
            return false;
        }

        for (int i = 0; i < signal.bitLength; ++i) {
            const int bitPos = signal.startBit + i;
            const int byteIdx = bitPos / 8;
            const int bitIdx = bitPos % 8;
            const uint64_t bitVal = (payload[byteIdx] >> bitIdx) & 0x01ULL;
            rawBitsOut |= (bitVal << i);
        }
        return true;
    }

    // Motorola Big Endian bit extraction
    int currentBit = signal.startBit;
    for (int i = 0; i < signal.bitLength; ++i) {
        const int byteIdx = currentBit / 8;
        const int bitIdx = currentBit % 8;

        if (static_cast<std::size_t>(byteIdx) >= payloadLength) {
            return false;
        }

        const uint64_t bitVal = (payload[byteIdx] >> bitIdx) & 0x01ULL;
        rawBitsOut = (rawBitsOut << 1) | bitVal;

        if ((currentBit % 8) == 0) {
            currentBit += 15;
        } else {
            currentBit -= 1;
        }
    }

    return true;
}

ResolveResult SignalResolver::decodeSignal(const CANFrame &frame, const SignalDefinition &signal)
{
    ResolveResult result;

    if (!signal.isValid()) {
        result.errorMessage = QStringLiteral("Invalid signal definition layout");
        return result;
    }

    const auto *payload = reinterpret_cast<const uint8_t *>(frame.payload().constData());
    const std::size_t payloadLength = static_cast<std::size_t>(frame.payload().size());

    uint64_t rawBits = 0;
    if (!extractRawBits(payload, payloadLength, signal, rawBits)) {
        result.errorMessage = QStringLiteral("Payload length %1 insufficient for signal bit range")
                                  .arg(payloadLength);
        return result;
    }

    result.valid = true;

    if (signal.valueType == ValueType::Signed) {
        const uint64_t signBitMask = 1ULL << (signal.bitLength - 1);
        if (rawBits & signBitMask) {
            const uint64_t signExtension = ~0ULL << signal.bitLength;
            const int64_t signedRaw = static_cast<int64_t>(rawBits | signExtension);
            result.rawValue = static_cast<double>(signedRaw);
        } else {
            result.rawValue = static_cast<double>(rawBits);
        }
    } else {
        result.rawValue = static_cast<double>(rawBits);
    }

    result.physicalValue = (result.rawValue * signal.scale) + signal.offset;
    return result;
}

} // namespace Signals
