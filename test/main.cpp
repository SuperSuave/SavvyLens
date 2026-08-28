// SavvyLens headers
#include "tst_lfqueue.h"
#include "tst_cancon.h"
#include "tst_framefileio.h"

// QT headers
#include <QtTest>

int main(int argc, char** argv)
{
   QCoreApplication app(argc, argv);

   int status = 0;
   auto ASSERT_TEST = [&status, argc, argv](QObject* obj) {
     status |= QTest::qExec(obj, argc, argv);
     delete obj;
   };

   ASSERT_TEST(new TestLFQueue());
   ASSERT_TEST(new TestCanCon(CANCon::NONE, "none", 1));
   ASSERT_TEST(new TestFrameFileIO());

   return status;
}
