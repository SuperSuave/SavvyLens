// SavvyLens headers
#include "tst_lfqueue.h"
#include "tst_framecomparison.h"
#include "tst_framehistory.h"
#include "tst_frameaggregatestore.h"
#include "tst_payloaddiff.h"

// Qt headers
#include <QtTest>

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  int status = 0;
  auto ASSERT_TEST = [&status, argc, argv](QObject *obj)
  {
    status |= QTest::qExec(obj, argc, argv);
    delete obj;
  };

  ASSERT_TEST(new TestFrameAggregateStore());
  ASSERT_TEST(new TestFrameComparison());
  ASSERT_TEST(new TestFrameHistory());
  ASSERT_TEST(new TestPayloadDiff());

  // This is Last since it takes the longest.
  ASSERT_TEST(new TestLFQueue());

  return status;
}
