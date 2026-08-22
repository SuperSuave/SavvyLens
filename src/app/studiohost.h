#ifndef STUDIOHOST_H
#define STUDIOHOST_H

// SavvyLens headers
#include "analysis/rangestatistics.h"
#include "analysis/candidateanalysis.h"

// Qt headers
#include <QVector>
#include <QWidget>

class CANFrame;
class QCloseEvent;
class QQuickWidget;
class StateExplorerPresentation;

class StudioHost final : public QWidget
{
    Q_OBJECT

public:
    explicit StudioHost(QWidget *parent = nullptr);

    Q_INVOKABLE bool analyzeStateExplorerCandidate(quint32 canId,
                                                   int startBit,
                                                   int bitLength,
                                                   bool isLittleEndian,
                                                   bool isSigned);

public slots:
    void closeStudio();

signals:
    void studioClosed();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    CandidateAnalysis::Config stateExplorerDemoConfig(
        const RangeSignalSpec &candidate) const;
    void loadStateExplorerDemo();

    QQuickWidget *quickWidget_ = nullptr;
    StateExplorerPresentation *stateExplorerPresentation_ = nullptr;
    QVector<CANFrame> stateExplorerDemoFrames_;
};

#endif // STUDIOHOST_H
