#include <QCoreApplication>
#include <QDir>
#include <QString>
#include <QLoggingCategory>

#include <OpenCLWrapper/OCLW_Core.hpp>

#include "Common.h"
#include "httplistener.h"
#include "RequestMapper.h"
#include "Simulation.h"

#include <iostream>

using namespace stefanfrings;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    std::setlocale(LC_NUMERIC, "C");

    OpenCLWrapper::Core::initialize();

    QString configFileName = Common::searchConfigFile();

    // Read logging configuration
    QSettings* loggingSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
    loggingSettings->beginGroup("logging");
    bool enableHttpDebug = loggingSettings->value("enableHttpDebug", false).toBool();
    loggingSettings->endGroup();

    // Conditionally disable Qt debug output to reduce HTTP logging noise
    if (!enableHttpDebug) {
        QLoggingCategory::setFilterRules("*.debug=false");
    }

    QSettings* listenerSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
    listenerSettings->beginGroup("listener");
    new HttpListener(listenerSettings, new RequestMapper(&app), &app);

    return app.exec();
}
