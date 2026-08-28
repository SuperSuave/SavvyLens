#include "selectioncontext.h"

bool SelectionContext::TimeRange::isValid() const
{
    return startUsec >= 0 && endUsec >= startUsec;
}

void SelectionContext::TimeRange::clear()
{
    startUsec = -1;
    endUsec = -1;
}

bool SelectionContext::BitRange::isValid() const
{
    return startBit >= 0 && bitLength > 0;
}

void SelectionContext::BitRange::clear()
{
    startBit = -1;
    bitLength = 0;
}

bool SelectionContext::isEmpty() const
{
    return m_sourceId.isEmpty() && !hasBus() && m_canIds.isEmpty() && !hasFrameIndex() && !m_bitRange.isValid() && !m_timeRange.isValid() && !hasSignal();
}

void SelectionContext::clear()
{
    m_sourceId.clear();
    clearBus();
    clearCanIds();
    clearFrameIndex();
    clearBitRange();
    clearTimeRange();
    clearSignal();
}

void SelectionContext::setSourceId(const QString &sourceId)
{
    m_sourceId = sourceId;
}

const QString &SelectionContext::sourceId() const
{
    return m_sourceId;
}

void SelectionContext::setBus(int bus)
{
    m_bus = bus;
}

int SelectionContext::bus() const
{
    return m_bus;
}

bool SelectionContext::hasBus() const
{
    return m_bus >= 0;
}

void SelectionContext::clearBus()
{
    m_bus = -1;
}

void SelectionContext::setCanIds(const QSet<quint32> &canIds)
{
    m_canIds = canIds;
}

const QSet<quint32> &SelectionContext::canIds() const
{
    return m_canIds;
}

void SelectionContext::setCanId(quint32 canId)
{
    m_canIds = {canId};
}

bool SelectionContext::hasSingleCanId() const
{
    return m_canIds.size() == 1;
}

quint32 SelectionContext::canId() const
{
    return hasSingleCanId() ? *m_canIds.constBegin() : 0;
}

void SelectionContext::clearCanIds()
{
    m_canIds.clear();
}

void SelectionContext::setFrameIndex(qint64 frameIndex)
{
    m_frameIndex = frameIndex;
}

qint64 SelectionContext::frameIndex() const
{
    return m_frameIndex;
}

bool SelectionContext::hasFrameIndex() const
{
    return m_frameIndex >= 0;
}

void SelectionContext::clearFrameIndex()
{
    m_frameIndex = -1;
}

void SelectionContext::setBitRange(int startBit, int bitLength)
{
    m_bitRange.startBit = startBit;
    m_bitRange.bitLength = bitLength;
}

const SelectionContext::BitRange &SelectionContext::bitRange() const
{
    return m_bitRange;
}

void SelectionContext::clearBitRange()
{
    m_bitRange.clear();
}

void SelectionContext::setTimeRange(qint64 startUsec, qint64 endUsec)
{
    m_timeRange.startUsec = startUsec;
    m_timeRange.endUsec = endUsec;
}

const SelectionContext::TimeRange &SelectionContext::timeRange() const
{
    return m_timeRange;
}

void SelectionContext::clearTimeRange()
{
    m_timeRange.clear();
}

void SelectionContext::setSignalId(const QString &signalId)
{
    m_signalId = signalId;
}

const QString &SelectionContext::signalId() const
{
    return m_signalId;
}

bool SelectionContext::hasSignal() const
{
    return !m_signalId.isEmpty();
}

void SelectionContext::clearSignal()
{
    m_signalId.clear();
}