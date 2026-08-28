// SavvyLens headers
#include "tst_analysissession.h"
#include "tst_cancon.h"
#include "tst_candidateanalysis.h"
#include "tst_canquerylexer.h"
#include "tst_discretestateanalysis.h"
#include "tst_frameaggregatestore.h"
#include "tst_framecomparison.h"
#include "tst_framefileio.h"
#include "tst_framehistory.h"
#include "tst_lfqueue.h"
#include "tst_payloaddiff.h"
#include "tst_rangestatistics.h"
#include "tst_signalresolver.h"
#include "tst_stateexplorerpresentation.h"
#include "tst_temporalanalysis.h"
#include "tst_transitionanalysis.h"

// Qt headers
#include <QtTest>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    int status = 0;
    auto ASSERT_TEST = [&status, argc, argv](QObject *obj) {
        status |= QTest::qExec(obj, argc, argv);
        delete obj;
    };

    ASSERT_TEST(new TestAnalysisSession());
    ASSERT_TEST(new TestCanCon(CANCon::NONE, "none", 1));
    ASSERT_TEST(new TestCandidateAnalysis());
    ASSERT_TEST(new TestCanQueryLexer());
    ASSERT_TEST(new TestDiscreteStateAnalysis());
    ASSERT_TEST(new TestFrameAggregateStore());
    ASSERT_TEST(new TestFrameComparison());
    ASSERT_TEST(new TestFrameHistory());
    ASSERT_TEST(new TestLFQueue());
    ASSERT_TEST(new TestPayloadDiff());
    ASSERT_TEST(new TestRangeStatistics());
    ASSERT_TEST(new TestSignalResolver());
    ASSERT_TEST(new TestStateExplorerPresentation());
    ASSERT_TEST(new TestTemporalAnalysis());
    ASSERT_TEST(new TestTransitionAnalysis());

    return status;
}
