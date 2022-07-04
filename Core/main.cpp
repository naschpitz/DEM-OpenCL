#include <QCoreApplication>
#include <QDir>
#include <QString>

#include "Common.h"
#include "httplistener.h"
#include "OpenCL.h"
#include "RequestMapper.h"
#include "Simulation.h"

#include <iostream>

using namespace stefanfrings;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    std::setlocale(LC_NUMERIC, "C");

    QString configFileName = Common::searchConfigFile();

    QSettings* listenerSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
    listenerSettings->beginGroup("listener");
    new HttpListener(listenerSettings, new RequestMapper(&app), &app);

    return app.exec();
}
