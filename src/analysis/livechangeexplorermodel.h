#ifndef LIVECHANGEEXPLORERMODEL_H
#define LIVECHANGEEXPLORERMODEL_H

// SavvyLens headers
#include "frameaggregatestore.h"
#include "selectioncontext.h"

// Qt headers
#include <QAbstractTableModel>
#include <QByteArray>
#include <QString>
#include <QVector>

// C++ standard-library headers
#include <cstdint>

class AnalysisSession;

class LiveChangeExplorerModel final : public QAbstractTableModel
{
    Q_OBJECT

    Q_PROPERTY(
        QString filterText
        READ filterText
        WRITE setFilterText
        NOTIFY filterTextChanged)

public:
    enum Column
    {
        BusColumn = 0,
        CanIdColumn,
        DirectionColumn,
        FormatColumn,
        FrameTypeColumn,
        CountColumn,
        RateColumn,
        LastSeenColumn,
        LatestPayloadColumn,
        ChangedBytesColumn,
        ColumnCount
    };
    Q_ENUM(Column)

    enum Role
    {
        BusRole = Qt::UserRole + 1,
        CanIdRole,
        CanIdTextRole,
        DirectionRole,
        DirectionTextRole,
        IsExtendedRole,
        FormatTextRole,
        FrameTypeRole,
        FrameTypeTextRole,
        OccurrenceCountRole,
        RateHzRole,
        RateTextRole,
        LastSeenAgeMillisecondsRole,
        LastSeenAgeTextRole,
        LatestPayloadRole,
        LatestPayloadTextRole,
        ChangedByteMaskRole,
        ChangedBitMaskRole,
        ChangedByteIndexesRole,
        HasPreviousRole,
        PayloadLengthChangedRole,
        PreviousPayloadLengthRole,
        LatestPayloadLengthRole,
        ChangedBytesTextRole
    };
    Q_ENUM(Role)

    struct Row
    {
        FrameAggregateKey key;
        quint64 occurrenceCount = 0;

        double rateHz = 0.0;
        bool hasRate = false;

        qint64 lastSeenAgeMilliseconds = -1;

        QByteArray latestPayload;

        QByteArray changedByteMask;
        QByteArray changedBitMask;

        bool hasPrevious = false;
        bool payloadLengthChanged = false;
        int previousPayloadLength = 0;
        int latestPayloadLength = 0;
    };

    explicit LiveChangeExplorerModel(
        const AnalysisSession &session,
        QObject *parent = nullptr);

    int rowCount(
        const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(
        const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(
        const QModelIndex &index,
        int role = Qt::DisplayRole) const override;

    QVariant headerData(
        int section,
        Qt::Orientation orientation,
        int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    SelectionContext selectionContextForRow(int row) const;

    QString filterText() const;
    void setFilterText(const QString &filterText);

signals:
    void filterTextChanged();

public slots:
    void refresh();
    void refreshActivityAges();
    void clear();

private:
    QString filterText_;
    const AnalysisSession &session_;
    QVector<Row> rows_;
};

#endif // LIVECHANGEEXPLORERMODEL_H
