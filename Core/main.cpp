#include <QCoreApplication>
#include <QDir>
#include <QString>

#include "httplistener.h"
#include "OpenCL.h"
#include "RequestMapper.h"
#include "Simulation.h"

#include <iostream>

using namespace stefanfrings;

QString searchConfigFile()
{
    QString binDir=QCoreApplication::applicationDirPath();
    QString appName=QCoreApplication::applicationName();
    QString fileName(appName + ".ini");

    QStringList searchList;
    searchList.append(binDir);
    searchList.append(binDir + "/etc");
    searchList.append(binDir + "/../etc");
    searchList.append(binDir + "/../../etc"); // for development without shadow build
    searchList.append(binDir + "/../" + appName + "/etc"); // for development with shadow build
    searchList.append(binDir + "/../../" + appName + "/etc"); // for development with shadow build
    searchList.append(binDir + "/../../../" + appName + "/etc"); // for development with shadow build
    searchList.append(binDir + "/../../../../" + appName + "/etc"); // for development with shadow build
    searchList.append(binDir + "/../../../../../" + appName + "/etc"); // for development with shadow build
    searchList.append(QDir::rootPath() + "etc/opt");
    searchList.append(QDir::rootPath() + "etc");

    foreach (QString dir, searchList)
    {
        QFile file(dir + "/" + fileName);
        if(file.exists())
        {
            // found
            fileName=QDir(file.fileName()).canonicalPath();
            qDebug("Using config file %s", qPrintable(fileName));
            return fileName;
        }
    }

    // not found
    foreach (QString dir, searchList)
    {
        qWarning("%s/%s not found", qPrintable(dir), qPrintable(fileName));
    }
    qFatal("Cannot find config file %s", qPrintable(fileName));
    return 0;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    std::setlocale(LC_NUMERIC, "C");

    QString configFileName = searchConfigFile();

    QSettings* listenerSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
    listenerSettings->beginGroup("listener");
    new HttpListener(listenerSettings, new RequestMapper(&app), &app);

    return app.exec();
}
