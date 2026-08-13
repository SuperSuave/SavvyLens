#include "common/utility.h"
#include "filterutility.h"
#include "dbc/dbchandler.h"

#include <QSettings>
#include <QVariant>


uint32_t FilterUtility::getIdAsInt(QListWidgetItem *item)
{
    if (item == nullptr)
        return 0;

    const QVariant storedId = item->data(FilterIdRole);

    if (storedId.isValid())
        return storedId.toUInt();

    // Compatibility fallback for entries created before FilterIdRole existed.
    return Utility::ParseStringToNum(getId(item));
}


QString FilterUtility::getId(QString itemText)
{
    if (itemText.contains(" "))
    {
        // Strip away the filter label.
        return itemText.left(itemText.indexOf(" "));
    }

    return itemText;
}


QString FilterUtility::getId(QListWidgetItem *item)
{
    if (item == nullptr)
        return QString();

    return getId(item->text());
}


uint32_t FilterUtility::getGMLanArbitrationId(int32_t id)
{
    return (id >> 13) & 0x1FFF;
}


uint32_t FilterUtility::getGMLanPriorityBits(int32_t id)
{
    return (id >> 26) & 0x7;
}


uint32_t FilterUtility::getGMLanSenderId(int32_t id)
{
    return id & 0x1FFF;
}


QListWidgetItem *FilterUtility::createCheckableFilterItem(
    uint32_t id,
    bool checked,
    QListWidget *parent)
{
    QListWidgetItem *thisItem = createFilterItem(id, parent);

    thisItem->setFlags(
        thisItem->flags()
        | Qt::ItemIsUserCheckable
        | Qt::ItemIsEnabled
        | Qt::ItemIsSelectable);

    thisItem->setCheckState(checked ? Qt::Checked : Qt::Unchecked);

    return thisItem;
}


QListWidgetItem *FilterUtility::createCheckableBusFilterItem(
    uint32_t id,
    bool checked,
    QListWidget *parent)
{
    QListWidgetItem *thisItem = createBusFilterItem(id, parent);

    thisItem->setFlags(
        thisItem->flags()
        | Qt::ItemIsUserCheckable
        | Qt::ItemIsEnabled
        | Qt::ItemIsSelectable);

    thisItem->setCheckState(checked ? Qt::Checked : Qt::Unchecked);

    return thisItem;
}


QListWidgetItem *FilterUtility::createFilterItem(
    uint32_t id,
    QListWidget *parent)
{
    QSettings settings;
    DBCHandler *dbcHandler = DBCHandler::getReference();

    QListWidgetItem *thisItem = new QListWidgetItem(parent);

    // Store the canonical numeric ID separately from display text.
    thisItem->setData(FilterIdRole, QVariant::fromValue(id));

    QString filterItemName = Utility::formatCANID(id);

    // Note: There are multiple filter-labeling preferences. One global setting
    // enables/disables labels; each loaded DBC can also be enabled/disabled.
    if (settings.value("Main/FilterLabeling", false).toBool())
    {
        MatchingCriteria_t matchingCriteria;
        DBC_MESSAGE *msg = dbcHandler->findMessageForFilter(
            id,
            &matchingCriteria);

        if (msg != nullptr)
        {
            filterItemName.append(" ");
            filterItemName.append(msg->name);

            QString tooltip;

            if (matchingCriteria == GMLAN)
            {
                tooltip.append(
                    "0x"
                    + QString::number(
                        FilterUtility::getGMLanArbitrationId(id),
                        16)
                          .toUpper()
                          .rightJustified(4, '0')
                    + ": ");
            }

            tooltip.append(msg->name);
            thisItem->setToolTip(tooltip);
        }
    }

    thisItem->setText(filterItemName);

    return thisItem;
}


QListWidgetItem *FilterUtility::createBusFilterItem(
    uint32_t id,
    QListWidget *parent)
{
    QSettings settings;
    DBCHandler *dbcHandler = DBCHandler::getReference();

    QListWidgetItem *thisItem = new QListWidgetItem(parent);

    // Store the canonical numeric ID separately from display text.
    thisItem->setData(FilterIdRole, QVariant::fromValue(id));

    QString filterItemName = QStringLiteral("%1").arg(id);

    if (settings.value("Main/FilterLabeling", false).toBool())
    {
        MatchingCriteria_t matchingCriteria;
        DBC_MESSAGE *msg = dbcHandler->findMessageForFilter(
            id,
            &matchingCriteria);

        if (msg != nullptr)
        {
            filterItemName.append(" ");
            filterItemName.append(msg->name);

            QString tooltip;

            if (matchingCriteria == GMLAN)
            {
                tooltip.append(
                    "Arbitration ID: 0x"
                    + QString::number(
                        FilterUtility::getGMLanArbitrationId(id),
                        16)
                          .toUpper()
                          .rightJustified(4, '0'));
            }
            else
            {
                tooltip.append(msg->name);
            }

            thisItem->setToolTip(tooltip);
        }
    }

    thisItem->setText(filterItemName);

    return thisItem;
}


QSet<uint32_t> FilterUtility::getActiveIds(
    const QListWidget *filterList)
{
    QSet<uint32_t> activeIds;

    if (filterList == nullptr)
        return activeIds;

    for (int row = 0; row < filterList->count(); ++row)
    {
        QListWidgetItem *item = filterList->item(row);

        if (item == nullptr)
            continue;

        if (item->checkState() != Qt::Checked)
            continue;

        activeIds.insert(getIdAsInt(item));
    }

    return activeIds;
}