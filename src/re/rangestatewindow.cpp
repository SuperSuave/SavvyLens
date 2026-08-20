#include "rangestatewindow.h"
#include "ui_rangestatewindow.h"

// SavvyLens headers
#include "app/helpwindow.h"
#include "app/mainwindow.h"
#include "common/utility.h"
#include "widgets/filterutility.h"

RangeStateWindow::RangeStateWindow(const QVector<CANFrame> *frames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RangeStateWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);

    modelFrames = frames;

    ui->graphSignal->xAxis->setRange(0, 8);
    ui->graphSignal->yAxis->setRange(-10, 265); //run range a bit outside possible number so they aren't plotted in a hard to see place
    ui->graphSignal->xAxis->setVisible(false);
    ui->graphSignal->yAxis->setVisible(false);

    //ui->graphSignal->axisRect()->setupFullAxesBox();

    ui->cbSignalMode->addItem(tr("Big Endian"));
    ui->cbSignalMode->addItem(tr("Little Endian"));
    ui->cbSignalMode->addItem(tr("Try Both"));

    ui->cbSignedMode->addItem(tr("Signed Value"));
    ui->cbSignedMode->addItem(tr("Unsigned Value"));
    ui->cbSignedMode->addItem(tr("Try Both"));

//lambda expressions used here because these are tiny functions that don't have any reason to be full named functions
//unfortunately the fact that valueChanged is overloaded makes the syntax here HORRIBLE. That sucks.
    connect(ui->spinMaxSigSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int newVal)
            {
                if (newVal < ui->spinMinSigSize->value()) ui->spinMinSigSize->setValue(newVal);
            });

    connect(ui->spinMinSigSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int newVal)
            {
                if (newVal > ui->spinMaxSigSize->value()) ui->spinMaxSigSize->setValue(newVal);
            });

    connect(ui->btnAllFilter, &QAbstractButton::clicked,
            [=]()
            {
                for (int i = 0; i < ui->listFilter->count(); i++)
                {
                    QListWidgetItem *item = ui->listFilter->item(i);
                    item->setCheckState(Qt::Checked);
                    idFilters[Utility::ParseStringToNum(item->text())] = true;
                }
            });

    connect(ui->btnNoneFilter, &QAbstractButton::clicked,
            [=]()
            {
                for (int i = 0; i < ui->listFilter->count(); i++)
                {
                    QListWidgetItem *item = ui->listFilter->item(i);
                    item->setCheckState(Qt::Unchecked);
                    idFilters[Utility::ParseStringToNum(item->text())] = false;
                }
            });

    connect(ui->listFilter, &QListWidget::itemChanged,
            [=](QListWidgetItem *item)
            {
                bool isChecked = false;
                int id = FilterUtility::getIdAsInt(item);
                if (item->checkState() == Qt::Checked) isChecked = true;
                idFilters[id] = isChecked;
            });

    connect(ui->btnRecalc, &QAbstractButton::clicked, this, &RangeStateWindow::recalcButton);
    connect(MainWindow::getReference(), SIGNAL(framesUpdated(int)), this, SLOT(updatedFrames(int)));
    connect(ui->listCandidates, &QListWidget::currentRowChanged, this, &RangeStateWindow::clickedSignalList);
}

RangeStateWindow::~RangeStateWindow()
{
    delete ui;
}

void RangeStateWindow::showEvent(QShowEvent* event)
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
    if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        switch (keyEvent->key())
        {
        case Qt::Key_F1:
            HelpWindow::getRef()->showHelp("rangestate.md");
            break;
        }
        return true;
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
    return false;
}

void RangeStateWindow::readSettings()
{
    QSettings settings;
    if (settings.value("Main/SaveRestorePositions", false).toBool())
    {
        resize(settings.value("RangeStateView/WindowSize", QSize(765, 615)).toSize());
        move(Utility::constrainedWindowPos(settings.value("RangeStateView/WindowPos", QPoint(50, 50)).toPoint()));
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
    CANFrame thisFrame;
    if (numFrames == -1) //all frames deleted. We don't need to do a thing on this window but erase everything in the filters section
    {
        ui->listFilter->clear();
        idFilters.clear();
    }
    else if (numFrames == -2) //all new set of frames. Reset
    {
        refreshFilterList();
    }
    else //just got some new frames. See if we need to update the filters list. Otherwise nothing to do - no recalc happens until the button is pressed
    {
        if (numFrames > modelFrames->count()) return;
        for (int i = modelFrames->count() - numFrames; i < modelFrames->count(); i++)
        {
            thisFrame = modelFrames->at(i);
            if (!idFilters.contains(thisFrame.frameId()))
            {
                idFilters.insert(thisFrame.frameId(), true);
                FilterUtility::createCheckableFilterItem(thisFrame.frameId(), true, ui->listFilter);
            }
        }
    }
}

void RangeStateWindow::refreshFilterList()
{
    int id;

    idFilters.clear();
    ui->listFilter->clear();

    for (int i = 0; i < modelFrames->length(); i++)
    {
        id = modelFrames->at(i).frameId();
        if (!idFilters.contains(id))
        {
            idFilters.insert(id, true);
            FilterUtility::createCheckableFilterItem(id, true, ui->listFilter);
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

        qDebug() << "ID"
                 << QString::number(canId, 16).toUpper()
                 << "produced"
                 << candidates.count()
                 << "range candidates.";

        for (const RangeSignalCandidate &candidate : candidates)
        {
            foundCandidates.append(candidate);

            ui->listCandidates->addItem(
                candidate.summaryText());
        }

        if (candidates.size() >= scanConfig.maxCandidates)
        {
            ui->listCandidates->addItem(
                tr("Candidate result limit reached (%1) for ID %2.")
                    .arg(scanConfig.maxCandidates)
                    .arg(QString::number(canId, 16).toUpper()));
        }

        qApp->processEvents();
    }

    progress.cancel();

    qDebug() << "Found" << foundCandidates.count()
             << "signals total.";
}

//graphs the vector such that the X axis is just the index into the vector and Y is perfectly graphed within the window
void RangeStateWindow::createGraph(QVector<int> values)
{

    int tempVal;
    float minval=1000000, maxval = -100000;

    int numEntries = values.count();

    ui->graphSignal->clearGraphs();

    QVector<double> x(numEntries), y(numEntries);

    for (int j = 0; j < numEntries; j++)
    {
        tempVal = values[j];

        x[j] = j;

        y[j] = tempVal;
        if (y[j] < minval) minval = y[j];
        if (y[j] > maxval) maxval = y[j];
    }

    qDebug() << "YMin: " << minval << " YMax: " << maxval;

    double ymin, ymax;

    ymin = minval;

    if (ymin < 0) ymin *= 1.2;
    else ymin *= 0.8;

    ymax = maxval;
    if (ymax < 0) ymax *= 0.8;
    else ymax *= 1.2;

    if (fabs(ymin) < 0.01) ymin -= (ymax / 60.0);
    if (fabs(ymax) < 0.01) ymax -= (ymin / 60.0);

    qDebug() << "YFm: " << ymin << " YFM: " << ymax;

    ui->graphSignal->addGraph();
    ui->graphSignal->graph()->setName("Graph");
    ui->graphSignal->graph()->setData(x,y);
    ui->graphSignal->graph()->setLineStyle(QCPGraph::lsLine); //connect points with lines
    QPen graphPen;
    graphPen.setColor(Qt::black);
    graphPen.setWidth(1);
    ui->graphSignal->graph()->setPen(graphPen);
    ui->graphSignal->xAxis->setRange(0, numEntries);
    ui->graphSignal->yAxis->setRange(ymin, ymax);
    ui->graphSignal->replot();
}

void RangeStateWindow::clickedSignalList(int idx)
{
    if (idx < 0 || idx >= foundCandidates.size())
    {
        return;
    }

    const RangeSignalCandidate &candidate =
        foundCandidates.at(idx);

    QVector<int> graphValues;

    graphValues.reserve(candidate.sampleValues.size());

    for (qint64 value : candidate.sampleValues)
    {
        graphValues.append(static_cast<int>(value));
    }

    createGraph(graphValues);
}
