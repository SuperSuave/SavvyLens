#ifndef FILECOMPARATORWINDOW_H
#define FILECOMPARATORWINDOW_H

// SavvyLens headers
#include "can/can_structs.h"
#include "common/utility.h"
#include "dbc/dbchandler.h"
#include "io/framefileio.h"

// QT headers
#include <QDialog>
#include <QDebug>
#include <QTreeWidget>
#include <QVector>

#include <array>

namespace Ui {
class FileComparatorWindow;
}

struct FrameData
{
    uint32_t ID = 0;
    int dataLen = 0; //longest payload seen for this ID
    QVector<uint8_t> bitmap; //per data byte, every bit that was ever set
    QVector<std::array<int, 256>> values; //first index is the data byte, second is # of times we saw that value
    QHash<QString, QList<QString>> signalInstances;

    void accumulate(const unsigned char *data, int len)
    {
        if (len > dataLen)
        {
            dataLen = len;
            bitmap.resize(len);
            while (values.size() < len) values.append(std::array<int, 256>{});
        }
        for (int y = 0; y < len; y++)
        {
            values[y][data[y]]++;
            bitmap[y] |= data[y];
        }
    }

    bool bitSet(int bit) const
    {
        const int byte = bit / 8;
        return byte < bitmap.size() && (bitmap[byte] & (1 << (bit % 8)));
    }

    int valueCount(int byte, int value) const
    {
        return byte < values.size() ? values[byte][value] : 0;
    }
};

class FileComparatorWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FileComparatorWindow(QWidget *parent = 0);
    ~FileComparatorWindow();

private slots:
    void loadInterestedFile();
    void loadReferenceFile();
    void clearReference();
    void saveDetails();

private:
    Ui::FileComparatorWindow *ui;
    QVector<CANFrame> interestedFrames;
    QVector<CANFrame> referenceFrames;
    QString interestedFilename;
    DBCHandler *dbcHandler;

    void calculateDetails();
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void readSettings();
    void writeSettings();
};

#endif // FILECOMPARATORWINDOW_H
