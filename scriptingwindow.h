#ifndef SCRIPTINGWINDOW_H
#define SCRIPTINGWINDOW_H

#include "scriptcontainer.h"
#include "can_structs.h"
#include "connections/canconnection.h"
#include "jsedit.h"

#include <QDialog>
#include <QElapsedTimer>
#include <QList>
#include <QTimer>
#include <QVector>

class QListWidgetItem;
class QTableWidget;
class ScriptContainer;

namespace Ui
{
    class ScriptingWindow;
}

class ScriptingWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ScriptingWindow(const QVector<CANFrame> *frames,
                             QWidget *parent = nullptr);
    ~ScriptingWindow() override;

public slots:
    void log(QString text);

signals:
    void updateValueTable(QTableWidget *widget);
    void updatedParameter(QString name, QString value);

private slots:
    void loadNewScript();
    void createNewScript();
    void deleteCurrentScript();

    void refreshSourceWindow();
    void saveScript();
    void saveAsScript();
    void revertScript();
    void reloadScript();

    void validateCurrentScript();
    void runCurrentScript();
    void stopCurrentScript();
    void restartCurrentScript();
    void runAllScripts();
    void stopAllScripts();
    void setCurrentScriptEnabled(bool enabled);

    void changeCurrentScript();
    void newFrames(const CANConnection *connection,
                   const QVector<CANFrame> &frames);

    void clickedLogClear();
    void valuesTimerElapsed();
    void updatedValue(int row, int col);

    void scriptStateChanged();
    void scriptRuntimeError(const QString &phase, int line,
                            const QString &message, const QString &stack);
    void navigateToRuntimeError(QListWidgetItem *item);

private:
    enum LogItemDataRole
    {
        LogScriptPointerRole = Qt::UserRole,
        LogLineRole,
        LogIsRuntimeErrorRole
    };
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

    void readSettings();
    void writeSettings();
    void saveLog();

    bool eventFilter(QObject *obj, QEvent *event) override;

    void connectScriptContainer(ScriptContainer *container);
    void synchronizeCurrentScriptSource();

    void updateExecutionControls();
    void updateScriptListItem(ScriptContainer *container);
    void updateAllScriptListItems();

    ScriptContainer *selectedScript() const;
    QListWidgetItem *listItemForScript(ScriptContainer *container) const;

    Ui::ScriptingWindow *ui = nullptr;
    JSEdit *editor = nullptr;

    QList<ScriptContainer *> scripts;
    ScriptContainer *currentScript = nullptr;

    const QVector<CANFrame> *modelFrames = nullptr;

    QElapsedTimer elapsedTime;
    QTimer valuesTimer;
};

#endif // SCRIPTINGWINDOW_H