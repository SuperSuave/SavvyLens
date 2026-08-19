#ifndef ANALYSISMARKER_H
#define ANALYSISMARKER_H

// SavvyLens headers
#include "analysis/selectioncontext.h"

// Qt headers
#include <QString>
#include <QtGlobal>

/*
 * UI-neutral analysis marker.
 *
 * A marker may refer to a capture-relative frame index, a typed selection
 * context, or both. It does not own a collection, persistence, playback,
 * or UI state.
 */
class AnalysisMarker
{
public:
    AnalysisMarker();

    explicit AnalysisMarker(
        quint64 frameIndex,
        const QString &label = QString());

    explicit AnalysisMarker(
        const SelectionContext &selectionContext,
        const QString &label = QString());

    bool isValid() const;

    bool hasFrameIndex() const;
    quint64 frameIndex() const;

    bool hasSelectionContext() const;
    const SelectionContext &selectionContext() const;

    const QString &label() const;

private:
    quint64 m_frameIndex = 0;
    bool m_hasFrameIndex = false;

    SelectionContext m_selectionContext;
    QString m_label;
};

#endif // ANALYSISMARKER_H
