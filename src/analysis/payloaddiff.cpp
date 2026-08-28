#include "payloaddiff.h"

// C++ standard-library headers
#include <algorithm>

PayloadDiff PayloadDiffCalculator::compare(const QByteArray &previous,
                                           const QByteArray &current)
{
    PayloadDiff result;

    result.previousLength = previous.size();
    result.currentLength = current.size();
    result.lengthChanged = result.previousLength != result.currentLength;

    const int comparedLength = std::max(
        result.previousLength,
        result.currentLength);

    result.changedByteMask.resize(comparedLength);
    result.changedBitMask.resize(comparedLength);

    for (int index = 0; index < comparedLength; ++index)
    {
        const bool hasPrevious = index < result.previousLength;
        const bool hasCurrent = index < result.currentLength;

        if (!hasPrevious || !hasCurrent)
        {
            result.changedByteMask[index] = static_cast<char>(0xFF);
            result.changedBitMask[index] = static_cast<char>(0xFF);
            result.hasChanges = true;
            continue;
        }

        const unsigned char previousByte =
            static_cast<unsigned char>(previous.at(index));
        const unsigned char currentByte =
            static_cast<unsigned char>(current.at(index));

        const unsigned char changedBits = previousByte ^ currentByte;

        result.changedBitMask[index] = static_cast<char>(changedBits);
        result.changedByteMask[index] =
            changedBits == 0 ? static_cast<char>(0x00)
                             : static_cast<char>(0xFF);

        if (changedBits != 0)
        {
            result.hasChanges = true;
        }
    }

    return result;
}
