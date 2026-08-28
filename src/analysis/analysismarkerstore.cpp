#include "analysismarkerstore.h"

void AnalysisMarkerStore::addMarker(const AnalysisMarker &marker)
{
    if (!marker.isValid())
    {
        return;
    }

    markers_.append(marker);
}

const QVector<AnalysisMarker> &AnalysisMarkerStore::markers() const noexcept
{
    return markers_;
}

int AnalysisMarkerStore::count() const noexcept
{
    return markers_.count();
}

bool AnalysisMarkerStore::isEmpty() const noexcept
{
    return markers_.isEmpty();
}

void AnalysisMarkerStore::clear() noexcept
{
    markers_.clear();
}
