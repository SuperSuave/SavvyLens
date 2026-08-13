#ifndef FILTERUTILITY_H
#define FILTERUTILITY_H

// QT headers
#include <QListWidget>
#include <QSet>
#include <QString>

// C++ standard-library headers
#include <cstdint>

class FilterUtility
{

public:
    static constexpr int FilterIdRole = Qt::UserRole + 1;
    static QListWidgetItem *createFilterItem(uint32_t id, QListWidget *parent=NULL);   // if parent is given, add item automatically to listwidget
    static QListWidgetItem *createCheckableFilterItem(uint32_t id, bool checked, QListWidget* parent=NULL);
    static QListWidgetItem *createBusFilterItem(uint32_t id, QListWidget *parent=NULL);   // if parent is given, add item automatically to listwidget
    static QListWidgetItem *createCheckableBusFilterItem(uint32_t id, bool checked, QListWidget *parent=NULL);
    static QSet<uint32_t> getActiveIds(const QListWidget *filterList);

    static uint32_t getIdAsInt(QListWidgetItem *item);
    static QString getId(QListWidgetItem *item);
    static QString getId(QString itemText);
    

    static uint32_t getGMLanArbitrationId(int32_t id);
    static uint32_t getGMLanSenderId(int32_t id);
    static uint32_t getGMLanPriorityBits(int32_t id);

};

#endif // FILTERUTILITY_H
