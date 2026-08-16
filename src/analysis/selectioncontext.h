#ifndef SELECTIONCONTEXT_H
#define SELECTIONCONTEXT_H

#include <QSet>
#include <QString>
#include <QtGlobal>

class SelectionContext
{
public:
    struct TimeRange
    {
        qint64 startUsec = -1;
        qint64 endUsec = -1;

        bool isValid() const;
        void clear();
    };

    struct BitRange
    {
        int startBit = -1;
        int bitLength = 0;

        bool isValid() const;
        void clear();
    };

    SelectionContext() = default;

    bool isEmpty() const;
    void clear();

    void setSourceId(const QString &sourceId);
    const QString &sourceId() const;

    void setBus(int bus);
    int bus() const;
    bool hasBus() const;
    void clearBus();

    void setCanIds(const QSet<quint32> &canIds);
    const QSet<quint32> &canIds() const;

    void setCanId(quint32 canId);
    bool hasSingleCanId() const;
    quint32 canId() const;
    void clearCanIds();

    void setFrameIndex(qint64 frameIndex);
    qint64 frameIndex() const;
    bool hasFrameIndex() const;
    void clearFrameIndex();

    void setBitRange(int startBit, int bitLength);
    const BitRange &bitRange() const;
    void clearBitRange();

    void setTimeRange(qint64 startUsec, qint64 endUsec);
    const TimeRange &timeRange() const;
    void clearTimeRange();

    void setSignalId(const QString &signalId);
    const QString &signalId() const;
    bool hasSignal() const;
    void clearSignal();

private:
    QString m_sourceId;
    int m_bus = -1;
    QSet<quint32> m_canIds;
    qint64 m_frameIndex = -1;
    BitRange m_bitRange;
    TimeRange m_timeRange;
    QString m_signalId;
};

#endif // SELECTIONCONTEXT_H