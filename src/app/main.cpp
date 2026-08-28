#include "app/mainwindow.h"

// SavvyLens headers
#include "common/savvylenspaths.h"
#include "mcp/mcpserver.h"
#include "themes/thememanager.h"
#include "utils/logger.h"

// Qt headers
#include <QApplication>
#include <QDebug>
#include <QFileOpenEvent>
#include <QFont>
#include <QLocale>
#include <QSettings>
#include <QTranslator>

class SavvyLensApplication : public QApplication
{
public:
    MainWindow *mainWindow;

    SavvyLensApplication(int &argc, char **argv) : QApplication(argc, argv)
    {
        mainWindow = nullptr;
    }

    bool event(QEvent *event) override
    {
        if (event->type() == QEvent::FileOpen)
        {
            QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
            if (mainWindow) mainWindow->handleDroppedFile(openEvent->file());
        }

        return QApplication::event(event);
    }
};

int main(int argc, char *argv[])
{
    // Initialize crash handler and Qt message logging
    Logger::init();

#ifdef QT_DEBUG
    //uncomment for verbose debug data in application output
    //qputenv("QT_FATAL_WARNINGS", "1");
    //qSetMessagePattern("Type: %{type}\nProduct Name: %{appname}\nFile: %{file}\nLine: %{line}\nMethod: %{function}\nThreadID: %{threadid}\nThreadPtr: %{qthreadptr}\nMessage: %{message}");
#endif

    SavvyLensApplication a(argc, argv);


    //Add a local path for Qt extensions, to allow for per-application extensions.
    a.addLibraryPath("plugins");

    //These things are used by QSettings to set up setting storage
    a.setOrganizationName("SuaveEV");
    a.setApplicationName("SavvyLens");
    a.setOrganizationDomain("github.com/SuperSuave");

    if (!SavvyLensPaths::initialize())
    {
        qWarning() << "Unable to fully initialize the SavvyLens workspace:"
                   << SavvyLensPaths::workspaceRoot();
    }

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;

    QString localeString = settings.value("Main/Language").toString();
    if (localeString.isEmpty()) {
        QLocale sysLocale = QLocale::system();
        localeString = sysLocale.name();
        settings.setValue("Main/Language", localeString);
    }

    QTranslator translator;
    QLocale locale(localeString);
    QString lang = locale.name();
    QString shortLang = locale.name().left(2);

    if (QString translationDir = QCoreApplication::applicationDirPath() + "/translations";
        !translator.load("SavvyLens_" + lang, translationDir)) {
        translator.load("SavvyLens_" + shortLang, translationDir);
    }
    a.installTranslator(&translator);

    qInfo() << "Locale Value is:" << locale.name();

    int fontSize = settings.value("Main/FontSize", 9).toUInt();
    QFont sysFont = QFont();
    sysFont.setPointSize(fontSize);
    a.setFont(sysFont);

    ThemeManager::applyDarkTheme(a);

    a.mainWindow = new MainWindow();
    
    if (settings.value("MCP/Enable", true).toBool()) {
        MCPServer *mcpServer = new MCPServer(&a);
        QObject::connect(mcpServer, &MCPServer::clientCountChanged, a.mainWindow, &MainWindow::updateCopilotStatus);
        mcpServer->start(settings.value("MCP/Port", "8888").toInt());
    }
    
    a.mainWindow->show();
    
    if (argc > 1) {
        a.mainWindow->handleDroppedFile(QString(argv[1]));
    }

    int retCode = a.exec();

    delete a.mainWindow;
    a.mainWindow = nullptr;

    return retCode;
}
