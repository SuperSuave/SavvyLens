#include "rangestatewindow.h"
#include "ui_rangestatewindow.h"

// SavvyLens headers
#include "app/helpwindow.h"
#include "app/mainwindow.h"
#include "common/utility.h"
#include "widgets/filterutility.h"

RangeStateWindow::RangeStateWindow(const QVector<CANFrame> *frames,
                                   QWidget *parent)
    : QDialog(parent),
      ui(new Ui::RangeStateWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);

    modelFrames = frames;

    ui->graphSignal->xAxis->setRange(0, 8);
    ui->graphSignal->yAxis->setRange(
        -10, 265); // Run range a bit outside possible values for visibility.
    ui->graphSignal->xAxis->setVisible(false);
    ui->graphSignal->yAxis->setVisible(false);

    // ui->graphSignal->axisRect()->setupFullAxesBox();

    ui->cbSignalMode->addItem(tr("Big Endian"));
    ui->cbSignalMode->addItem(tr("Little Endian"));
    ui->cbSignalMode->addItem(tr("Try Both"));

    ui->cbSignedMode->addItem(tr("Signed Value"));
    ui->cbSignedMode->addItem(tr("Unsigned Value"));
    ui->cbSignedMode->addItem(tr("Try Both"));

    // First-open discovery defaults. These align with RangeScanConfig defaults
    // and provide a practical byte-aligned first pass.
    ui->spinMinSigSize->setValue(8);
    ui->spinMaxSigSize->setValue(32);
    ui->spinGranularity->setValue(8);
    ui->slideSensitivity->setValue(128);
    ui->cbSignalMode->setCurrentIndex(2); // Try Both
    ui->cbSignedMode->setCurrentIndex(2); // Try Both

    // Keep the selected signal-size range valid as either spin box changes.
    connect(ui->spinMaxSigSize,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int newValue)
            {
                if (newValue < ui->spinMinSigSize->value())
                {
                    ui->spinMinSigSize->setValue(newValue);
                }
            });

    connect(ui->spinMinSigSize,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int newValue)
            {
                if (newValue > ui->spinMaxSigSize->value())
                {
                    ui->spinMaxSigSize->setValue(newValue);
                }
            });

    connect(ui->btnAllFilter,
            &QAbstractButton::clicked,
            [=]()
            {
                for (int i = 0; i < ui->listFilter->count(); ++i)
                {
                    QListWidgetItem *item = ui->listFilter->item(i);
                    item->setCheckState(Qt::Checked);
                    idFilters[Utility::ParseStringToNum(item->text())] = true;
                }
            });

    connect(ui->btnNoneFilter,
            &QAbstractButton::clicked,
            [=]()
            {
                for (int i = 0; i < ui->listFilter->count(); ++i)
                {
                    QListWidgetItem *item = ui->listFilter->item(i);
                    item->setCheckState(Qt::Unchecked);
                    idFilters[Utility::ParseStringToNum(item->text())] = false;
                }
            });

    connect(ui->listFilter,
            &QListWidget::itemChanged,
            [=](QListWidgetItem *item)
            {
                const int id = FilterUtility::getIdAsInt(item);
                const bool isChecked = item->checkState() == Qt::Checked;
                idFilters[id] = isChecked;
            });

    connect(ui->btnRecalc,
            &QAbstractButton::clicked,
            this,
            &RangeStateWindow::recalcButton);

    connect(MainWindow::getReference(),
            SIGNAL(framesUpdated(int)),
            this,
            SLOT(updatedFrames(int)));

    connect(ui->listCandidates,
            &QListWidget::currentRowChanged,
            this,
            &RangeStateWindow::clickedSignalList);
}

RangeStateWindow::~RangeStateWindow()
{
    delete ui;
}

void RangeStateWindow::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    readSettings();
    refreshFilterList();

    installEventFilter(this);
}

void RangeStateWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    removeEventFilter(this);
    writeSettings();
}

bool RangeStateWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        switch (keyEvent->key())
        {
        case Qt::Key_F1:
            HelpWindow::getRef()->showHelp("rangestate.md");
            break;

        default:
            break;
        }

        return true;
    }

    return QObject::eventFilter(obj, event);
}

void RangeStateWindow::readSettings()
{
    QSettings settings;

    if (settings.value("Main/SaveRestorePositions", false).toBool())
    {
        resize(settings
                   .value("RangeStateView/WindowSize", QSize(765, 615))
                   .toSize());

        move(Utility::constrainedWindowPos(
            settings
                .value("RangeStateView/WindowPos", QPoint(50, 50))
                .toPoint()));
    }
}

void RangeStateWindow::writeSettings()
{
    QSettings settings;

    if (settings.value("Main/SaveRestorePositions", false).toBool())
    {
        settings.setValue("RangeStateView/WindowSize", size());
        settings.setValue("RangeStateView/WindowPos", pos());
    }
}

void RangeStateWindow::updatedFrames(int numFrames)
{
    if (numFrames == -1)
    {
        // All frames were deleted.
        ui->listFilter->clear();
        idFilters.clear();
        return;
    }

    if (numFrames == -2)
    {
        // A complete new frame set was loaded.
        refreshFilterList();
        return;
    }

    // New frames arrived. Update available ID filters only; recalculation
    // remains an explicit action through the Recalculate button.
    if (numFrames > modelFrames->count())
    {
        return;
    }

    for (int i = modelFrames->count() - numFrames;
         i < modelFrames->count();
         ++i)
    {
        const CANFrame &frame = modelFrames->at(i);

        if (!idFilters.contains(frame.frameId()))
        {
            idFilters.insert(frame.frameId(), true);

            FilterUtility::createCheckableFilterItem(
                frame.frameId(),
                true,
                ui->listFilter);
        }
    }
}

void RangeStateWindow::refreshFilterList()
{
    idFilters.clear();
    ui->listFilter->clear();

    for (int i = 0; i < modelFrames->length(); ++i)
    {
        const int id = modelFrames->at(i).frameId();

        if (!idFilters.contains(id))
        {
            idFilters.insert(id, true);
            FilterUtility::createCheckableFilterItem(
                id,
                true,
                ui->listFilter);
        }
    }

    ui->listFilter->sortItems();
}

void RangeStateWindow::recalcButton()
{
    ui->listCandidates->clear();
    foundCandidates.clear();
    ui->graphSignal->clearGraphs();

    RangeScanConfig scanConfig;
    scanConfig.minBitLength = ui->spinMinSigSize->value();
    scanConfig.maxBitLength = ui->spinMaxSigSize->value();
    scanConfig.bitGranularity = ui->spinGranularity->value();
    scanConfig.sensitivity = ui->slideSensitivity->value();
    scanConfig.maxCandidates = 2000;
    scanConfig.populateSamples = true;

    switch (ui->cbSignalMode->currentIndex())
    {
    case 0:
        scanConfig.endianMode = RangeScanConfig::BigEndianOnly;
        break;

    case 1:
        scanConfig.endianMode = RangeScanConfig::LittleEndianOnly;
        break;

    default:
        scanConfig.endianMode = RangeScanConfig::TryBothEndian;
        break;
    }

    switch (ui->cbSignedMode->currentIndex())
    {
    case 0:
        scanConfig.signedMode = RangeScanConfig::SignedOnly;
        break;

    case 1:
        scanConfig.signedMode = RangeScanConfig::UnsignedOnly;
        break;

    default:
        scanConfig.signedMode = RangeScanConfig::TryBothSigned;
        break;
    }

    QProgressDialog progress(qApp->activeWindow());

    progress.setWindowModality(Qt::WindowModal);
    progress.setLabelText(tr("Calculating"));
    progress.setCancelButton(nullptr);
    progress.setRange(0, 0);
    progress.setMinimumDuration(0);
    progress.show();

    for (QMap<int, bool>::const_iterator iter = idFilters.constBegin();
         iter != idFilters.constEnd();
         ++iter)
    {
        if (!iter.value())
        {
            continue;
        }

        const quint32 canId = static_cast<quint32>(iter.key());

        qDebug() << "Processing for ID:" << canId;

        const QVector<RangeSignalCandidate> candidates =
            RangeStatistics::scanCandidates(
                *modelFrames,
                canId,
                scanConfig);

        for (const RangeSignalCandidate &candidate : candidates)
        {
            foundCandidates.append(candidate);
            ui->listCandidates->addItem(candidate.summaryText());
        }

        qApp->processEvents();
    }

    progress.cancel();

    qDebug() << "Found" << foundCandidates.count()
             << "signals total.";
}

// Graphs extracted sample values by accepted-sample index.
void RangeStateWindow::createGraph(const QVector<qint64> &values)
{
    ui->graphSignal->clearGraphs();

    if (values.isEmpty())
    {
        ui->graphSignal->replot();
        return;
    }

    const int numEntries = values.count();

    QVector<double> x(numEntries);
    QVector<double> y(numEntries);

    double minValue = static_cast<double>(values.at(0));
    double maxValue = minValue;

    for (int index = 0; index < numEntries; ++index)
    {
        const qint64 value = values.at(index);

        x[index] = static_cast<double>(index);
        y[index] = static_cast<double>(value);

        if (y[index] < minValue)
        {
            minValue = y[index];
        }

        if (y[index] > maxValue)
        {
            maxValue = y[index];
        }
    }

    double yMinimum = minValue;
    double yMaximum = maxValue;

    if (yMinimum < 0.0)
    {
        yMinimum *= 1.2;
    }
    else
    {
        yMinimum *= 0.8;
    }

    if (yMaximum < 0.0)
    {
        yMaximum *= 0.8;
    }
    else
    {
        yMaximum *= 1.2;
    }

    if (qAbs(yMinimum) < 0.01)
    {
        yMinimum -= yMaximum / 60.0;
    }

    if (qAbs(yMaximum) < 0.01)
    {
        yMaximum -= yMinimum / 60.0;
    }

    if (qFuzzyCompare(yMinimum, yMaximum))
    {
        const double padding = qMax(1.0, qAbs(yMinimum) * 0.1);
        yMinimum -= padding;
        yMaximum += padding;
    }

    ui->graphSignal->addGraph();
    ui->graphSignal->graph()->setName("Graph");
    ui->graphSignal->graph()->setData(x, y);
    ui->graphSignal->graph()->setLineStyle(QCPGraph::lsLine);

    QPen graphPen;
    graphPen.setColor(Qt::black);
    graphPen.setWidth(1);
    ui->graphSignal->graph()->setPen(graphPen);

    ui->graphSignal->xAxis->setRange(0, numEntries);
    ui->graphSignal->yAxis->setRange(yMinimum, yMaximum);
    ui->graphSignal->replot();
}

void RangeStateWindow::clickedSignalList(int idx)
{
    if (idx < 0 || idx >= foundCandidates.size())
    {
        return;
    }

    const RangeSignalCandidate &candidate = foundCandidates.at(idx);
    createGraph(candidate.sampleValues);
}
