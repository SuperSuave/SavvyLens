#ifndef ANALYSISMARKER_H
#define ANALYSISMARKER_H

#include <QtGlobal>
#include <QString>

/*
 * UI-neutral, capture-relative anchor for future analysis workflows.
 *
 * This type intentionally does not own a CANFrame, timestamp conversion,
 * playback state, persistence, selection state, or a marker collection.
 */
class AnalysisMarker
{
public:
    AnalysisMarker();

    explicit AnalysisMarker(quint64 frameIndex,
                            const QString &label = QString());

    bool isValid() const;

    quint64 frameIndex() const;
    const QString &label() const;

private:
    quint64 m_frameIndex;
    bool m_hasFrameIndex;
    QString m_label;
};

#endif // ANALYSISMARKER_H
