#include "scripting/scriptcontainer.h"

// SavvyLens headers
#include "connections/canconmanager.h"
#include "scripting/scriptingwindow.h"

// QT headers
#include <QDebug>
#include <QJSValueIterator>
#include <QTableWidget>
#include <QRegularExpression>
#include <QSet>


/* -------------------------------------------------------------------------
 * CANScriptHelper
 * ------------------------------------------------------------------------- */

CANScriptHelper::CANScriptHelper(QJSEngine *engine, QObject *parent)
    : QObject(parent),
      scriptEngine(engine)
{
}

void CANScriptHelper::setRxCallback(QJSValue cb)
{
    gotFrameFunction = cb;
}

void CANScriptHelper::setFilter(QJSValue id, QJSValue mask, QJSValue bus)
{
    const uint32_t idVal = id.toUInt();
    const uint32_t maskVal = mask.toUInt();
    const int busVal = bus.toInt();

    CANFilter filter;
    filter.setFilter(idVal, maskVal, busVal);
    filters.append(filter);

    CANConManager::getInstance()->addTargettedFrame(busVal, idVal, maskVal, this);
}

void CANScriptHelper::clearFilters()
{
    for (CANFilter &filter : filters)
    {
        CANConManager::getInstance()->removeTargettedFrame(
            filter.bus, filter.ID, filter.mask, this);
    }

    filters.clear();
    gotFrameFunction = QJSValue();
}

void CANScriptHelper::sendFrame(QJSValue bus, QJSValue id,
                                QJSValue length, QJSValue data)
{
    if (!data.isArray())
    {
        qWarning() << "Script sendFrame data argument is not an array";
        return;
    }

    const int requestedLength = length.toInt();
    if (requestedLength < 0 || requestedLength > 64)
    {
        qWarning() << "Script sendFrame invalid payload length:" << requestedLength;
        return;
    }

    CANFrame frame;
    frame.setExtendedFrameFormat(false);
    frame.setFrameId(id.toUInt());

    QByteArray bytes(requestedLength, 0);
    for (int i = 0; i < bytes.length(); i++)
    {
        bytes[i] = static_cast<char>(data.property(i).toInt() & 0xFF);
    }

    frame.setPayload(bytes);
    frame.bus = bus.toUInt();

    if (frame.frameId() > 0x7FF)
    {
        frame.setExtendedFrameFormat(true);
    }

    CANConManager::getInstance()->sendFrame(frame);
}

void CANScriptHelper::gotTargettedFrame(const CANFrame &frame)
{
    if (!scriptEngine || !gotFrameFunction.isCallable())
    {
        return;
    }

    for (CANFilter &filter : filters)
    {
        if (!filter.checkFilter(frame.frameId(), frame.bus))
        {
            continue;
        }

        const QByteArray payload = frame.payload();
        QJSValue dataBytes = scriptEngine->newArray(payload.length());

        for (int i = 0; i < payload.length(); i++)
        {
            dataBytes.setProperty(
                i,
                QJSValue(static_cast<unsigned char>(payload.at(i))));
        }

        QJSValueList args;
        args << QJSValue(static_cast<int>(frame.bus));
        args << QJSValue(static_cast<double>(frame.frameId()));
        args << QJSValue(payload.length());
        args << dataBytes;

        const QJSValue result = gotFrameFunction.call(args);
        if (result.isError())
        {
            reportCallbackError(QStringLiteral("gotCANFrame"), result);
        }

        return;
    }
}

void CANScriptHelper::reportCallbackError(const QString &phase,
                                          const QJSValue &result)
{
    emit callbackError(
        phase,
        result.property(QStringLiteral("lineNumber")).toInt(),
        result.property(QStringLiteral("message")).toString(),
        result.property(QStringLiteral("stack")).toString());
}

/* -------------------------------------------------------------------------
 * ISOTPScriptHelper
 * ------------------------------------------------------------------------- */

ISOTPScriptHelper::ISOTPScriptHelper(QJSEngine *engine, QObject *parent)
    : QObject(parent),
      scriptEngine(engine),
      handler(new ISOTP_HANDLER)
{
    connect(handler, SIGNAL(newISOMessage(ISOTP_MESSAGE)),
            this, SLOT(newISOMessage(ISOTP_MESSAGE)));

    handler->setReception(true);
    handler->setFlowCtrl(true);
}

ISOTPScriptHelper::~ISOTPScriptHelper()
{
    if (handler)
    {
        handler->clearAllFilters();
        delete handler;
        handler = nullptr;
    }
}

void ISOTPScriptHelper::setRxCallback(QJSValue cb)
{
    gotFrameFunction = cb;
}

void ISOTPScriptHelper::clearFilters()
{
    if (handler)
    {
        handler->clearAllFilters();
    }

    gotFrameFunction = QJSValue();
}

void ISOTPScriptHelper::setFilter(QJSValue id, QJSValue mask, QJSValue bus)
{
    if (!handler)
    {
        return;
    }

    handler->addFilter(bus.toInt(), id.toUInt(), mask.toUInt());
}

void ISOTPScriptHelper::sendISOTP(QJSValue bus, QJSValue id,
                                  QJSValue length, QJSValue dataBytes)
{
    if (!handler || !dataBytes.isArray())
    {
        qWarning() << "Script sendISOTP requires an array payload";
        return;
    }

    const int requestedLength = length.toInt();
    if (requestedLength < 0)
    {
        qWarning() << "Script sendISOTP invalid payload length:" << requestedLength;
        return;
    }

    ISOTP_MESSAGE msg;
    msg.setExtendedFrameFormat(false);
    msg.setFrameId(id.toUInt());

    QByteArray dataArray(requestedLength, 0);
    for (int i = 0; i < dataArray.length(); i++)
    {
        dataArray[i] =
            static_cast<char>(dataBytes.property(i).toInt() & 0xFF);
    }

    msg.setPayload(dataArray);
    msg.bus = bus.toInt();

    if (msg.frameId() > 0x7FF)
    {
        msg.setExtendedFrameFormat(true);
    }

    handler->sendISOTPFrame(msg.bus, msg.frameId(), msg.payload());
}

void ISOTPScriptHelper::newISOMessage(ISOTP_MESSAGE msg)
{
    if (!scriptEngine || !gotFrameFunction.isCallable())
    {
        return;
    }

    const QByteArray payload = msg.payload();
    QJSValue dataBytes = scriptEngine->newArray(payload.length());

    for (int i = 0; i < payload.length(); i++)
    {
        dataBytes.setProperty(
            i,
            QJSValue(static_cast<unsigned char>(payload.at(i))));
    }

    QJSValueList args;
    args << QJSValue(static_cast<int>(msg.bus));
    args << QJSValue(static_cast<double>(msg.frameId()));
    args << QJSValue(payload.length());
    args << dataBytes;

    const QJSValue result = gotFrameFunction.call(args);
    if (result.isError())
    {
        reportCallbackError(QStringLiteral("gotISOTPMessage"), result);
    }
}

void ISOTPScriptHelper::reportCallbackError(const QString &phase,
                                            const QJSValue &result)
{
    emit callbackError(
        phase,
        result.property(QStringLiteral("lineNumber")).toInt(),
        result.property(QStringLiteral("message")).toString(),
        result.property(QStringLiteral("stack")).toString());
}

/* -------------------------------------------------------------------------
 * UDSScriptHelper
 * ------------------------------------------------------------------------- */

UDSScriptHelper::UDSScriptHelper(QJSEngine *engine, QObject *parent)
    : QObject(parent),
      scriptEngine(engine),
      handler(new UDS_HANDLER)
{
    connect(handler, SIGNAL(newUDSMessage(UDS_MESSAGE)),
            this, SLOT(newUDSMessage(UDS_MESSAGE)));

    handler->setReception(true);
    handler->setFlowCtrl(true);
}

UDSScriptHelper::~UDSScriptHelper()
{
    if (handler)
    {
        handler->clearAllFilters();
        delete handler;
        handler = nullptr;
    }
}

void UDSScriptHelper::setRxCallback(QJSValue cb)
{
    gotFrameFunction = cb;
}

void UDSScriptHelper::clearFilters()
{
    if (handler)
    {
        handler->clearAllFilters();
    }

    gotFrameFunction = QJSValue();
}

void UDSScriptHelper::setFilter(QJSValue id, QJSValue mask, QJSValue bus)
{
    if (!handler)
    {
        return;
    }

    handler->addFilter(bus.toInt(), id.toUInt(), mask.toUInt());
}

void UDSScriptHelper::sendUDS(QJSValue bus, QJSValue id,
                              QJSValue service, QJSValue sublen,
                              QJSValue subFunc, QJSValue length,
                              QJSValue dataBytes)
{
    if (!handler || !dataBytes.isArray())
    {
        qWarning() << "Script sendUDS requires an array payload";
        return;
    }

    const int requestedLength = length.toInt();
    if (requestedLength < 0)
    {
        qWarning() << "Script sendUDS invalid payload length:" << requestedLength;
        return;
    }

    UDS_MESSAGE msg;
    msg.setExtendedFrameFormat(false);
    msg.setFrameId(id.toUInt());
    msg.service = service.toUInt();
    msg.subFuncLen = sublen.toUInt();
    msg.subFunc = subFunc.toUInt();

    QByteArray dataArray(requestedLength, 0);
    for (int i = 0; i < dataArray.length(); i++)
    {
        dataArray[i] =
            static_cast<char>(dataBytes.property(i).toInt() & 0xFF);
    }

    msg.setPayload(dataArray);
    msg.bus = bus.toInt();

    if (msg.frameId() > 0x7FF)
    {
        msg.setExtendedFrameFormat(true);
    }

    handler->sendUDSFrame(msg);
}

void UDSScriptHelper::newUDSMessage(UDS_MESSAGE msg)
{
    if (!scriptEngine || !gotFrameFunction.isCallable())
    {
        return;
    }

    const QByteArray payload = msg.payload();
    QJSValue dataBytes = scriptEngine->newArray(payload.length());

    for (int i = 0; i < payload.length(); i++)
    {
        dataBytes.setProperty(
            i,
            QJSValue(static_cast<unsigned char>(payload.at(i))));
    }

    QJSValueList args;
    args << QJSValue(static_cast<int>(msg.bus));
    args << QJSValue(static_cast<double>(msg.frameId()));
    args << QJSValue(static_cast<int>(msg.service));
    args << QJSValue(static_cast<int>(msg.subFunc));
    args << QJSValue(payload.length());
    args << dataBytes;

    const QJSValue result = gotFrameFunction.call(args);
    if (result.isError())
    {
        reportCallbackError(QStringLiteral("gotUDSMessage"), result);
    }
}

void UDSScriptHelper::reportCallbackError(const QString &phase,
                                          const QJSValue &result)
{
    emit callbackError(
        phase,
        result.property(QStringLiteral("lineNumber")).toInt(),
        result.property(QStringLiteral("message")).toString(),
        result.property(QStringLiteral("stack")).toString());
}

/* -------------------------------------------------------------------------
 * ScriptContainer
 * ------------------------------------------------------------------------- */

ScriptContainer::ScriptContainer(QObject *parent)
    : QObject(parent)
{
    connect(&timer, &QTimer::timeout, this, &ScriptContainer::tick);
}

ScriptContainer::~ScriptContainer()
{
    timer.stop();
    tearDownRuntime();
}

void ScriptContainer::setScriptWindow(ScriptingWindow *win)
{
    window = win;
    connect(this, &ScriptContainer::sendLog, window, &ScriptingWindow::log);
}

ScriptContainer::ScriptRunState ScriptContainer::state() const
{
    return runState;
}

bool ScriptContainer::isRunning() const
{
    return runState == ScriptRunState::Running;
}

bool ScriptContainer::isEnabled() const
{
    return enabled;
}

QString ScriptContainer::stateText() const
{
    switch (runState)
    {
    case ScriptRunState::Stopped:
        return QStringLiteral("Stopped");

    case ScriptRunState::Starting:
        return QStringLiteral("Starting");

    case ScriptRunState::Running:
        return QStringLiteral("Running");

    case ScriptRunState::Stopping:
        return QStringLiteral("Stopping");

    case ScriptRunState::Error:
        return QStringLiteral("Error");

    case ScriptRunState::Disabled:
        return QStringLiteral("Disabled");
    }

    return QStringLiteral("Unknown");
}

bool ScriptContainer::validateScript(const QString &source)
{
    /*
     * QJSEngine has no checkSyntax() API. Evaluate source wrapped in an
     * immediately uncalled function instead: parsing occurs, but the
     * function body itself is not executed. Therefore host/can calls,
     * setup(), timers, filters, and transmit functions cannot run here.
     */
    QJSEngine validationEngine;

    const QString wrappedSource =
        QStringLiteral("(function() {\n%1\n});").arg(source);

    const QJSValue result =
        validationEngine.evaluate(wrappedSource, fileName);

    if (result.isError())
    {
        const int wrappedLine =
            result.property(QStringLiteral("lineNumber")).toInt();

        const int reportedSourceLine = qMax(1, wrappedLine - 1);

        const int sourceLineCount =
            qMax(1, source.count(QLatin1Char('\n')) + 1);

        const int sourceLine =
            qBound(1, reportedSourceLine, sourceLineCount);

        const QString message =
            result.property(QStringLiteral("message")).toString();

        const QString stack =
            result.property(QStringLiteral("stack")).toString();

        qDebug() << "QJS wrapped line:" << wrappedLine
                 << "reported source line:" << reportedSourceLine
                 << "navigated source line:" << sourceLine
                 << "source line count:" << sourceLineCount;

        emit runtimeError(
            QStringLiteral("validation"),
            sourceLine,
            message,
            stack);

        return false;
    }

    emit sendLog(QStringLiteral("Validation succeeded."));
    return true;
}

bool ScriptContainer::start(const QString &source)
{
    if (!enabled)
    {
        emit sendLog(QStringLiteral(
            "Script is disabled and cannot be started."));
        return false;
    }

    if (runState == ScriptRunState::Running ||
        runState == ScriptRunState::Starting ||
        runState == ScriptRunState::Stopping)
    {
        stop();
    }
    else if (scriptEngine || canHelper || isoHelper || udsHelper)
    {
        /*
         * Error state can retain a runtime until queued safe cleanup occurs.
         * Starting again must always begin from a known-clean state.
         */
        tearDownRuntime();
    }

    scriptText = source;

    /*
     * Populate and retain manifest values before a runtime exists. This lets
     * scripts be configured in the Public Variables table while stopped.
     */
    discoverPublicParameters(scriptText);

    setState(ScriptRunState::Starting);

    if (!createRuntime())
    {
        emit sendLog(QStringLiteral("Unable to create script runtime."));
        setState(ScriptRunState::Error);
        return false;
    }

    if (!evaluateAndInitialize(scriptText))
    {
        tearDownRuntime();
        setState(ScriptRunState::Error);
        return false;
    }

    /*
     * The script has declared its globals, but setup() has not run yet.
     * Apply values edited in the stopped-state Public Variables table before
     * setup() configures filters, schedules timers, or sends frames.
     */
    applyPublicParameterValues();

    if (!runSetup())
    {
        tearDownRuntime();
        setState(ScriptRunState::Error);
        return false;
    }

    setState(ScriptRunState::Running);
    emit sendLog(QStringLiteral("Script started."));
    return true;
}

void ScriptContainer::stop()
{
    if (runState == ScriptRunState::Disabled)
    {
        return;
    }

    const bool hadRuntime =
        scriptEngine || canHelper || isoHelper || udsHelper || timer.isActive();

    if (!hadRuntime)
    {
        if (runState != ScriptRunState::Stopped)
        {
            setState(ScriptRunState::Stopped);
        }
        return;
    }

    setState(ScriptRunState::Stopping);
    tearDownRuntime();
    setState(ScriptRunState::Stopped);
    emit sendLog(QStringLiteral("Script stopped."));
}

bool ScriptContainer::restart(const QString &source)
{
    if (!enabled)
    {
        emit sendLog(QStringLiteral(
            "Script is disabled and cannot be restarted."));
        return false;
    }

    stop();
    return start(source);
}

void ScriptContainer::setEnabled(bool value)
{
    if (enabled == value)
    {
        return;
    }

    if (!value)
    {
        stop();
        enabled = false;
        setState(ScriptRunState::Disabled);
        emit sendLog(QStringLiteral("Script disabled."));
        return;
    }

    enabled = true;
    setState(ScriptRunState::Stopped);
    emit sendLog(QStringLiteral("Script enabled."));
}

void ScriptContainer::setTickInterval(QJSValue interval)
{
    const int intervalValue = interval.toInt();

    if (intervalValue <= 0)
    {
        timer.stop();
        emit sendLog(QStringLiteral("Tick timer stopped."));
        return;
    }

    timer.setInterval(intervalValue);
    timer.start();
    emit sendLog(QStringLiteral("Tick timer set to %1 ms.")
                     .arg(intervalValue));
}

int ScriptContainer::scheduleOnce(QJSValue delayMs, QJSValue callback)
{
    const int delay = delayMs.toInt();

    if (delay < 0)
    {
        emit sendLog(QStringLiteral(
            "scheduleOnce requires a delay of zero milliseconds or more."));
        return 0;
    }

    if (!callback.isCallable())
    {
        emit sendLog(QStringLiteral(
            "scheduleOnce requires a callable JavaScript function."));
        return 0;
    }

    /*
     * A zero-delay QTimer is legal, but keep it asynchronous. This prevents
     * scenario code from recursively re-entering itself on the JS call stack.
     */
    return createScheduledTask(qMax(1, delay), false, callback);
}

int ScriptContainer::scheduleEvery(QJSValue intervalMs, QJSValue callback)
{
    const int interval = intervalMs.toInt();

    if (interval <= 0)
    {
        emit sendLog(QStringLiteral(
            "scheduleEvery requires an interval greater than zero."));
        return 0;
    }

    if (!callback.isCallable())
    {
        emit sendLog(QStringLiteral(
            "scheduleEvery requires a callable JavaScript function."));
        return 0;
    }

    return createScheduledTask(interval, true, callback);
}

bool ScriptContainer::cancelTask(QJSValue taskId)
{
    const int id = taskId.toInt();

    QTimer *timer = scheduledTimers.take(id);
    if (!timer)
    {
        return false;
    }

    scheduledCallbacks.remove(id);

    timer->stop();
    timer->deleteLater();

    emit sendLog(QStringLiteral("Cancelled scheduled task %1.").arg(id));
    return true;
}

int ScriptContainer::createScheduledTask(int intervalMs, bool repeating,
                                         const QJSValue &callback)
{
    /*
     * setup() is called while start() is still in the Starting state.
     * Accept scheduling there, but do not permit it while stopped, stopping,
     * disabled, or in an error state.
     *
     * The QTimer callback cannot run until control returns to Qt's event loop,
     * at which point start() has either set Running or torn the runtime down.
     */
    if ((runState != ScriptRunState::Starting &&
         runState != ScriptRunState::Running) ||
        !scriptEngine)
    {
        emit sendLog(QStringLiteral(
            "Cannot schedule work because the script is not starting or running."));
        return 0;
    }

    const int taskId = allocateScheduledTaskId();

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(!repeating);
    timer->setInterval(intervalMs);

    scheduledTimers.insert(taskId, timer);
    scheduledCallbacks.insert(taskId, callback);

    connect(timer, &QTimer::timeout, this,
            [this, taskId]()
            {
                invokeScheduledTask(taskId);
            });

    timer->start();

    emit sendLog(QStringLiteral("Scheduled %1 task %2 every %3 ms.")
                     .arg(repeating
                              ? QStringLiteral("repeating")
                              : QStringLiteral("one-shot"))
                     .arg(taskId)
                     .arg(intervalMs));

    return taskId;
}

void ScriptContainer::invokeScheduledTask(int taskId)
{
    if (runState != ScriptRunState::Running || !scriptEngine)
    {
        cancelTask(QJSValue(taskId));
        return;
    }

    QTimer *timer = scheduledTimers.value(taskId, nullptr);
    if (!timer)
    {
        return;
    }

    QJSValue callback = scheduledCallbacks.value(taskId);
    if (!callback.isCallable())
    {
        cancelTask(QJSValue(taskId));

        emit sendLog(QStringLiteral(
                         "Scheduled task %1 no longer has a callable callback.")
                         .arg(taskId));
        return;
    }

    if (timer->isSingleShot())
    {
        scheduledTimers.remove(taskId);
        scheduledCallbacks.remove(taskId);

        timer->stop();
        timer->deleteLater();
    }

    const QJSValue result = callback.call();
    if (result.isError())
    {
        handleHelperError(
            QStringLiteral("scheduled task"),
            result.property(QStringLiteral("lineNumber")).toInt(),
            result.property(QStringLiteral("message")).toString(),
            result.property(QStringLiteral("stack")).toString());
    }
}

void ScriptContainer::cancelAllScheduledTasks()
{
    for (QTimer *timer : scheduledTimers)
    {
        if (!timer)
        {
            continue;
        }

        timer->stop();
        timer->deleteLater();
    }

    scheduledTimers.clear();
    scheduledCallbacks.clear();
}

void ScriptContainer::requestStop()
{
    if (runState != ScriptRunState::Running)
    {
        return;
    }

    QTimer::singleShot(0, this, [this]()
                       {
        if (runState == ScriptRunState::Running)
        {
            stop();
        } });
}

int ScriptContainer::allocateScheduledTaskId()
{
    /*
     * Task ID zero is reserved as the JavaScript-visible failure value.
     * In normal use rollover is effectively impossible, but keep allocation
     * deterministic and collision-free.
     */
    while (nextScheduledTaskId == 0 ||
           scheduledTimers.contains(nextScheduledTaskId))
    {
        ++nextScheduledTaskId;
    }

    const int taskId = nextScheduledTaskId;
    ++nextScheduledTaskId;

    if (nextScheduledTaskId <= 0)
    {
        nextScheduledTaskId = 1;
    }

    return taskId;
}

void ScriptContainer::log(QJSValue logString)
{
    emit sendLog(logString.toString());
}

void ScriptContainer::addParameter(QJSValue name)
{
    const QString parameterName = name.toString();

    if (parameterName.isEmpty() || scriptParams.contains(parameterName))
    {
        return;
    }

    scriptParams.append(parameterName);
}

void ScriptContainer::discoverPublicParameters(const QString &source)
{
    /*
     * Supported syntax:
     *
     *   // @public bus = 0
     *   // @public id = 0x321
     *   // @public dataText = B4 5A 14 51 00 00 25 05
     *
     * This only reads source comments. It never evaluates JavaScript.
     */
    QStringList discoveredOrder;
    QMap<QString, QString> discoveredValues;
    QSet<QString> seenNames;

    const QStringList lines = source.split(QLatin1Char('\n'));

    for (const QString &sourceLine : lines)
    {
        QString line = sourceLine.trimmed();

        if (!line.startsWith(QStringLiteral("//")))
        {
            continue;
        }

        line.remove(0, 2);
        line = line.trimmed();

        if (!line.startsWith(QStringLiteral("@public")))
        {
            continue;
        }

        line.remove(0, QStringLiteral("@public").length());
        line = line.trimmed();

        const int equalsPosition = line.indexOf(QLatin1Char('='));

        if (equalsPosition <= 0)
        {
            qDebug() << "Ignoring malformed @public declaration:"
                     << sourceLine;
            continue;
        }

        const QString name =
            line.left(equalsPosition).trimmed();

        const QString defaultValue =
            line.mid(equalsPosition + 1).trimmed();

        const QRegularExpression validName(
            QStringLiteral("^[A-Za-z_$][A-Za-z0-9_$]*$"));

        if (!validName.match(name).hasMatch())
        {
            qDebug() << "Ignoring invalid @public name:"
                     << name;
            continue;
        }

        if (seenNames.contains(name))
        {
            qDebug() << "Ignoring duplicate @public name:"
                     << name;
            continue;
        }

        seenNames.insert(name);
        discoveredOrder.append(name);

        discoveredValues.insert(
            name,
            publicParameterValues.contains(name)
                ? publicParameterValues.value(name)
                : defaultValue);

        qDebug() << "Discovered public parameter:"
                 << name
                 << "="
                 << discoveredValues.value(name);
    }

    publicParameterOrder = discoveredOrder;
    publicParameterValues = discoveredValues;

    qDebug() << "Public parameter count:"
             << publicParameterOrder.count();
}

void ScriptContainer::updateValuesTable(QTableWidget *widget)
{
    if (!widget)
    {
        return;
    }

    /*
     * Manifest parameters are available even with no active QJSEngine.
     * Legacy host.addParameter(...) entries remain supported while running.
     */
    QStringList parameterNames = publicParameterOrder;

    for (const QString &legacyName : scriptParams)
    {
        if (!parameterNames.contains(legacyName))
        {
            parameterNames.append(legacyName);
        }
    }

    for (const QString &paramName : parameterNames)
    {
        QString value;

        if (publicParameterValues.contains(paramName))
        {
            value = publicParameterValues.value(paramName);
        }
        else if (scriptEngine)
        {
            value = scriptEngine->globalObject()
                        .property(paramName)
                        .toString();
        }

        bool found = false;

        for (int row = 0; row < widget->rowCount(); row++)
        {
            QTableWidgetItem *nameItem = widget->item(row, 0);

            if (!nameItem || nameItem->text() != paramName)
            {
                continue;
            }

            found = true;

            QTableWidgetItem *valueItem = widget->item(row, 1);

            if (valueItem && !valueItem->isSelected())
            {
                valueItem->setText(value);
            }

            break;
        }

        if (found)
        {
            continue;
        }

        const int row = widget->rowCount();
        widget->insertRow(row);

        QTableWidgetItem *nameItem =
            new QTableWidgetItem(paramName);

        nameItem->setFlags(Qt::ItemIsEnabled);

        widget->setItem(row, 0, nameItem);
        widget->setItem(row, 1, new QTableWidgetItem(value));
    }
}

void ScriptContainer::updateParameter(QString name, QString value)
{
    if (name.isEmpty())
    {
        return;
    }

    /*
     * Persist a manifest value whether or not JavaScript is currently
     * running. This is what makes stopped-state configuration possible.
     */
    if (publicParameterValues.contains(name))
    {
        publicParameterValues.insert(name, value);
    }

    /*
     * Preserve existing live-update behavior for running scripts.
     * Values are intentionally text; templates convert with Number(...)
     * or parse their hexadecimal payload text themselves.
     */
    if (scriptEngine)
    {
        scriptEngine->globalObject().setProperty(
            name,
            QJSValue(value));
    }
}

void ScriptContainer::tick()
{
    if (runState != ScriptRunState::Running || !tickFunction.isCallable())
    {
        return;
    }

    const QJSValue result = tickFunction.call();
    if (result.isError())
    {
        reportError(QStringLiteral("tick"), result);
        handleHelperError(
            QStringLiteral("tick"),
            result.property(QStringLiteral("lineNumber")).toInt(),
            result.property(QStringLiteral("message")).toString(),
            result.property(QStringLiteral("stack")).toString());
    }
}

void ScriptContainer::handleHelperError(const QString &phase,
                                        int line,
                                        const QString &message,
                                        const QString &stack)
{
    if (runState != ScriptRunState::Running &&
        runState != ScriptRunState::Starting)
    {
        return;
    }

    cancelAllScheduledTasks();

    emit runtimeError(phase, line, message, stack);

    emit sendLog(QStringLiteral("Error in %1 on line %2: %3")
                     .arg(phase)
                     .arg(line)
                     .arg(message));

    if (!stack.isEmpty())
    {
        emit sendLog(QStringLiteral("Stack: %1").arg(stack));
    }

    /*
     * Do not delete the helper while it is on the call stack inside a
     * gotCANFrame/newISOMessage/newUDSMessage callback. The queued teardown
     * occurs after that callback has returned to Qt's event loop.
     */
    setState(ScriptRunState::Error);

    QTimer::singleShot(0, this, [this]()
                       {
        if (runState != ScriptRunState::Error)
        {
            return;
        }

        tearDownRuntime();
        emit sendLog(QStringLiteral("Script stopped after runtime error.")); });
}

bool ScriptContainer::createRuntime()
{
    tearDownRuntime();

    scriptEngine = new QJSEngine(this);

    canHelper = new CANScriptHelper(scriptEngine, this);
    isoHelper = new ISOTPScriptHelper(scriptEngine, this);
    udsHelper = new UDSScriptHelper(scriptEngine, this);

    connect(canHelper, &CANScriptHelper::callbackError,
            this, &ScriptContainer::handleHelperError);

    connect(isoHelper, &ISOTPScriptHelper::callbackError,
            this, &ScriptContainer::handleHelperError);

    connect(udsHelper, &UDSScriptHelper::callbackError,
            this, &ScriptContainer::handleHelperError);

    /*
     * Publish host APIs before evaluate(). This permits ordinary JavaScript
     * initialization at top level, while setup() remains the recommended
     * lifecycle callback for scripts needing initialized runtime state.
     */
    scriptEngine->globalObject().setProperty(
        QStringLiteral("host"), scriptEngine->newQObject(this));

    scriptEngine->globalObject().setProperty(
        QStringLiteral("can"), scriptEngine->newQObject(canHelper));

    scriptEngine->globalObject().setProperty(
        QStringLiteral("isotp"), scriptEngine->newQObject(isoHelper));

    scriptEngine->globalObject().setProperty(
        QStringLiteral("uds"), scriptEngine->newQObject(udsHelper));

    return true;
}

void ScriptContainer::applyPublicParameterValues()
{
    if (!scriptEngine)
    {
        return;
    }

    QJSValue globalObject = scriptEngine->globalObject();

    for (const QString &name : publicParameterOrder)
    {
        globalObject.setProperty(
            name,
            QJSValue(publicParameterValues.value(name)));
    }
}

bool ScriptContainer::evaluateAndInitialize(const QString &source)
{
    if (!scriptEngine)
    {
        return false;
    }

    const QJSValue result = scriptEngine->evaluate(source, fileName);

    if (result.isError())
    {
        reportError(QStringLiteral("evaluation"), result);
        return false;
    }

    compiledScript = result;

    setupFunction =
        scriptEngine->globalObject().property(QStringLiteral("setup"));

    tickFunction =
        scriptEngine->globalObject().property(QStringLiteral("tick"));

    canHelper->setRxCallback(
        scriptEngine->globalObject().property(
            QStringLiteral("gotCANFrame")));

    isoHelper->setRxCallback(
        scriptEngine->globalObject().property(
            QStringLiteral("gotISOTPMessage")));

    udsHelper->setRxCallback(
        scriptEngine->globalObject().property(
            QStringLiteral("gotUDSMessage")));


    return true;
}

bool ScriptContainer::runSetup()
{
    if (!setupFunction.isCallable())
    {
        return true;
    }

    const QJSValue setupResult = setupFunction.call();

    if (setupResult.isError())
    {
        reportError(QStringLiteral("setup"), setupResult);
        return false;
    }

    return true;
}

void ScriptContainer::tearDownRuntime()
{
    /*
     * Emergency-brake ordering:
     * 1. Stop all future scenario callbacks.
     * 2. Remove CAN/ISO-TP/UDS subscriptions.
     * 3. Release QJSValue references.
     * 4. Destroy the QJSEngine.
     */
    cancelAllScheduledTasks();
    timer.stop();

    if (canHelper)
    {
        canHelper->clearFilters();
    }

    if (isoHelper)
    {
        isoHelper->clearFilters();
    }

    if (udsHelper)
    {
        udsHelper->clearFilters();
    }

    clearParameters();

    setupFunction = QJSValue();
    tickFunction = QJSValue();
    compiledScript = QJSValue();

    delete canHelper;
    canHelper = nullptr;

    delete isoHelper;
    isoHelper = nullptr;

    delete udsHelper;
    udsHelper = nullptr;

    delete scriptEngine;
    scriptEngine = nullptr;
}

void ScriptContainer::setState(ScriptRunState state)
{
    if (runState == state)
    {
        return;
    }

    runState = state;
    emit stateChanged(runState);
}

void ScriptContainer::reportError(const QString &phase,
                                  const QJSValue &result)
{
    const int line =
        result.property(QStringLiteral("lineNumber")).toInt();

    const QString message =
        result.property(QStringLiteral("message")).toString();

    const QString stack =
        result.property(QStringLiteral("stack")).toString();

    emit runtimeError(phase, line, message, stack);

    emit sendLog(QStringLiteral("Error in %1 on line %2: %3")
                     .arg(phase)
                     .arg(line)
                     .arg(message));

    if (!stack.isEmpty())
    {
        emit sendLog(QStringLiteral("Stack: %1").arg(stack));
    }
}

void ScriptContainer::clearParameters()
{
    scriptParams.clear();
}
