#include "analysis/analysismarker.h"

#ifndef ANALYSISMARKERSTORE_H
#define ANALYSISMARKERSTORE_H

// Qt headers
#include <QVector>

class AnalysisMarkerStore
{
public:
    void addMarker(const AnalysisMarker &marker);

    const QVector<AnalysisMarker> &markers() const noexcept;

    int count() const noexcept;
    bool isEmpty() const noexcept;

    void clear() noexcept;

private:
    QVector<AnalysisMarker> markers_;
};

#endif // ANALYSISMARKERSTORE_H
