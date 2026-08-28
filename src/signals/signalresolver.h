#ifndef SIGNALRESOLVER_H
#define SIGNALRESOLVER_H

// SavvyLens headers
#include "can/can_structs.h"
#include "signaldefinition.h"

// Qt headers
#include <QString>

// C++ standard-library headers
#include <cstddef>
#include <cstdint>

namespace Signals {

struct ResolveResult
{
    bool valid = false;
    double rawValue = 0.0;
    double physicalValue = 0.0;
    QString errorMessage;
};

class SignalResolver
{
public:
    static ResolveResult decodeSignal(const CANFrame &frame, const SignalDefinition &signal);
    static bool extractRawBits(const uint8_t *payload, std::size_t payloadLength,
                               const SignalDefinition &signal, uint64_t &rawBitsOut);
};

} // namespace Signals

#endif // SIGNALRESOLVER_H
