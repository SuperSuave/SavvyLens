#ifndef ISOTP_HANDLER_H
#define ISOTP_HANDLER_H

#pragma once

// SavvyLens headers
#include "bus_protocols/isotp_message.h"
#include "can/can_structs.h"
#include "can/canfilter.h"

// Qt headers
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QObject>
#include <QTimer>
#include <QVector>

class CANConnection;

class ISOTP_HANDLER : public QObject
{
    Q_OBJECT

public:
    ISOTP_HANDLER();
    ~ISOTP_HANDLER();

    void setExtendedAddressing(bool mode);
    void setReception(bool mode);
    void setEmitPartials(bool mode);
    void sendISOTPFrame(int bus, int ID, QByteArray data);
    void setProcessAll(bool state);
    void setFlowCtrl(bool state);

    void addFilter(int pBusId, uint32_t ID, uint32_t mask);
    void removeFilter(int pBusId, uint32_t ID, uint32_t mask);
    void clearAllFilters();

public slots:
    void updatedFrames(int);
    void rapidFrames(
        const CANConnection *conn,
        const QVector<CANFrame> &pFrames);

    void frameTimerTick();

signals:
    void newISOMessage(ISOTP_MESSAGE msg);

private:
    QHash<uint32_t, ISOTP_MESSAGE> messageBuffer;
    QList<CANFrame> sendingFrames;
    QList<CANFilter> filters;

    const QVector<CANFrame> *modelFrames = nullptr;

    bool useExtendedAddressing = false;
    bool isReceiving = false;
    bool waitingForFlow = false;
    int framesUntilFlow = 0;
    bool processAll = false;
    bool issueFlowMsgs = false;
    bool sendPartialMessages = false;

    QTimer frameTimer;

    uint32_t lastSenderID = 0;
    uint32_t lastSenderBus = 0;

    void processFrame(const CANFrame &frame);
    void checkNeedFlush(uint64_t ID);
};

#endif // ISOTP_HANDLER_H
