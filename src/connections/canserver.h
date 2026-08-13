#ifndef CANSERVER_H
#define CANSERVER_H

// SavvyLens headers
#include "frames/canframemodel.h"
#include "connections/canconnection.h"
#include "connections/canconmanager.h"

// QT headers
#include <QCanBusDevice>
#include <QDateTime>
#include <QThread>
#include <QTimer>
#include <QUdpSocket>

// C++ standard-library headers
#include <cstdio>

class CANserver : public CANConnection
{
    Q_OBJECT

public:
    CANserver(QString serverAddress);
    virtual ~CANserver();

protected:

    virtual void piStarted();
    virtual void piStop();
    virtual void piSetBusSettings(int pBusIdx, CANBus pBus);
    virtual bool piGetBusSettings(int pBusIdx, CANBus& pBus);
    virtual void piSuspend(bool pSuspend);
    virtual bool piSendFrame(const CANFrame&);
    
private slots:
    void readNetworkData();
    void heartbeatTimerSlot();


private:
    void readSettings();
    
    void connectToDevice();
    void disconnectFromDevice();

    void heartbeat();
    
protected:
    QHostAddress _canserverAddress;
    
    QUdpSocket *_udpClient;

    QTimer  *_heartbeatTimer;
};

#endif // CANSERVER_H
