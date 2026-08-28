#ifndef SIGNALDEFINITION_H
#define SIGNALDEFINITION_H

// Qt headers
#include <QString>

// C++ standard-library headers
#include <cstdint>

namespace Signals {

enum class ByteOrder {
    LittleEndian, // Intel
    BigEndian     // Motorola
};

enum class ValueType {
    Unsigned,
    Signed,
    Float,
    Double
};

struct SignalDefinition
{
    QString name;
    quint32 canId = 0;
    int startBit = 0;
    int bitLength = 8;
    ByteOrder byteOrder = ByteOrder::LittleEndian;
    ValueType valueType = ValueType::Unsigned;
    double scale = 1.0;
    double offset = 0.0;
    double minimum = 0.0;
    double maximum = 0.0;
    QString unit;

    constexpr bool isValid() const noexcept
    {
        return bitLength > 0 && bitLength <= 64 && startBit >= 0;
    }
};

} // namespace Signals

#endif // SIGNALDEFINITION_H
