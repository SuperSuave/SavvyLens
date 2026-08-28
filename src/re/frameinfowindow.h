#ifndef FRAMEINFOWINDOW_H
#define FRAMEINFOWINDOW_H

// SavvyLens headers
#include "analysis/selectioncontext.h"
#include "bus_protocols/j1939_handler.h"
#include "can/can_structs.h"
#include "common/utility.h"
#include "dbc/dbchandler.h"
#include "third_party/qcustomplot.h"
#include "widgets/candatagrid.h"

// QT headers
#include <QDialog>
#include <QFile>
#include <QListWidget>
#include <QTreeWidget>
#include <QPushButton>

namespace Ui {
class FrameInfoWindow;
}

class FrameInfoWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FrameInfoWindow(const QVector<CANFrame> *frames, QWidget *parent = 0);
    ~FrameInfoWindow();
    void showEvent(QShowEvent*);
    void selectID(QString idStr);
    QTreeWidget* getDetailsTree() const;
    void setSelectionContext(const SelectionContext &context);

private slots:
    void updateDetailsWindow(QString);
    void updatedFrames(int);
    void saveDetails();
    void mousePress();
    void mouseWheel();
    void mouseDoubleClick();
    void applyPlotTheme(QCustomPlot*);
    void resetByteGraph(int idx);
    void resetAllByteGraphs();
    void togglePlotType(int idx, bool scatter);
    void toggleAllPlotType(bool scatter);

private:
    Ui::FrameInfoWindow *ui;
    QCustomPlot *graphByte[8];
    QCustomPlot *graphHistogram;
    CANDataGrid *heatmap;

    QList<int> foundID;
    QList<CANFrame> frameCache;
    const QVector<CANFrame> *modelFrames;
    bool useOpenGL;
    bool useHexTicker;
    static QColor byteGraphColorForIndex(int idx);
    TimeStyle timeStyle;
    static QPen bytePens[8];
    DBCHandler *dbcHandler;

    QCPGraph *graphRef[8];
    QPushButton *btnResetByteGraph[8];
    QPushButton *btnTogglePlotType[8];

    void refreshIDList();
    void captureXRange(double &xmin, double &xmax);
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void setupByteGraph(QCustomPlot *plot, int num);
    void readSettings();
    void writeSettings();
    void dumpNode(QTreeWidgetItem* item, QFile *file, int indent);

};

#endif // FRAMEINFOWINDOW_H
