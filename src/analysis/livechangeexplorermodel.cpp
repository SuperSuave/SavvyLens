#include "livechangeexplorermodel.h"

// SavvyLens headers
#include "analysis/analysissession.h"
#include "analysis/framecomparison.h"

// Qt headers
#include <QCanBusFrame>
#include <QString>
#include <QVariantList>

// C++ standard-library headers
#include <algorithm>

namespace
{

    QString canIdText(std::uint32_t frameId)
    {
        return QStringLiteral("0x%1")
            .arg(frameId, 0, 16)
            .toUpper();
    }

    QString payloadText(const QByteArray &payload)
    {
        if (payload.isEmpty())
        {
            return QStringLiteral("—");
        }

        return QString::fromLatin1(payload.toHex(' ')).toUpper();
    }

    QString directionText(bool isReceived)
    {
        return isReceived
                   ? QStringLiteral("Rx")
                   : QStringLiteral("Tx");
    }

    QString formatText(bool hasExtendedFrameFormat)
    {
        return hasExtendedFrameFormat
                   ? QStringLiteral("Ext")
                   : QStringLiteral("Std");
    }

    QString frameTypeText(QCanBusFrame::FrameType frameType)
    {
        switch (frameType)
        {
        case QCanBusFrame::DataFrame:
            return QStringLiteral("Data");

        case QCanBusFrame::RemoteRequestFrame:
            return QStringLiteral("RTR");

        case QCanBusFrame::ErrorFrame:
            return QStringLiteral("Error");

        case QCanBusFrame::InvalidFrame:
            return QStringLiteral("Invalid");

        case QCanBusFrame::UnknownFrame:
        default:
            return QStringLiteral("Unknown");
        }
    }

    QString changedBytesText(const LiveChangeExplorerModel::Row &row)
    {
        if (!row.hasPrevious)
        {
            return QStringLiteral("—");
        }

        QStringList changedByteIndexes;

        for (int byteIndex = 0;
             byteIndex < row.changedByteMask.size();
             ++byteIndex)
        {
            if (row.changedByteMask.at(byteIndex) != 0)
            {
                changedByteIndexes.append(
                    QStringLiteral("%1")
                        .arg(byteIndex, 2, 16, QLatin1Char('0'))
                        .toUpper());
            }
        }

        QString result;

        if (row.payloadLengthChanged)
        {
            result = QStringLiteral("LEN %1→%2")
                         .arg(row.previousPayloadLength)
                         .arg(row.latestPayloadLength);
        }

        if (!changedByteIndexes.isEmpty())
        {
            if (!result.isEmpty())
            {
                result += QStringLiteral(" · ");
            }

            result += changedByteIndexes.join(QLatin1Char(' '));
        }

        if (result.isEmpty())
        {
            return QStringLiteral("None");
        }

        return result;
    }

    bool rowLessThan(
        const LiveChangeExplorerModel::Row &left,
        const LiveChangeExplorerModel::Row &right)
    {
        if (left.key.bus != right.key.bus)
        {
            return left.key.bus < right.key.bus;
        }

        if (left.key.frameId != right.key.frameId)
        {
            return left.key.frameId < right.key.frameId;
        }

        if (left.key.hasExtendedFrameFormat !=
            right.key.hasExtendedFrameFormat)
        {
            return left.key.hasExtendedFrameFormat <
                   right.key.hasExtendedFrameFormat;
        }

        if (left.key.frameType != right.key.frameType)
        {
            return static_cast<int>(left.key.frameType) <
                   static_cast<int>(right.key.frameType);
        }

        return left.key.isReceived < right.key.isReceived;
    }

} // namespace

LiveChangeExplorerModel::LiveChangeExplorerModel(
    const AnalysisSession &session,
    QObject *parent)
    : QAbstractTableModel(parent), session_(session)
{
}

int LiveChangeExplorerModel::rowCount(
    const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return rows_.size();
}

int LiveChangeExplorerModel::columnCount(
    const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return ColumnCount;
}

SelectionContext LiveChangeExplorerModel::selectionContextForRow(
    int row) const
{
    SelectionContext context;

    if (row < 0 || row >= rows_.size())
    {
        return context;
    }

    const Row &selectedRow = rows_.at(row);

    context.setCanId(selectedRow.key.frameId);
    context.setBus(selectedRow.key.bus);

    return context;
}

QVariant LiveChangeExplorerModel::data(
    const QModelIndex &index,
    int role) const
{
    if (!index.isValid() ||
        index.row() < 0 ||
        index.row() >= rows_.size())
    {
        return QVariant();
    }

    const Row &row = rows_.at(index.row());

    switch (role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
        case BusColumn:
            return row.key.bus;

        case CanIdColumn:
            return canIdText(row.key.frameId);

        case DirectionColumn:
            return directionText(row.key.isReceived);

        case FormatColumn:
            return formatText(row.key.hasExtendedFrameFormat);

        case FrameTypeColumn:
            return frameTypeText(row.key.frameType);

        case CountColumn:
            return QVariant::fromValue(row.occurrenceCount);

        case LatestPayloadColumn:
            return payloadText(row.latestPayload);

        case ChangedBytesColumn:
            return changedBytesText(row);

        default:
            return QVariant();
        }

    case BusRole:
        return row.key.bus;

    case CanIdRole:
        return row.key.frameId;

    case CanIdTextRole:
        return canIdText(row.key.frameId);

    case DirectionRole:
        return row.key.isReceived;

    case DirectionTextRole:
        return directionText(row.key.isReceived);

    case IsExtendedRole:
        return row.key.hasExtendedFrameFormat;

    case FormatTextRole:
        return formatText(row.key.hasExtendedFrameFormat);

    case FrameTypeRole:
        return static_cast<int>(row.key.frameType);

    case FrameTypeTextRole:
        return frameTypeText(row.key.frameType);

    case OccurrenceCountRole:
        return QVariant::fromValue(row.occurrenceCount);

    case LatestPayloadRole:
        return row.latestPayload;

    case LatestPayloadTextRole:
        return payloadText(row.latestPayload);

    case ChangedByteMaskRole:
        return row.changedByteMask;

    case ChangedBitMaskRole:
        return row.changedBitMask;
        
    case ChangedByteIndexesRole:
    {
        QVariantList indexes;

        for (int byteIndex = 0;
             byteIndex < row.changedByteMask.size();
             ++byteIndex)
        {
            if (row.changedByteMask.at(byteIndex) != 0)
            {
                indexes.append(byteIndex);
            }
        }

        return indexes;
    }

    case HasPreviousRole:
        return row.hasPrevious;

    case PayloadLengthChangedRole:
        return row.payloadLengthChanged;

    case PreviousPayloadLengthRole:
        return row.previousPayloadLength;

    case LatestPayloadLengthRole:
        return row.latestPayloadLength;

    case ChangedBytesTextRole:
        return changedBytesText(row);

    default:
        return QVariant();
    }
}

QVariant LiveChangeExplorerModel::headerData(
    int section,
    Qt::Orientation orientation,
    int role) const
{
    if (orientation != Qt::Horizontal ||
        role != Qt::DisplayRole)
    {
        return QVariant();
    }

    switch (section)
    {
    case BusColumn:
        return QStringLiteral("Bus");

    case CanIdColumn:
        return QStringLiteral("CAN ID");

    case DirectionColumn:
        return QStringLiteral("Direction");

    case FormatColumn:
        return QStringLiteral("Format");

    case FrameTypeColumn:
        return QStringLiteral("Type");

    case CountColumn:
        return QStringLiteral("Count");

    case LatestPayloadColumn:
        return QStringLiteral("Latest Payload");

    case ChangedBytesColumn:
        return QStringLiteral("Changed Bytes");

    default:
        return QVariant();
    }
}

QHash<int, QByteArray> LiveChangeExplorerModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles.insert(BusRole, "bus");
    roles.insert(CanIdRole, "canId");
    roles.insert(CanIdTextRole, "canIdText");
    roles.insert(DirectionRole, "isReceived");
    roles.insert(DirectionTextRole, "directionText");
    roles.insert(IsExtendedRole, "isExtended");
    roles.insert(FormatTextRole, "formatText");
    roles.insert(FrameTypeRole, "frameType");
    roles.insert(FrameTypeTextRole, "frameTypeText");
    roles.insert(OccurrenceCountRole, "occurrenceCount");
    roles.insert(LatestPayloadRole, "latestPayload");
    roles.insert(LatestPayloadTextRole, "latestPayloadText");
    roles.insert(ChangedByteMaskRole, "changedByteMask");
    roles.insert(ChangedBitMaskRole, "changedBitMask");
    roles.insert(ChangedByteIndexesRole, "changedByteIndexes");
    roles.insert(HasPreviousRole, "hasPrevious");
    roles.insert(PayloadLengthChangedRole, "payloadLengthChanged");
    roles.insert(PreviousPayloadLengthRole, "previousPayloadLength");
    roles.insert(LatestPayloadLengthRole, "latestPayloadLength");
    roles.insert(ChangedBytesTextRole, "changedBytesText");

    return roles;
}

void LiveChangeExplorerModel::refresh()
{
    QVector<Row> refreshedRows;

    const QVector<FrameAggregateKey> keys = session_.aggregateKeys();
    refreshedRows.reserve(keys.size());

    for (const FrameAggregateKey &key : keys)
    {
        const FrameAggregate *aggregate = session_.findAggregate(key);

        if (!aggregate)
        {
            continue;
        }

        Row row;
        row.key = aggregate->key;
        row.occurrenceCount = aggregate->occurrenceCount;
        row.latestPayload = aggregate->lastIngested.payload;
        row.latestPayloadLength = row.latestPayload.size();

        FrameComparison comparison;

        if (session_.compareLatest(key, &comparison))
        {
            row.hasPrevious = true;
            row.changedByteMask =
                comparison.payloadDiff.changedByteMask;
            row.changedBitMask =
                comparison.payloadDiff.changedBitMask;
            row.payloadLengthChanged =
                comparison.payloadDiff.lengthChanged;
            row.previousPayloadLength =
                comparison.payloadDiff.previousLength;
            row.latestPayloadLength =
                comparison.payloadDiff.currentLength;
        }

        refreshedRows.append(row);
    }

    std::sort(
        refreshedRows.begin(),
        refreshedRows.end(),
        rowLessThan);

    bool structureChanged = refreshedRows.size() != rows_.size();

    if (!structureChanged)
    {
        for (int rowIndex = 0;
             rowIndex < refreshedRows.size();
             ++rowIndex)
        {
            if (!(refreshedRows.at(rowIndex).key ==
                  rows_.at(rowIndex).key))
            {
                structureChanged = true;
                break;
            }
        }
    }

    if (structureChanged)
    {
        beginResetModel();
        rows_ = std::move(refreshedRows);
        endResetModel();
        return;
    }

    for (int rowIndex = 0;
         rowIndex < refreshedRows.size();
         ++rowIndex)
    {
        const Row &refreshedRow = refreshedRows.at(rowIndex);
        Row &currentRow = rows_[rowIndex];

        const bool rowChanged =
            currentRow.occurrenceCount != refreshedRow.occurrenceCount ||
            currentRow.latestPayload != refreshedRow.latestPayload ||
            currentRow.changedByteMask != refreshedRow.changedByteMask ||
            currentRow.changedBitMask != refreshedRow.changedBitMask ||
            currentRow.hasPrevious != refreshedRow.hasPrevious ||
            currentRow.payloadLengthChanged !=
                refreshedRow.payloadLengthChanged ||
            currentRow.previousPayloadLength !=
                refreshedRow.previousPayloadLength ||
            currentRow.latestPayloadLength !=
                refreshedRow.latestPayloadLength;

        if (!rowChanged)
        {
            continue;
        }

        currentRow = refreshedRow;

        const QModelIndex first = index(rowIndex, 0);
        const QModelIndex last = index(
            rowIndex,
            ColumnCount - 1);

        emit dataChanged(first, last);
    }
}

void LiveChangeExplorerModel::clear()
{
    beginResetModel();
    rows_.clear();
    endResetModel();
}
