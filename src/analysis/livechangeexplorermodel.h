#pragma once

#include <QAbstractTableModel>
#include <QVector>

#include "analysissession.h"

class LiveChangeExplorerModel final : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column
    {
        BusColumn,
        CanIdColumn,
        DirectionColumn,
        FormatColumn,
        FrameTypeColumn,
        CountColumn,
        LatestPayloadColumn,
        ChangedBytesColumn,
        ColumnCount
    };
    Q_ENUM(Column)

    enum Role
    {
        BusRole = Qt::UserRole + 1,
        CanIdRole,
        DirectionRole,
        IsExtendedRole,
        FrameTypeRole,
        OccurrenceCountRole,
        LatestPayloadRole,
        ChangedByteMaskRole,
        ChangedBitMaskRole,
        HasPreviousRole,
        PayloadLengthChangedRole,
        PreviousPayloadLengthRole,
        LatestPayloadLengthRole
    };
    Q_ENUM(Role)

    explicit LiveChangeExplorerModel(
        const AnalysisSession &session,
        QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(
        int section,
        Qt::Orientation orientation,
        int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void refresh();
    void clear();

private:
    struct Row
    {
        FrameAggregateKey key;
        quint64 occurrenceCount = 0;
        QByteArray latestPayload;
        QByteArray changedByteMask;
        QByteArray changedBitMask;
        bool hasPrevious = false;
        bool payloadLengthChanged = false;
        int previousPayloadLength = 0;
        int latestPayloadLength = 0;
    };

    const AnalysisSession &session_;
    QVector<Row> rows_;
};
