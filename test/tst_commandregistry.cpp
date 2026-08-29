#include "tst_commandregistry.h"
#include "app/commandregistry.h"

// Qt headers
#include <QtTest>

using namespace App;

void TestCommandRegistry::registerAndFindCommand()
{
    CommandRegistry registry;

    Command cmd;
    cmd.id = QStringLiteral("re.open_sniffer");
    cmd.title = QStringLiteral("Open Sniffer");
    cmd.category = CommandCategory::Analysis;
    cmd.handler = [](const QVariantMap &) {};

    QVERIFY(registry.registerCommand(cmd));
    QCOMPARE(registry.commandCount(), 1ULL);
    QVERIFY(registry.hasCommand(QStringLiteral("re.open_sniffer")));

    Command retrieved = registry.findCommand(QStringLiteral("re.open_sniffer"));
    QCOMPARE(retrieved.title, QStringLiteral("Open Sniffer"));
}

void TestCommandRegistry::preventDuplicateRegistration()
{
    CommandRegistry registry;

    Command cmd;
    cmd.id = QStringLiteral("nav.open_settings");
    cmd.title = QStringLiteral("Open Settings");
    cmd.handler = [](const QVariantMap &) {};

    QVERIFY(registry.registerCommand(cmd));
    QVERIFY(!registry.registerCommand(cmd)); // Duplicate registration fails
}

void TestCommandRegistry::searchCommandsByTextAndTag()
{
    CommandRegistry registry;

    Command cmd1;
    cmd1.id = QStringLiteral("view.graph");
    cmd1.title = QStringLiteral("Graph Selection");
    cmd1.tags = QStringList{QStringLiteral("plot"), QStringLiteral("signal")};
    cmd1.handler = [](const QVariantMap &) {};
    registry.registerCommand(cmd1);

    Command cmd2;
    cmd2.id = QStringLiteral("view.sniffer");
    cmd2.title = QStringLiteral("Open Sniffer Window");
    cmd2.tags = QStringList{QStringLiteral("traffic")};
    cmd2.handler = [](const QVariantMap &) {};
    registry.registerCommand(cmd2);

    QVector<Command> graphResults = registry.searchCommands(QStringLiteral("plot"));
    QCOMPARE(graphResults.size(), 1);
    QCOMPARE(graphResults[0].id, QStringLiteral("view.graph"));
}

void TestCommandRegistry::filterAvailableCommandsByContext()
{
    CommandRegistry registry;

    Command cmd;
    cmd.id = QStringLiteral("re.inspect_id");
    cmd.title = QStringLiteral("Inspect Selection");
    cmd.requiresSelection = true;
    cmd.handler = [](const QVariantMap &) {};
    registry.registerCommand(cmd);

    SelectionContext emptyContext;
    QCOMPARE(registry.availableCommands(emptyContext).size(), 0);

    SelectionContext activeContext;
    activeContext.canIds.insert(0x123);
    QCOMPARE(registry.availableCommands(activeContext).size(), 1);
}

void TestCommandRegistry::executeCommandWithArguments()
{
    CommandRegistry registry;
    bool executed = false;
    int receivedValue = 0;

    Command cmd;
    cmd.id = QStringLiteral("test.execute");
    cmd.title = QStringLiteral("Test Execute");
    cmd.handler = [&](const QVariantMap &args) {
        executed = true;
        receivedValue = args.value(QStringLiteral("val")).toInt();
    };
    registry.registerCommand(cmd);

    QVariantMap args;
    args.insert(QStringLiteral("val"), 42);

    QVERIFY(registry.executeCommand(QStringLiteral("test.execute"), args));
    QVERIFY(executed);
    QCOMPARE(receivedValue, 42);
}
