#ifndef TST_COMMANDREGISTRY_H
#define TST_COMMANDREGISTRY_H

// Qt headers
#include <QObject>

class TestCommandRegistry : public QObject
{
    Q_OBJECT

private slots:
    void registerAndFindCommand();
    void preventDuplicateRegistration();
    void searchCommandsByTextAndTag();
    void filterAvailableCommandsByContext();
    void executeCommandWithArguments();
};

#endif // TST_COMMANDREGISTRY_H
