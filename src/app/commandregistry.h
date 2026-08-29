#ifndef COMMANDREGISTRY_H
#define COMMANDREGISTRY_H

// SavvyLens headers
#include "analysis/selectioncontext.h"

// Qt headers
#include <QKeySequence>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QVector>

// C++ standard-library headers
#include <functional>

namespace App {

enum class CommandCategory {
    Navigation,
    Capture,
    Analysis,
    Visualization,
    Automation,
    Settings,
    Help
};

struct Command
{
    QString id;
    QString title;
    QString description;
    CommandCategory category = CommandCategory::Navigation;
    QKeySequence shortcut;
    QString iconName;
    QStringList tags;
    bool requiresSelection = false;

    // Optional context predicate: returns true if command is active/enabled for the context
    std::function<bool(const SelectionContext &)> isEnabledPredicate;

    // Action callback to execute the command
    std::function<void(const QVariantMap &)> handler;

    bool isValid() const noexcept { return !id.isEmpty() && !title.isEmpty() && handler != nullptr; }
};

class CommandRegistry : public QObject
{
    Q_OBJECT

public:
    explicit CommandRegistry(QObject *parent = nullptr);

    bool registerCommand(const Command &command);
    bool unregisterCommand(const QString &commandId);
    void clear();

    std::size_t commandCount() const noexcept;
    bool hasCommand(const QString &commandId) const noexcept;
    Command findCommand(const QString &commandId) const;

    QVector<Command> allCommands() const;
    QVector<Command> commandsByCategory(CommandCategory category) const;
    QVector<Command> searchCommands(const QString &filterText) const;
    QVector<Command> availableCommands(const SelectionContext &context) const;

    bool executeCommand(const QString &commandId, const QVariantMap &args = QVariantMap());

signals:
    void commandRegistered(const QString &commandId);
    void commandUnregistered(const QString &commandId);
    void commandExecuted(const QString &commandId);

private:
    QHash<QString, Command> commands_;
};

} // namespace App

#endif // COMMANDREGISTRY_H
