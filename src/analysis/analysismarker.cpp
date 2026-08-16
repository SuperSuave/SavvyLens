#include "analysismarker.h"

#include <QtGlobal>

AnalysisMarker::AnalysisMarker()
    : m_frameIndex(0), m_hasFrameIndex(false)
{
}

AnalysisMarker::AnalysisMarker(quint64 frameIndex, const QString &label)
    : m_frameIndex(frameIndex), m_hasFrameIndex(true), m_label(label)
{
}

bool AnalysisMarker::isValid() const
{
    return m_hasFrameIndex;
}

quint64 AnalysisMarker::frameIndex() const
{
    return m_frameIndex;
}

const QString &AnalysisMarker::label() const
{
    return m_label;
}
