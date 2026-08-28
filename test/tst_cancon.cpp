#include <QtTest>
#include <QVector>

#include "tst_cancon.h"
#include "src/connections/canconnection.h"
#include "src/connections/canconfactory.h"


#define QVERIFYB(statement) \
do {\
    if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__))\
        return false;\
} while (0)

#define QCOMPAREB(actual, expected) \
do {\
    if (!QTest::qCompare(actual, expected, #actual, #expected, __FILE__, __LINE__))\
        return false;\
} while (0)

Q_DECLARE_METATYPE(CANConStatus)
Q_DECLARE_METATYPE(QVector<CANFltObserver>)

TestCanCon::TestCanCon(CANCon::type pType, QString pPortName, int pNbBus):
    mType(pType),
    mPortName(pPortName),
    mNbBus(pNbBus){}

void TestCanCon::create()
{
    CANConnection* conn_p = nullptr;
    if (!pCreate(conn_p)) {
        QSKIP("No hardware device available");
    }
    /* try to get an element from the queue */
    QVERIFY(conn_p->getQueue().get());

    delete conn_p;
}

void TestCanCon::connectToDevice()
{
    CANConnection* conn_p = nullptr;
    if (!pCreate(conn_p)) {
        QSKIP("No hardware device available");
    }

    QSignalSpy spy(conn_p, SIGNAL(status(CANConStatus)));

    /* start connection */
    conn_p->start();

    /* wait for a signal */
    for(int i=0 ; (spy.count() != 1) && (i < 10) ; i++)
        QTest::qWait(500);


    QCOMPARE(spy.count(), 1); // make sure the signal was emitted exactly one time
    QList<QVariant> arguments = spy.takeFirst(); // take the first signal

    QVERIFY(arguments.at(0).value<CANConStatus>().conStatus == CANCon::CONNECTED); // verify the first argument

    /* stop connection */
    conn_p->stop();
    delete conn_p;
}

void TestCanCon::recvFrames()
{
    CANConnection* conn_p = nullptr;
    if (!pCreate(conn_p)) {
        QSKIP("No hardware device available");
    }

    /* start connection */
    conn_p->start();

    /* configure */
    QVERIFY(pConfig(conn_p));

    LFQueue<CANFrame>& queue = conn_p->getQueue();

    /* wait for frames to arrive */
    QTest::qWait(1000);

    int i;
    for(i=0 ; queue.peek() && i<1000 ; i++)
    {
        CANFrame* canf_p = queue.peek();
        QVERIFY(pValidateFrame(conn_p, canf_p));

        queue.dequeue();
    }

    QVERIFY(i>0);

    /* stop connection */
    conn_p->stop();
    delete conn_p;
}


void TestCanCon::suspend()
{
    CANConnection* conn_p = nullptr;
    if (!pCreate(conn_p)) {
        QSKIP("No hardware device available");
    }

    /* start connection */
    conn_p->start();

    /* configure */
    QVERIFY(pConfig(conn_p));

    LFQueue<CANFrame>& queue = conn_p->getQueue();

    /* wait for frames to arrive */
    QTest::qWait(1000);

    CANFrame* canf_p = queue.peek();
    QVERIFY(pValidateFrame(conn_p, canf_p));

    conn_p->suspend(true);

    /* the queue should be flushed */
    canf_p = queue.peek();
    QVERIFY(!canf_p);

    /* restart capture */
    conn_p->suspend(false);

    /* wait for frames to arrive */
    QTest::qWait(1000);

    /* get a frame */
    canf_p = queue.peek();
    QVERIFY(pValidateFrame(conn_p, canf_p));

    /* stop connection */
    conn_p->stop();
    delete conn_p;
}


void TestCanCon::filter_data()
{
    CANConnection* conn_p = nullptr;
    if (!pCreate(conn_p)) {
        QSKIP("No hardware device available");
    }

    /* start connection */
    conn_p->start();

    /* configure */
    QVERIFY(pConfig(conn_p));

    LFQueue<CANFrame>& queue = conn_p->getQueue();

    /* wait for frames to arrive */
    QTest::qWait(1000);

    /* find 3 different ids */
    QVector<quint32> ids;

    while( queue.peek() && ids.count()!=3 )
    {
        CANFrame* canf_p = queue.peek();
        QVERIFY(pValidateFrame(conn_p, canf_p));

        if(!ids.contains(canf_p->frameId()))
            ids.append(canf_p->frameId());

        queue.dequeue();
    }

    QCOMPARE(ids.count(), 3);

    /* stop connection */
    conn_p->stop();
    delete conn_p;

    /* prepare test vector */

    QTest::addColumn<QVector<CANFltObserver>>("filters");
    QTest::addColumn<bool>("filterOut");
    QTest::addColumn<bool>("signalReceived");
    QTest::addColumn<QVector<quint32>>("filtered");

    QVector<CANFltObserver> filters;
    QVector<quint32> filteredIds;

    /* one filter no signal*/
    filters.clear();
    filteredIds.clear();
    filters.append(CANFltObserver{ids[0], 0xFFFF, nullptr});
    filteredIds.append(ids[0]);
    QTest::newRow("1filternosignal")        << filters << false << false << filteredIds;

    /* one filter & signal*/
    filters.clear();
    filteredIds.clear();
    filters.append(CANFltObserver{ids[0], 0xFFFF, nullptr});
    filteredIds.append(ids[0]);
    QTest::newRow("1filtersignal")          << filters << false << true << filteredIds;

    /* 3 filters */
    filters.clear();
    filteredIds.clear();
    foreach(quint32 id, ids) {
        filters.append(CANFltObserver{id, 0xFFFF, nullptr});
        filteredIds.append(id);
    }
    QTest::newRow("3filters")               << filters << false << false << filteredIds;
}


void TestCanCon::filter()
{
    CANConnection* conn_p = nullptr;
    if (!pCreate(conn_p)) {
        QSKIP("No hardware device available");
    }

    QFETCH(QVector<CANFltObserver>, filters);
    QFETCH(bool, filterOut);
    QFETCH(bool, signalReceived);
    QFETCH(QVector<quint32>, filtered);

    /* start connection */
    conn_p->start();

    /* spy signal */
    QSignalSpy spy(conn_p, SIGNAL(targettedFrameReceived(CANFrame)));

    /* configure */
    QVERIFY(pConfig(conn_p));

    LFQueue<CANFrame>& queue = conn_p->getQueue();

    /* wait for frames to arrive */
    QTest::qWait(1000);

    if(signalReceived)
        QVERIFY(spy.count()>0);

    int i;
    for(i=0 ; queue.peek() && i<1000 ; i++)
    {
        CANFrame* canf_p = queue.peek();
        QVERIFY(pValidateFrame(conn_p, canf_p));

        if(filterOut)
            QVERIFY(filtered.contains(canf_p->frameId()));

        queue.dequeue();
    }

    QVERIFY(i>0);

    conn_p->stop();
    delete conn_p;
}


void TestCanCon::write()
{
    CANConnection* conn_p = nullptr;
    if (!pCreate(conn_p)) {
        QSKIP("No hardware device available");
    }

    /* start connection */
    conn_p->start();

    /* configure */
    QVERIFY(pConfig(conn_p));

    QList<CANFrame> frames;
    /* build frames */
    CANFrame frame;
    frame.bus       = 0;
    frame.setFrameId(0x1DE);
    frame.setPayload(QByteArray::fromHex("DEAD C0DE"));

    frames.append(frame);

    frame.setPayload(QByteArray::fromHex("DEAD BEEF"));
    frames.append(frame);

    frame.setPayload(QByteArray::fromHex("DEAD DEAD"));

    /* bad bus id */
    int oldVal = frame.bus;
    frame.bus = 48;
    QCOMPARE(conn_p->sendFrame(frame), false);
    frame.bus       = oldVal;

    qDebug() << "Sending DE AD DE AD";
    /* send */
    QVERIFY(conn_p->sendFrame(frame));

    /* leave some time for the frame to be sent */
    QTest::qWait(1000);

    qDebug() << "Sending DE AD C0 DE";
    qDebug() << "Sending DE AD BE EF";
    /* send */
    QVERIFY(conn_p->sendFrames(frames));

    /* leave some time for the frame to be sent */
    QTest::qWait(1000);

    conn_p->stop();
    delete conn_p;
}


/*********************************************************/

bool TestCanCon::pCreate(CANConnection*& pConn_p)
{
    pConn_p = CanConFactory::create(mType, mPortName, "", 115200, 500000, false, 0);
    if (!pConn_p) return false;

    QCOMPAREB(pConn_p->getPort(),     mPortName);
    QCOMPAREB(pConn_p->getNumBuses(), mNbBus);
    QCOMPAREB(pConn_p->getType(),     mType);
    QCOMPAREB(pConn_p->getStatus(),   CANCon::NOT_CONNECTED);

    return true;
}

bool TestCanCon::pConfig(CANConnection* pConn_p)
{
    /*configure buses */
    CANBus bus;
    CANBus retBus;
    for(int i=0 ; i<pConn_p->getNumBuses() ; i++)
    {
        /* TODO: fix configuration */
        bus.setActive(true);
        pConn_p->setBusSettings(i, bus);
        QVERIFYB(pConn_p->getBusSettings(i, retBus));
        QCOMPAREB(bus, retBus);
    }

    return true;
}

bool TestCanCon::pValidateFrame(CANConnection* pConn_p, CANFrame* pCan_p)
{
    QVERIFYB( pCan_p );
    QVERIFYB( (0<=pCan_p->bus) && (pCan_p->bus <= pConn_p->getNumBuses()) );
    QVERIFYB( pCan_p->isReceived);
    QVERIFYB( pCan_p->payload().length()<=8 );
    QVERIFYB( (0<=pCan_p->frameId()) && (pCan_p->frameId()<2048) );

    return true;
}
