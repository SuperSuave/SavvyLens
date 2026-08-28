#ifndef SAVVYLENS_PATHS_H
#define SAVVYLENS_PATHS_H

#include <QString>
#include <QStringList>

class SavvyLensPaths
{
public:
    static QString workspaceRoot();
    static QString capturesDir();
    static QString dbcDir();
    static QString definitionsDir();
    static QString exportsDir();
    static QString templatesDir();
    static QString logsDir();

    static QString packagedTemplatesDir();
    static QStringList templateSearchPaths();

    static bool initialize();
    static bool seedDefaultTemplates();

private:
    static bool ensureDirectory(const QString &path);
    static bool copyMissingFiles(const QString &sourcePath,
                                 const QString &destinationPath);
};

#endif // SAVVYLENS_PATHS_H
