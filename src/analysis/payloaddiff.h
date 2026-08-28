#ifndef PAYLOADDIFF_H
#define PAYLOADDIFF_H

// Qt headers
#include <QByteArray>

struct PayloadDiff
{
    QByteArray changedByteMask;
    QByteArray changedBitMask;

    int previousLength = 0;
    int currentLength = 0;

    bool lengthChanged = false;
    bool hasChanges = false;
};

class PayloadDiffCalculator
{
public:
    static PayloadDiff compare(const QByteArray &previous,
                               const QByteArray &current);
};

#endif // PAYLOADDIFF_H
