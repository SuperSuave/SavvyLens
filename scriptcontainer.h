#ifndef SCRIPTCONTAINER_H
#define SCRIPTCONTAINER_H

#include "can/can_structs.h"
#include "can/canfilter.h"
#include "bus_protocols/isotp_handler.h"
#include "bus_protocols/isotp_message.h"
#include "bus_protocols/uds_handler.h"

#include <QObject>
#include <QJSEngine>
#include <QJSValue>
#include <QTimer>
#include <QString>
#include <QStringList>
#include <QList>
#include <QVector>
#include <QHash>
#include <QMap>

class QTableWidget;
class ScriptingWindow;
class ScriptContainer;

class CANScriptHelper : public QObject
{
    Q_OBJECT

public:
    explicit CANScriptHelper(QJSEngine *engine, QObject *parent = nullptr);

public slots:
    void setFilter(QJSValue id, QJSValue mask, QJSValue bus);
    void clearFilters();
    void sendFrame(QJSValue bus, QJSValue id, QJSValue length, QJSValue data);
    void setRxCallback(QJSValue cb);

private slots:
    void gotTargettedFrame(const CANFrame &frame);

signals:
    void callbackError(const QString &phase, int line,
                       const QString &message, const QString &stack);

private:
    void reportCallbackError(const QString &phase, const QJSValue &result);

    QList<CANFilter> filters;
    QJSValue gotFrameFunction;
    QJSEngine *scriptEngine = nullptr;
};

class ISOTPScriptHelper : public QObject
{
    Q_OBJECT

public:
    explicit ISOTPScriptHelper(QJSEngine *engine, QObject *parent = nullptr);
    ~ISOTPScriptHelper() override;

public slots:
    void setFilter(QJSValue id, QJSValue mask, QJSValue bus);
    void clearFilters();
    void sendISOTP(QJSValue bus, QJSValue id, QJSValue length, QJSValue data);
    void setRxCallback(QJSValue cb);

private slots:
    void newISOMessage(ISOTP_MESSAGE msg);

signals:
    void callbackError(const QString &phase, int line,
                       const QString &message, const QString &stack);

private:
    void reportCallbackError(const QString &phase, const QJSValue &result);

    QJSValue gotFrameFunction;
    QJSEngine *scriptEngine = nullptr;
    ISOTP_HANDLER *handler = nullptr;
};

class UDSScriptHelper : public QObject
{
    Q_OBJECT

public:
    explicit UDSScriptHelper(QJSEngine *engine, QObject *parent = nullptr);
    ~UDSScriptHelper() override;

public slots:
    void setFilter(QJSValue id, QJSValue mask, QJSValue bus);
    void clearFilters();
    void sendUDS(QJSValue bus, QJSValue id, QJSValue service,
                 QJSValue sublen, QJSValue subFunc,
                 QJSValue length, QJSValue data);
    void setRxCallback(QJSValue cb);

private slots:
    void newUDSMessage(UDS_MESSAGE msg);

signals:
    void callbackError(const QString &phase, int line,
                       const QString &message, const QString &stack);

private:
    void reportCallbackError(const QString &phase, const QJSValue &result);

    QJSValue gotFrameFunction;
    QJSEngine *scriptEngine = nullptr;
    UDS_HANDLER *handler = nullptr;
};

class ScriptContainer : public QObject
{
    Q_OBJECT

public:
    enum class ScriptRunState
    {
        Stopped,
        Starting,
        Running,
        Stopping,
        Error,
        Disabled
    };
    Q_ENUM(ScriptRunState)

    explicit ScriptContainer(QObject *parent = nullptr);
    ~ScriptContainer() override;

    void setScriptWindow(ScriptingWindow *win);

    ScriptRunState state() const;
    bool isRunning() const;
    bool isEnabled() const;
    QString stateText() const;

    QString fileName;
    QString filePath;
    QString scriptText;

public slots:
    /*
     * Validation checks JavaScript syntax only. It must not execute the
     * script, register filters, call setup(), or begin a timer.
     */
    bool validateScript(const QString &source);

    /*
     * Starts a clean runtime using source. A running instance is stopped
     * first, then receives a brand-new QJSEngine and helper objects.
     */
    bool start(const QString &source);
    void stop();
    bool restart(const QString &source);
    void setEnabled(bool enabled);

    /*
     * These functions are exposed to JavaScript as host.<method>().
     */
    void setTickInterval(QJSValue interval);

    /*
     * Cancellable scenario scheduler exposed to JavaScript as:
     *
     *   host.scheduleOnce(delayMs, callback)
     *   host.scheduleEvery(intervalMs, callback)
     *   host.cancelTask(taskId)
     *
     * Task IDs are local to this ScriptContainer and are automatically
     * cancelled by stop() / tearDownRuntime().
     */
    int scheduleOnce(QJSValue delayMs, QJSValue callback);
    int scheduleEvery(QJSValue intervalMs, QJSValue callback);
    bool cancelTask(QJSValue taskId);
    void requestStop();

    void log(QJSValue logString);
    void addParameter(QJSValue name);

    /*
     * Parses // @public name = defaultValue declarations without evaluating
     * JavaScript. This provides stopped-state configuration in the UI.
     */
    void discoverPublicParameters(const QString &source);

    void updateValuesTable(QTableWidget *widget);
    void updateParameter(QString name, QString value);

signals:
    void sendLog(QString text);
    void stateChanged(ScriptContainer::ScriptRunState state);
    void runtimeError(const QString &phase, int line,
                      const QString &message, const QString &stack);

private slots:
    void tick();
    void handleHelperError(const QString &phase, int line,
                           const QString &message, const QString &stack);

private:
    bool createRuntime();
    bool evaluateAndInitialize(const QString &source);
    bool runSetup();
    void applyPublicParameterValues();
    int createScheduledTask(int intervalMs, bool repeating,
                            const QJSValue &callback);
    void invokeScheduledTask(int taskId);
    void cancelAllScheduledTasks();
    int allocateScheduledTaskId();
    void tearDownRuntime();

    void setState(ScriptRunState state);
    void reportError(const QString &phase, const QJSValue &result);
    void clearParameters();

    QJSEngine *scriptEngine = nullptr;
    QJSValue compiledScript;
    QJSValue setupFunction;
    QJSValue tickFunction;

    QTimer timer;
    ScriptingWindow *window = nullptr;

    CANScriptHelper *canHelper = nullptr;
    ISOTPScriptHelper *isoHelper = nullptr;
    UDSScriptHelper *udsHelper = nullptr;

    /*
     * Public parameter metadata is available while stopped. Values are stored
     * as text because the Public Variables table is a text editor; template
     * JavaScript converts numeric values with Number(...) as needed.
     */
    QStringList publicParameterOrder;
    QMap<QString, QString> publicParameterValues;

    QVector<QString> scriptParams;
    ScriptRunState runState = ScriptRunState::Stopped;
    
    bool enabled = true;

    /*
     * QTimer objects are parented to ScriptContainer. The callback values
     * are retained separately because QTimer only knows about its timeout
     * connection, not the JavaScript function it should invoke.
     */
    QHash<int, QTimer *> scheduledTimers;
    QHash<int, QJSValue> scheduledCallbacks;
    int nextScheduledTaskId = 1;
};

#endif // SCRIPTCONTAINER_H