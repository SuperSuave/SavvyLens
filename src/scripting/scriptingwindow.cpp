#include "scripting/scriptingwindow.h"
#include "ui_scriptingwindow.h"

// SavvyLens headers
#include "themes/thememanager.h"
#include "common/utility.h"

// QT headers
#include <QAction>
#include <QCheckBox>
#include <QColor>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QListWidgetItem>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QSignalBlocker>
#include <QStandardPaths>
#include <QTableWidget>
#include <QTextBlock>
#include <QTextCursor>
#include <QToolButton>

// C++ standard-library headers
#include <algorithm>

#if QT_VERSION >= QT_VERSION_CHECK( 5, 10, 0 )
#include <QtCore/QRandomGenerator>
#endif

#include "connections/canconmanager.h"
#include "app/helpwindow.h"

ScriptingWindow::ScriptingWindow(const QVector<CANFrame> *frames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScriptingWindow)
{
    ui->setupUi(this);

    ui->listLog->setStyleSheet(ThemeManager::logListStyleSheet());
    setWindowFlags(Qt::Window);

    editor = new JSEdit();

    const ThemeColors theme = ThemeManager::colors();

    QColor currentLine = theme.accent;
    currentLine.setAlpha(48);

    QColor bracketMatch = theme.trace[1];
    bracketMatch.setAlpha(120);

    QColor bracketError = theme.trace[3];
    bracketError.setAlpha(120);

    editor->setColor(JSEdit::Background, theme.viewBg);
    editor->setColor(JSEdit::Normal, theme.text);
    editor->setColor(JSEdit::Comment, theme.mutedText);
    editor->setColor(JSEdit::Number, theme.trace[1]);
    editor->setColor(JSEdit::String, theme.trace[3]);
    editor->setColor(JSEdit::Operator, theme.trace[2]);
    editor->setColor(JSEdit::Identifier, theme.text);
    editor->setColor(JSEdit::Keyword, theme.trace[5]);
    editor->setColor(JSEdit::BuiltIn, theme.trace[0]);
    editor->setColor(JSEdit::Custom, theme.trace[6]);

    editor->setColor(JSEdit::BracketMatch, bracketMatch);
    editor->setColor(JSEdit::BracketError, bracketError);

    editor->setColor(JSEdit::Sidebar, theme.panelBg);
    editor->setColor(JSEdit::LineNumber, theme.mutedText);
    editor->setColor(JSEdit::Cursor, currentLine);

    editor->setFrameShape(JSEdit::NoFrame);
    editor->setWordWrapMode(QTextOption::NoWrap);
    editor->setEnabled(false);
    editor->setFont(QFont("Monospace", 12));
    editor->show();

    //Show whitespaces
    QTextOption option = editor->document()->defaultTextOption();
    option.setFlags(QTextOption::ShowTabsAndSpaces);
    editor->document()->setDefaultTextOption(option);

    ui->verticalLayout->insertWidget(2,editor, 10);

    readSettings();

    modelFrames = frames;

    templateMenu = new QMenu(ui->btnInsertTemplate);

    ui->btnInsertTemplate->setMenu(templateMenu);
    connect(templateMenu, &QMenu::aboutToShow, this, &ScriptingWindow::rebuildTemplateMenu);

    rebuildTemplateMenu();

    connect(ui->btnLoadScript, &QAbstractButton::pressed, this, &ScriptingWindow::loadNewScript);
    connect(ui->btnNewScript, &QAbstractButton::pressed, this, &ScriptingWindow::createNewScript);
    connect(ui->btnRemoveScript, &QAbstractButton::pressed, this, &ScriptingWindow::deleteCurrentScript);
    connect(ui->btnSaveScript, &QAbstractButton::pressed, this, &ScriptingWindow::saveScript);
    connect(ui->btnSaveAsScript, &QAbstractButton::pressed, this, &ScriptingWindow::saveAsScript);
    connect(ui->btnRevertScript, &QAbstractButton::pressed, this, &ScriptingWindow::revertScript);
    connect(ui->btnReloadScript, &QAbstractButton::pressed, this, &ScriptingWindow::reloadScript);
    connect(ui->btnValidateScript, &QAbstractButton::pressed, this, &ScriptingWindow::validateCurrentScript);
    connect(ui->btnRunScript, &QAbstractButton::pressed, this, &ScriptingWindow::runCurrentScript);
    connect(ui->btnStopScript, &QAbstractButton::pressed, this, &ScriptingWindow::stopCurrentScript);
    connect(ui->btnRestartScript, &QAbstractButton::pressed, this, &ScriptingWindow::restartCurrentScript);
    connect(ui->btnRunAllScripts, &QAbstractButton::pressed, this, &ScriptingWindow::runAllScripts);
    connect(ui->btnStopAllScripts, &QAbstractButton::pressed, this, &ScriptingWindow::stopAllScripts);
    connect(ui->cbScriptEnabled, &QCheckBox::toggled, this, &ScriptingWindow::setCurrentScriptEnabled);
    connect(ui->btnClearLog, &QAbstractButton::pressed, this, &ScriptingWindow::clickedLogClear);
    connect(ui->btnSaveLog, &QAbstractButton::pressed, this, &ScriptingWindow::saveLog);
    connect(ui->listLoadedScripts, &QListWidget::currentRowChanged, this, &ScriptingWindow::changeCurrentScript);
    connect(ui->tableVariables, SIGNAL(cellChanged(int, int)), this, SLOT(updatedValue(int, int)));
    connect(ui->listLog, &QListWidget::itemDoubleClicked, this, &ScriptingWindow::navigateToRuntimeError);
    connect(CANConManager::getInstance(), &CANConManager::framesReceived, this, &ScriptingWindow::newFrames);
    connect(&valuesTimer, &QTimer::timeout, this, &ScriptingWindow::valuesTimerElapsed);
    connect(CANConManager::getInstance(), &CANConManager::framesReceived, this, &ScriptingWindow::newFrames);
    connect(&valuesTimer, SIGNAL(timeout()), this, SLOT(valuesTimerElapsed()));
    currentScript = nullptr;

    elapsedTime.start();
    valuesTimer.start(1000);

    ui->tableVariables->insertColumn(0);
    ui->tableVariables->insertColumn(1);

    updateExecutionControls();
}

ScriptingWindow::~ScriptingWindow()
{
    stopAllScripts();

    qDeleteAll(scripts);
    scripts.clear();

    delete editor;
    delete ui;
}

void ScriptingWindow::newFrames(const CANConnection* pConn, const QVector<CANFrame>& pFrames)
{
    /*FIXME: name of the probe and bus should be checked */
    Q_UNUSED(pConn);
    Q_UNUSED(pFrames);

    /*for (int j = 0; j < scripts.length(); j++)
    {
        foreach(const CANFrame& frame, pFrames)
        {
            //scripts[j]->gotFrame(frame);
        }
    }*/
}

void ScriptingWindow::updatedValue(int row, int col)
{
    QTableWidgetItem *nameItem = ui->tableVariables->item(row, 0);
    QTableWidgetItem *valItem = ui->tableVariables->item(row, 1);

    if (!valItem) return;
    if (col == 0) return;
    if (!valItem->isSelected()) return; // don't record updates not from a user edited cell

    if (nameItem && valItem)
    {
        QString name = nameItem->text();
        QString val = valItem->text();
        emit updatedParameter(name, val);
    }
}

void ScriptingWindow::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    installEventFilter(this);
}

void ScriptingWindow::closeEvent(QCloseEvent *event)
{
    /*
     * Closing the scripting window is a safety stop: hidden windows must
     * not leave background scripts transmitting or reacting to CAN traffic.
     */
    stopAllScripts();

    removeEventFilter(this);
    writeSettings();

    QDialog::closeEvent(event);
}

bool ScriptingWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        switch (keyEvent->key())
        {
        case Qt::Key_F1:
            HelpWindow::getRef()->showHelp("scriptingwindow.md");
            break;
        }
        return true;
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
    return false;
}

void ScriptingWindow::readSettings()
{
    QSettings settings;
    if (settings.value("Main/SaveRestorePositions", false).toBool())
    {
        resize(settings.value("ScriptingWindow/WindowSize", QSize(860, 650)).toSize());
        move(Utility::constrainedWindowPos(settings.value("ScriptingWindow/WindowPos", QPoint(100, 100)).toPoint()));
    }
}

void ScriptingWindow::writeSettings()
{
    QSettings settings;

    if (settings.value("Main/SaveRestorePositions", false).toBool())
    {
        settings.setValue("ScriptingWindow/WindowSize", size());
        settings.setValue("ScriptingWindow/WindowPos", pos());
    }
}

void ScriptingWindow::changeCurrentScript()
{
    synchronizeCurrentScriptSource();

    if (currentScript)
    {
        disconnect(this, SIGNAL(updateValueTable(QTableWidget *)),
                   currentScript, SLOT(updateValuesTable(QTableWidget *)));

        disconnect(this, SIGNAL(updatedParameter(QString, QString)),
                   currentScript, SLOT(updateParameter(QString, QString)));
    }

    ui->tableVariables->clearContents();
    ui->tableVariables->setRowCount(0);
    ui->tableVariables->setHorizontalHeaderLabels(
        QStringList() << QStringLiteral("Parameter")
                      << QStringLiteral("Value"));

    currentScript = selectedScript();

    if (!currentScript)
    {
        editor->clear();
        editor->setEnabled(false);
        updateExecutionControls();
        return;
    }

    editor->setPlainText(currentScript->scriptText);
    editor->setEnabled(true);

    connect(this, SIGNAL(updateValueTable(QTableWidget *)),
            currentScript, SLOT(updateValuesTable(QTableWidget *)));

    connect(this, SIGNAL(updatedParameter(QString, QString)),
            currentScript, SLOT(updateParameter(QString, QString)));

    /*
     * Populate Public Variables immediately for a stopped script rather than
     * waiting for the one-second runtime values timer.
     */
    refreshCurrentPublicVariables();

    updateExecutionControls();
}

void ScriptingWindow::valuesTimerElapsed()
{
    if (currentScript)
    {
        emit updateValueTable(ui->tableVariables);
    }
}

void ScriptingWindow::loadNewScript()
{
    QString filename;
    QFileDialog dialog;
    QSettings settings;
    ScriptContainer *container;

    QStringList filters;
    filters.append(QString(tr("Javascript File (*.js)")));

    dialog.setDirectory(settings.value("ScriptingWindow/LoadSaveDirectory", dialog.directory().path()).toString());
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilters(filters);
    dialog.setViewMode(QFileDialog::Detail);

    if (dialog.exec() == QDialog::Accepted)
    {
        filename = dialog.selectedFiles()[0];

        if (dialog.selectedNameFilter() == filters[0])
        {
            QFile scriptFile(filename);

            if (scriptFile.open(QIODevice::ReadOnly))
            {
                const QString contents = QString::fromUtf8(scriptFile.readAll());
                scriptFile.close();

                const QString justFileName = QFileInfo(filename).fileName();

                container = new ScriptContainer(this);
                container->fileName = justFileName;
                container->filePath = filename;
                container->scriptText = contents;

                connectScriptContainer(container);
                scripts.append(container);

                new QListWidgetItem(container->fileName, ui->listLoadedScripts);

                ui->listLoadedScripts->setCurrentRow(
                    ui->listLoadedScripts->count() - 1);

                updateScriptListItem(container);
                updateExecutionControls();

                settings.setValue(
                    "ScriptingWindow/LoadSaveDirectory",
                    dialog.directory().path());
            }
        }
    }
}

void ScriptingWindow::createNewScript()
{
    ScriptContainer *container = new ScriptContainer(this);

    QString randomPart;
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
    randomPart = QString::number(qrand() % 10000);
#else
    randomPart = QString::number(
        QRandomGenerator::global()->bounded(10000));
#endif

    container->fileName =
        QStringLiteral("UNNAMED_%1.js").arg(randomPart);

    container->filePath.clear();
    container->scriptText.clear();

    connectScriptContainer(container);
    scripts.append(container);

    new QListWidgetItem(
        container->fileName,
        ui->listLoadedScripts);

    ui->listLoadedScripts->setCurrentRow(
        ui->listLoadedScripts->count() - 1);

    updateScriptListItem(container);
    updateExecutionControls();
}

void ScriptingWindow::deleteCurrentScript()
{
    const int row = ui->listLoadedScripts->currentRow();
    ScriptContainer *container = selectedScript();

    if (row < 0 || !container)
    {
        return;
    }

    const QMessageBox::StandardButton answer =
        QMessageBox::question(
            this,
            tr("Remove Script"),
            tr("Remove \"%1\"?").arg(container->fileName),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No);

    if (answer != QMessageBox::Yes)
    {
        return;
    }

    /*
     * Stop first: this unregisters CAN filters and tears down all script
     * callbacks before the container is destroyed.
     */
    container->stop();

    delete ui->listLoadedScripts->takeItem(row);
    scripts.removeAt(row);

    currentScript = nullptr;
    delete container;

    if (!scripts.isEmpty())
    {
        ui->listLoadedScripts->setCurrentRow(
            qMin(row, scripts.count() - 1));
    }
    else
    {
        editor->clear();
        editor->setEnabled(false);
        ui->tableVariables->clearContents();
        ui->tableVariables->setRowCount(0);
    }

    updateExecutionControls();
}

void ScriptingWindow::refreshSourceWindow()
{
    editor->setPlainText(currentScript->scriptText);
}

void ScriptingWindow::saveScript()
{
    QFile *outFile = new QFile(currentScript->filePath);

    if (!outFile->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        delete outFile;
        return;
    }
    outFile->write(editor->toPlainText().toUtf8());
    currentScript->scriptText = editor->toPlainText();
    outFile->close();
    delete outFile;
}

void ScriptingWindow::saveAsScript()
{
    QString filename;
    QFileDialog dialog(this);
    QSettings settings;

    QStringList filters;
    filters.append(QString(tr("Javascript File (*.js)")));

    dialog.setDirectory(settings.value("ScriptingWindow/LoadSaveDirectory", dialog.directory().path()).toString());
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilters(filters);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    if (dialog.exec() == QDialog::Accepted)
    {
        filename = dialog.selectedFiles()[0];
        if (!filename.contains('.')) filename += ".js";
        if (dialog.selectedNameFilter() == filters[0])
        {
            QFile *outFile = new QFile(filename);

            if (!outFile->open(QIODevice::WriteOnly | QIODevice::Text))
            {
                delete outFile;
                return;
            }
            outFile->write(editor->toPlainText().toUtf8());
            outFile->close();
            delete outFile;
            settings.setValue("ScriptingWindow/LoadSaveDirectory", dialog.directory().path());
        }
    }
}

void ScriptingWindow::revertScript()
{
    QMessageBox msgBox;
    msgBox.setText("Are you sure you'd like to revert?");
    msgBox.setInformativeText("Really do it?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    switch (ret)
    {
    case QMessageBox::Yes:
        //just grab the last stored copy of the script (last compiled version) and replace current text with that text
        editor->setPlainText(currentScript->scriptText);
        break;
    case QMessageBox::No:
        break;
    default:
        // should never be reached
        break;
    }
}

void ScriptingWindow::reloadScript()
{
    if (!currentScript || currentScript->filePath.isEmpty())
    {
        return;
    }

    const QMessageBox::StandardButton answer =
        QMessageBox::question(
            this,
            tr("Reload Script"),
            tr("Reload \"%1\" from disk?\n"
               "A currently running instance will continue using its "
               "current runtime until you explicitly Restart it.")
                .arg(currentScript->fileName),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No);

    if (answer != QMessageBox::Yes)
    {
        return;
    }

    QFile scriptFile(currentScript->filePath);
    if (!scriptFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        log(tr("Unable to reload %1.").arg(currentScript->fileName));
        return;
    }

    const QString contents =
        QString::fromUtf8(scriptFile.readAll());

    currentScript->scriptText = contents;
    editor->setPlainText(contents);

    refreshCurrentPublicVariables();

    log(tr("Reloaded %1. Use Restart to apply it to a running script.")
            .arg(currentScript->fileName));
}

void ScriptingWindow::validateCurrentScript()
{
    ScriptContainer *container = selectedScript();
    if (!container)
    {
        return;
    }

    synchronizeCurrentScriptSource();
    container->validateScript(container->scriptText);
}

void ScriptingWindow::runCurrentScript()
{
    ScriptContainer *container = selectedScript();
    if (!container)
    {
        return;
    }

    synchronizeCurrentScriptSource();
    container->start(container->scriptText);

    updateExecutionControls();
}

void ScriptingWindow::stopCurrentScript()
{
    ScriptContainer *container = selectedScript();
    if (!container)
    {
        return;
    }

    container->stop();
    updateExecutionControls();
}

void ScriptingWindow::restartCurrentScript()
{
    ScriptContainer *container = selectedScript();
    if (!container)
    {
        return;
    }

    synchronizeCurrentScriptSource();
    container->restart(container->scriptText);

    updateExecutionControls();
}

void ScriptingWindow::runAllScripts()
{
    synchronizeCurrentScriptSource();

    for (ScriptContainer *container : scripts)
    {
        if (!container->isEnabled() || container->isRunning())
        {
            continue;
        }

        container->start(container->scriptText);
    }

    updateExecutionControls();
}

void ScriptingWindow::stopAllScripts()
{
    for (ScriptContainer *container : scripts)
    {
        container->stop();
    }

    updateExecutionControls();
}

void ScriptingWindow::setCurrentScriptEnabled(bool enabled)
{
    ScriptContainer *container = selectedScript();
    if (!container)
    {
        return;
    }

    container->setEnabled(enabled);
    updateExecutionControls();
}

void ScriptingWindow::clickedLogClear()
{
    ui->listLog->clear();
    elapsedTime.start();
}

void ScriptingWindow::saveLog()
{
    QString filename;
    QFileDialog dialog(this);
    QSettings settings;

    QStringList filters;
    filters.append(QString(tr("Log File (*.log)")));

    dialog.setDirectory(settings.value("ScriptingWindow/LoadSaveDirectory", dialog.directory().path()).toString());
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilters(filters);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    if (dialog.exec() == QDialog::Accepted)
    {
        filename = dialog.selectedFiles()[0];
        if (!filename.contains('.')) filename += ".log";
        if (dialog.selectedNameFilter() == filters[0])
        {
            QFile *outFile = new QFile(filename);

            if (!outFile->open(QIODevice::WriteOnly | QIODevice::Text))
            {
                delete outFile;
                return;
            }
            int c = ui->listLog ->count();
            for (int row = 0; row < c; row++) {
                outFile->write(ui->listLog->item(row)->data(Qt::DisplayRole).toString().toUtf8() + "\n");
            }
            outFile->close();
            delete outFile;
            settings.setValue("ScriptingWindow/LoadSaveDirectory", dialog.directory().path());
        }
    }
}

void ScriptingWindow::log(QString text)
{
    ScriptContainer *cont = qobject_cast<ScriptContainer*>(sender());
    if (cont != nullptr)
       ui->listLog->addItem(QString::number(elapsedTime.elapsed()) + "(" + cont->fileName + "): " + text);
    else
       ui->listLog->addItem(QString::number(elapsedTime.elapsed()) + ": " + text);

    if (ui->cbAutoScroll->isChecked())
    {
        ui->listLog->scrollToBottom();
    }
}

ScriptContainer *ScriptingWindow::selectedScript() const
{
    const int row = ui->listLoadedScripts->currentRow();

    if (row < 0 || row >= scripts.count())
    {
        return nullptr;
    }

    return scripts.at(row);
}

QListWidgetItem *ScriptingWindow::listItemForScript(
    ScriptContainer *container) const
{
    const int index = scripts.indexOf(container);

    if (index < 0)
    {
        return nullptr;
    }

    return ui->listLoadedScripts->item(index);
}

void ScriptingWindow::synchronizeCurrentScriptSource()
{
    if (!currentScript || !editor)
    {
        return;
    }

    currentScript->scriptText = editor->toPlainText();

    /*
     * Re-scan manifest comments whenever the current source is synchronized.
     * This updates defaults or added/removed @public declarations without
     * ever evaluating the script.
     */
    refreshCurrentPublicVariables();
}

void ScriptingWindow::refreshCurrentPublicVariables()
{
    if (!currentScript || !ui || !ui->tableVariables)
    {
        // qDebug() << "Cannot refresh public variables:"
        //          << "currentScript=" << currentScript
        //          << "ui=" << ui;
        return;
    }

    // qDebug() << "Refreshing public variables for:"
    //          << currentScript->fileName;

    // qDebug() << "Script source:"
    //          << currentScript->scriptText;

    currentScript->discoverPublicParameters(
        currentScript->scriptText);

    currentScript->updateValuesTable(
        ui->tableVariables);

    // qDebug() << "Public Variables table rows:"
    //          << ui->tableVariables->rowCount();
}

void ScriptingWindow::connectScriptContainer(ScriptContainer *container)
{
    if (!container)
    {
        return;
    }

    container->setScriptWindow(this);

    connect(container, &ScriptContainer::stateChanged,
            this, &ScriptingWindow::scriptStateChanged);

    connect(container, &ScriptContainer::runtimeError,
            this, &ScriptingWindow::scriptRuntimeError);
}

void ScriptingWindow::updateScriptListItem(ScriptContainer *container)
{
    if (!container)
    {
        return;
    }

    QListWidgetItem *item = listItemForScript(container);
    if (!item)
    {
        return;
    }

    QString prefix;
    QColor color;
    QString tooltip;

    switch (container->state())
    {
    case ScriptContainer::ScriptRunState::Stopped:
        prefix = QStringLiteral("○");
        color = QColor(Qt::darkGray);
        tooltip = QStringLiteral("Stopped");
        break;

    case ScriptContainer::ScriptRunState::Starting:
        prefix = QStringLiteral("◐");
        color = QColor(30, 110, 200);
        tooltip = QStringLiteral("Starting");
        break;

    case ScriptContainer::ScriptRunState::Running:
        prefix = QStringLiteral("●");
        color = QColor(0, 130, 55);
        tooltip = QStringLiteral("Running");
        break;

    case ScriptContainer::ScriptRunState::Stopping:
        prefix = QStringLiteral("◐");
        color = QColor(210, 130, 0);
        tooltip = QStringLiteral("Stopping");
        break;

    case ScriptContainer::ScriptRunState::Error:
        prefix = QStringLiteral("✕");
        color = QColor(190, 0, 0);
        tooltip = QStringLiteral(
            "Stopped because the script produced a runtime error");
        break;

    case ScriptContainer::ScriptRunState::Disabled:
        prefix = QStringLiteral("−");
        color = QColor(Qt::lightGray);
        tooltip = QStringLiteral(
            "Disabled; excluded from Run All");
        break;
    }

    item->setText(QStringLiteral("%1 %2 — %3")
                      .arg(prefix,
                           container->fileName,
                           container->stateText()));

    item->setForeground(QBrush(color));
    item->setToolTip(tooltip);
}

void ScriptingWindow::updateAllScriptListItems()
{
    for (ScriptContainer *container : scripts)
    {
        updateScriptListItem(container);
    }
}

void ScriptingWindow::updateExecutionControls()
{
    ScriptContainer *container = selectedScript();

    const bool hasSelection = container != nullptr;
    const bool enabled = hasSelection && container->isEnabled();
    const bool running = hasSelection && container->isRunning();

    const bool hasRunnableScript = std::any_of(
        scripts.cbegin(), scripts.cend(),
        [](const ScriptContainer *script)
        {
            return script->isEnabled() && !script->isRunning();
        });

    const bool hasRunningScript = std::any_of(
        scripts.cbegin(), scripts.cend(),
        [](const ScriptContainer *script)
        {
            return script->isRunning();
        });

    ui->btnValidateScript->setEnabled(hasSelection);
    ui->btnRunScript->setEnabled(enabled && !running);
    ui->btnStopScript->setEnabled(running);
    ui->btnRestartScript->setEnabled(enabled);

    ui->btnRunAllScripts->setEnabled(hasRunnableScript);
    ui->btnStopAllScripts->setEnabled(hasRunningScript);

    {
        QSignalBlocker blocker(ui->cbScriptEnabled);
        ui->cbScriptEnabled->setEnabled(hasSelection);
        ui->cbScriptEnabled->setChecked(
            hasSelection ? container->isEnabled() : false);
    }
}

void ScriptingWindow::scriptStateChanged()
{
    ScriptContainer *container =
        qobject_cast<ScriptContainer *>(sender());

    updateScriptListItem(container);
    updateExecutionControls();
}

void ScriptingWindow::scriptRuntimeError(const QString &phase,
                                         int line,
                                         const QString &message,
                                         const QString &stack)
{
    ScriptContainer *container =
        qobject_cast<ScriptContainer *>(sender());

    if (!container)
    {
        return;
    }

    const ThemeColors colors = ThemeManager::colors();

    const QColor errorColor =
        ThemeManager::adjustedForContrast(
            colors.trace[3], colors.viewBg);

    const QColor stackColor =
        ThemeManager::adjustedForContrast(
            colors.mutedText, colors.viewBg);

    log(QStringLiteral("✕ %1 error, line %2: %3")
            .arg(phase)
            .arg(line)
            .arg(message));

    QListWidgetItem *errorItem = nullptr;
    if (ui->listLog->count() > 0)
    {
        errorItem = ui->listLog->item(ui->listLog->count() - 1);
    }

    if (errorItem)
    {
        errorItem->setForeground(errorColor);

        errorItem->setData(
            LogScriptPointerRole,
            QVariant::fromValue<quintptr>(
                reinterpret_cast<quintptr>(container)));
        errorItem->setData(LogLineRole, line);
        errorItem->setData(LogIsRuntimeErrorRole, true);

        errorItem->setToolTip(
            tr("Double-click to open %1 at line %2.")
                .arg(container->fileName)
                .arg(line));
    }

    if (!stack.isEmpty())
    {
        log(QStringLiteral("    ↳ Stack: %1").arg(stack));

        QListWidgetItem *stackItem = nullptr;
        if (ui->listLog->count() > 0)
        {
            stackItem = ui->listLog->item(ui->listLog->count() - 1);
        }

        if (stackItem)
        {
            stackItem->setForeground(stackColor);

            QFont stackFont = stackItem->font();
            stackFont.setItalic(true);
            stackItem->setFont(stackFont);

            /*
             * Informational only: stack locations are not the editor
             * navigation target, so prevent this row being selected.
             */
            stackItem->setFlags(
                stackItem->flags() & ~Qt::ItemIsSelectable);

            stackItem->setToolTip(
                tr("Engine stack trace; informational only."));
        }
    }
}

void ScriptingWindow::navigateToRuntimeError(QListWidgetItem *item)
{
    if (!item || !item->data(LogIsRuntimeErrorRole).toBool())
    {
        return;
    }

    const quintptr containerAddress =
        static_cast<quintptr>(
            item->data(LogScriptPointerRole).toULongLong());

    ScriptContainer *container =
        reinterpret_cast<ScriptContainer *>(containerAddress);

    /*
     * The log can outlive a deleted script. Compare addresses only; never
     * dereference a container unless it is still owned by scripts.
     */
    if (!container || !scripts.contains(container))
    {
        log(tr("The script associated with this error is no longer loaded."));
        return;
    }

    const int scriptRow = scripts.indexOf(container);
    if (scriptRow < 0)
    {
        log(tr("The script associated with this error is no longer loaded."));
        return;
    }

    if (ui->listLoadedScripts->currentRow() != scriptRow)
    {
        ui->listLoadedScripts->setCurrentRow(scriptRow);
    }
    else if (currentScript != container)
    {
        changeCurrentScript();
    }

    if (!editor)
    {
        return;
    }

    const int targetLine = qMax(1, item->data(LogLineRole).toInt());
    QTextBlock targetBlock =
        editor->document()->findBlockByNumber(targetLine - 1);

    if (!targetBlock.isValid())
    {
        targetBlock = editor->document()->lastBlock();
    }

    QTextCursor cursor(targetBlock);
    editor->setTextCursor(cursor);
    editor->ensureCursorVisible();
    editor->setFocus();
}

QString ScriptingWindow::builtInTemplateDirectory() const
{
    const QString installedDirectory = QStandardPaths::locate(
        QStandardPaths::GenericDataLocation,
        QStringLiteral("SavvyLens/templates"),
        QStandardPaths::LocateDirectory);

    if (!installedDirectory.isEmpty())
    {
        return installedDirectory;
    }

    /*
     * Development fallback: look beside the executable, which lets a local
     * Debug or Release build use a manually/deployment-copied templates
     * directory without installing the application system-wide.
     */
    return QDir(QCoreApplication::applicationDirPath())
        .filePath(QStringLiteral("templates"));
}

QList<ScriptingWindow::ScriptTemplate>
ScriptingWindow::availableTemplates() const
{
    QList<ScriptTemplate> templates;

    const QDir builtInDirectory(builtInTemplateDirectory());
    const QDir userDirectory(userTemplateDirectory());

    const QStringList filters =
        QStringList() << QStringLiteral("*.js");

    const QFileInfoList builtInFiles =
        builtInDirectory.entryInfoList(
            filters,
            QDir::Files | QDir::Readable,
            QDir::Name | QDir::IgnoreCase);

    for (const QFileInfo &fileInfo : builtInFiles)
    {
        templates.append(
            {fileInfo.completeBaseName(),
             fileInfo.absoluteFilePath(),
             true});
    }

    const QFileInfoList userFiles =
        userDirectory.entryInfoList(
            filters,
            QDir::Files | QDir::Readable,
            QDir::Name | QDir::IgnoreCase);

    for (const QFileInfo &fileInfo : userFiles)
    {
        templates.append(
            {fileInfo.completeBaseName(),
             fileInfo.absoluteFilePath(),
             false});
    }

    return templates;
}

QString ScriptingWindow::userTemplateDirectory() const
{
    const QString appDataPath = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation);

    return QDir(appDataPath).filePath(QStringLiteral("templates"));
}

void ScriptingWindow::rebuildTemplateMenu()
{
    if (!templateMenu)
    {
        return;
    }

    templateMenu->clear();

    const QList<ScriptTemplate> templates = availableTemplates();

    QMenu *builtInMenu = templateMenu->addMenu(tr("Built-in Templates"));
    QMenu *userMenu = templateMenu->addMenu(tr("My Templates"));

    bool hasUserTemplates = false;

    for (const ScriptTemplate &scriptTemplate : templates)
    {
        QMenu *targetMenu =
            scriptTemplate.builtIn ? builtInMenu : userMenu;

        QAction *action = targetMenu->addAction(
            scriptTemplate.displayName);

        connect(action, &QAction::triggered, this,
                [this, scriptTemplate]()
                {
                    insertTemplate(scriptTemplate);
                });

        if (!scriptTemplate.builtIn)
        {
            hasUserTemplates = true;
        }
    }

    if (!hasUserTemplates)
    {
        QAction *emptyAction = userMenu->addAction(
            tr("No saved templates"));

        emptyAction->setEnabled(false);
    }

    templateMenu->addSeparator();

    QAction *saveTemplateAction = templateMenu->addAction(
        tr("Save Current Script as Template..."));

    saveTemplateAction->setEnabled(currentScript != nullptr);

    connect(saveTemplateAction, &QAction::triggered,
            this, &ScriptingWindow::saveCurrentScriptAsTemplate);
}

QString ScriptingWindow::loadTemplateSource(
    const ScriptTemplate &scriptTemplate) const
{
    QFile templateFile(scriptTemplate.sourcePath);

    if (!templateFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return QString();
    }

    return QString::fromUtf8(templateFile.readAll());
}

void ScriptingWindow::insertTemplate(
    const ScriptTemplate &scriptTemplate)
{
    const QString source = loadTemplateSource(scriptTemplate);

    if (source.isEmpty())
    {
        log(tr("Unable to load template \"%1\".")
                .arg(scriptTemplate.displayName));
        return;
    }

    /*
     * Templates always create an independent loaded script. Editing or saving
     * it does not modify the original built-in or user template source file.
     */
    createNewScript();

    if (!currentScript || !editor)
    {
        return;
    }

    const QString baseName =
        scriptTemplate.displayName
            .toLower()
            .replace(QLatin1Char(' '), QLatin1Char('_'));

    currentScript->fileName =
        QStringLiteral("%1.js").arg(baseName);

    currentScript->filePath.clear();
    currentScript->scriptText = source;

    editor->setPlainText(source);

    refreshCurrentPublicVariables();

    QListWidgetItem *item = listItemForScript(currentScript);
    if (item)
    {
        updateScriptListItem(currentScript);
    }

    updateExecutionControls();

    log(tr("Created script from template \"%1\".")
            .arg(scriptTemplate.displayName));
}

void ScriptingWindow::saveCurrentScriptAsTemplate()
{
    if (!currentScript || !editor)
    {
        return;
    }

    synchronizeCurrentScriptSource();

    if (currentScript->scriptText.trimmed().isEmpty())
    {
        log(tr("Cannot save an empty script as a template."));
        return;
    }

    bool accepted = false;

    QString suggestedName =
        QFileInfo(currentScript->fileName).completeBaseName();

    if (suggestedName.isEmpty())
    {
        suggestedName = QStringLiteral("my_template");
    }

    QString templateName = QInputDialog::getText(
                               this,
                               tr("Save Script as Template"),
                               tr("Template name:"),
                               QLineEdit::Normal,
                               suggestedName,
                               &accepted)
                               .trimmed();

    if (!accepted || templateName.isEmpty())
    {
        return;
    }

    /*
     * Keep template filenames portable and prevent an entered path from
     * escaping the dedicated templates directory.
     */
    templateName.replace(QLatin1Char('/'), QLatin1Char('_'));
    templateName.replace(QLatin1Char('\\'), QLatin1Char('_'));

    if (!templateName.endsWith(
            QStringLiteral(".js"), Qt::CaseInsensitive))
    {
        templateName.append(QStringLiteral(".js"));
    }

    QDir directory(userTemplateDirectory());

    if (!directory.exists() && !directory.mkpath(QStringLiteral(".")))
    {
        log(tr("Unable to create the user template directory."));
        return;
    }

    const QString targetPath =
        directory.filePath(templateName);

    QFile templateFile(targetPath);

    if (!templateFile.open(
            QIODevice::WriteOnly |
            QIODevice::Text |
            QIODevice::Truncate))
    {
        log(tr("Unable to save template \"%1\".")
                .arg(templateName));
        return;
    }

    templateFile.write(
        currentScript->scriptText.toUtf8());

    templateFile.close();

    log(tr("Saved \"%1\" as a reusable template.")
            .arg(templateName));
}
