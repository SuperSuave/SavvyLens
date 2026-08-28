#ifndef TST_SIGNALRESOLVER_H
#define TST_SIGNALRESOLVER_H

// Qt headers
#include <QObject>

class TestSignalResolver : public QObject
{
    Q_OBJECT

private slots:
    void decode8BitUnsignedIntel();
    void decode16BitSignedLittleEndian();
    void decodeMotorolaBigEndianSignal();
    void rejectShortFrame();
};

#endif // TST_SIGNALRESOLVER_H
