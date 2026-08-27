#ifndef STUDIOHOST_H
#define STUDIOHOST_H

// SavvyLens headers
#include "analysis/candidateanalysis.h"
#include "analysis/frameaggregatestore.h"
#include "analysis/rangestatistics.h"

// Qt headers
#include <QVector>
#include <QWidget>

class CANFrame;
class LiveChangeExplorerModel;
class QCloseEvent;
class QQuickWidget;
class StateExplorerPresentation;

class StudioHost final : public QWidget
{
    Q_OBJECT

public:
    explicit StudioHost(
        LiveChangeExplorerModel *liveChangeExplorerModel,
        QWidget *parent = nullptr);

    Q_INVOKABLE bool analyzeStateExplorerCandidate(quint32 canId,
                                                   int startBit,
                                                   int bitLength,
                                                   bool isLittleEndian,
                                                   bool isSigned);
    Q_INVOKABLE void openTrafficWorkspace();
    Q_INVOKABLE void exploreLiveChangeRowInStateExplorer(int row);

    void loadStateExplorerSnapshot(
        const FrameAggregateKey &key,
        const QVector<CANFrame> &frames);

public slots:
    void closeStudio();

signals:
    void exploreLiveChangeRowRequested(int row);
    void studioClosed();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    CandidateAnalysis::Config stateExplorerConfig(
        const RangeSignalSpec &candidate) const;

        void openWorkspace(const QString &workspaceId);
        QString stateExplorerSnapshotSourceLabel(
            const FrameAggregateKey &key) const;
    void loadStateExplorerDemo();

    QQuickWidget *quickWidget_ = nullptr;
    StateExplorerPresentation *stateExplorerPresentation_ = nullptr;
    QVector<CANFrame> stateExplorerFrames_;
};

#endif // STUDIOHOST_H
