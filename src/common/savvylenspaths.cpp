#include "common/savvylenspaths.h"

// QT headers
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>

QString SavvyLensPaths::workspaceRoot()
{
    QString documentsPath = QStandardPaths::writableLocation(
        QStandardPaths::DocumentsLocation);

    if (documentsPath.isEmpty())
    {
        documentsPath = QStandardPaths::writableLocation(
            QStandardPaths::HomeLocation);
    }

    return QDir(documentsPath).filePath(QStringLiteral("SavvyLens"));
}

QString SavvyLensPaths::capturesDir()
{
    return QDir(workspaceRoot()).filePath(QStringLiteral("captures"));
}

QString SavvyLensPaths::dbcDir()
{
    return QDir(workspaceRoot()).filePath(QStringLiteral("dbc"));
}

QString SavvyLensPaths::definitionsDir()
{
    return QDir(workspaceRoot()).filePath(QStringLiteral("definitions"));
}

QString SavvyLensPaths::exportsDir()
{
    return QDir(workspaceRoot()).filePath(QStringLiteral("exports"));
}

QString SavvyLensPaths::templatesDir()
{
    return QDir(workspaceRoot()).filePath(QStringLiteral("templates"));
}

QString SavvyLensPaths::logsDir()
{
    const QString appDataPath = QStandardPaths::writableLocation(
        QStandardPaths::AppLocalDataLocation);

    return QDir(appDataPath).filePath(QStringLiteral("logs"));
}

QString SavvyLensPaths::packagedTemplatesDir()
{
    /*
     * Linux installed/AppImage layout:
     *   <prefix>/share/SavvyLens/templates
     *
     * Windows/macOS deployment layout:
     *   <application-dir>/templates
     */
    const QString installedDirectory = QStandardPaths::locate(
        QStandardPaths::GenericDataLocation,
        QStringLiteral("SavvyLens/templates"),
        QStandardPaths::LocateDirectory);

    if (!installedDirectory.isEmpty())
    {
        return installedDirectory;
    }

    return QDir(QCoreApplication::applicationDirPath())
        .filePath(QStringLiteral("templates"));
}

QStringList SavvyLensPaths::templateSearchPaths()
{
    return {templatesDir(), packagedTemplatesDir()};
}

bool SavvyLensPaths::ensureDirectory(const QString &path)
{
    return QDir().mkpath(path);
}

bool SavvyLensPaths::copyMissingFiles(const QString &sourcePath,
                                      const QString &destinationPath)
{
    const QDir sourceDirectory(sourcePath);

    if (!sourceDirectory.exists())
    {
        return false;
    }

    if (!ensureDirectory(destinationPath))
    {
        return false;
    }

    bool success = true;

    const QFileInfoList entries = sourceDirectory.entryInfoList(
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable,
        QDir::Name | QDir::IgnoreCase);

    for (const QFileInfo &entry : entries)
    {
        const QString destination =
            QDir(destinationPath).filePath(entry.fileName());

        if (entry.isDir())
        {
            success = copyMissingFiles(entry.absoluteFilePath(), destination) && success;
            continue;
        }

        /*
         * User workspace files are authoritative. A later application update
         * may add new defaults but never overwrites a user-customized file.
         */
        if (!QFileInfo::exists(destination) &&
            !QFile::copy(entry.absoluteFilePath(), destination))
        {
            success = false;
        }
    }

    return success;
}

bool SavvyLensPaths::seedDefaultTemplates()
{
    return copyMissingFiles(packagedTemplatesDir(), templatesDir());
}

bool SavvyLensPaths::initialize()
{
    bool success = true;

    for (const QString &path : {
             workspaceRoot(),
             capturesDir(),
             dbcDir(),
             definitionsDir(),
             exportsDir(),
             templatesDir(),
             logsDir()})
    {
        success = ensureDirectory(path) && success;
    }

    if (success)
    {
        success = seedDefaultTemplates();
    }

    return success;
}
