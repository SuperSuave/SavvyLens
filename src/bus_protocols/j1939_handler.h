#ifndef J1939_HANDLER_H
#define J1939_HANDLER_H

// SavvyLens headers
#include "can/can_structs.h"

// Qt headers
#include <QDebug>
#include <QObject>
#include <Qt>

struct J1939ID
{
public:
    int src;
    int dest;
    int pgn;
    int edp;
    int dp;
    int pf;
    int ps;
    int priority;
    bool isBroadcast;
};

#endif // J1939_HANDLER_H
