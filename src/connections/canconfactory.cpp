#include "connections/canconfactory.h"

// SavvyLens headers
#include "connections/canlogserver.h"
#include "connections/canserver.h"
#include "connections/gvretserial.h"
#include "connections/lawicel_serial.h"
#include "connections/mqtt_bus.h"
#include "connections/serialbusconnection.h"
#include "connections/socketcand.h"

using namespace CANCon;

CANConnection* CanConFactory::create(type pType, QString pPortName, QString pDriverName, int pSerialSpeed, int pBusSpeed, bool pCanFd, int pDataRate)
{
    switch(pType) {
    case SERIALBUS:
      return new SerialBusConnection(pPortName, pDriverName, pBusSpeed, pDataRate, pCanFd);
    case GVRET_SERIAL:
        if(pPortName.contains(".") && !pPortName.contains("tty") && !pPortName.contains("serial"))
        return new GVRetSerial(pPortName, true);
        else
        return new GVRetSerial(pPortName, false);
    case REMOTE:
        return new GVRetSerial(pPortName, true);  //it's a special case of GVRET connected over TCP/IP so it uses the same class
    case LAWICEL:
        return new LAWICELSerial(pPortName, pSerialSpeed, pBusSpeed, pCanFd, pDataRate);
    case KAYAK:
        return new SocketCANd(pPortName);
    case MQTT:
        return new MQTT_BUS(pPortName);
    case CANSERVER:
        return new CANserver(pPortName);
    case CANLOGSERVER:
        return new CanLogServer(pPortName);
    default: {}
    }

    return nullptr;
}
