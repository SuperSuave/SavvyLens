#include "commandregistry.h"

namespace App {

CommandRegistry::CommandRegistry(QObject *parent)
    : QObject(parent)
{
}

bool CommandRegistry::registerCommand(const Command &command)
{
    if (!command.isValid()) {
        return false;
    }

    if (commands_.contains(command.id)) {
        return false; // Prevent duplicate IDs
    }

    commands_.insert(command.id, command);
    emit commandRegistered(command.id);
    return true;
}

bool CommandRegistry::unregisterCommand(const QString &commandId)
{
    if (!commands_.contains(commandId)) {
        return false;
    }

    commands_.remove(commandId);
    emit commandUnregistered(commandId);
    return true;
}

void CommandRegistry::clear()
{
    commands_.clear();
}

std::size_t CommandRegistry::commandCount() const noexcept
{
    return static_cast<std::size_t>(commands_.size());
}

bool CommandRegistry::hasCommand(const QString &commandId) const noexcept
{
    return commands_.contains(commandId);
}

Command CommandRegistry::findCommand(const QString &commandId) const
{
    return commands_.value(commandId, Command());
}

QVector<Command> CommandRegistry::allCommands() const
{
    QVector<Command> result;
    result.reserve(commands_.size());
    for (auto it = commands_.constBegin(); it != commands_.constEnd(); ++it) {
        result.append(it.value());
    }
    return result;
}

QVector<Command> CommandRegistry::commandsByCategory(CommandCategory category) const
{
    QVector<Command> result;
    for (auto it = commands_.constBegin(); it != commands_.constEnd(); ++it) {
        if (it.value().category == category) {
            result.append(it.value());
        }
    }
    return result;
}

QVector<Command> CommandRegistry::searchCommands(const QString &filterText) const
{
    if (filterText.trimmed().isEmpty()) {
        return allCommands();
    }

    const QString query = filterText.trimmed().toLower();
    QVector<Command> result;

    for (auto it = commands_.constBegin(); it != commands_.constEnd(); ++it) {
        const Command &cmd = it.value();
        if (cmd.title.toLower().contains(query) ||
            cmd.description.toLower().contains(query) ||
            cmd.id.toLower().contains(query)) {
            result.append(cmd);
            continue;
        }

        bool tagMatched = false;
        for (const QString &tag : cmd.tags) {
            if (tag.toLower().contains(query)) {
                tagMatched = true;
                break;
            }
        }
        if (tagMatched) {
            result.append(cmd);
        }
    }

    return result;
}

QVector<Command> CommandRegistry::availableCommands(const SelectionContext &context) const
{
    QVector<Command> result;
    for (auto it = commands_.constBegin(); it != commands_.constEnd(); ++it) {
        const Command &cmd = it.value();
        if (cmd.requiresSelection && context.canIds().isEmpty()) {
            continue;
        }

        if (cmd.isEnabledPredicate && !cmd.isEnabledPredicate(context)) {
            continue;
        }

        result.append(cmd);
    }
    return result;
}

bool CommandRegistry::executeCommand(const QString &commandId, const QVariantMap &args)
{
    if (!commands_.contains(commandId)) {
        return false;
    }

    const Command &cmd = commands_[commandId];
    if (!cmd.handler) {
        return false;
    }

    cmd.handler(args);
    emit commandExecuted(commandId);
    return true;
}

} // namespace App
