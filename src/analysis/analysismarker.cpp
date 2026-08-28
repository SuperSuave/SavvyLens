// SavvyLens headers
#include "analysismarker.h"

AnalysisMarker::AnalysisMarker() = default;

AnalysisMarker::AnalysisMarker(
    quint64 frameIndex,
    const QString &label)
    : m_frameIndex(frameIndex),
      m_hasFrameIndex(true),
      m_label(label)
{
}

AnalysisMarker::AnalysisMarker(
    const SelectionContext &selectionContext,
    const QString &label)
    : m_selectionContext(selectionContext),
      m_label(label)
{
}

bool AnalysisMarker::isValid() const
{
    return m_hasFrameIndex || !m_selectionContext.isEmpty();
}

bool AnalysisMarker::hasFrameIndex() const
{
    return m_hasFrameIndex;
}

quint64 AnalysisMarker::frameIndex() const
{
    return m_frameIndex;
}

bool AnalysisMarker::hasSelectionContext() const
{
    return !m_selectionContext.isEmpty();
}

const SelectionContext &AnalysisMarker::selectionContext() const
{
    return m_selectionContext;
}

const QString &AnalysisMarker::label() const
{
    return m_label;
}
