#include "Pigz.h"

#include <QProcess>

#include <iostream>

Pigz::Pigz() {

}

QByteArray Pigz::deflate(const QByteArray& data)
{
    QProcess pigz;
    pigz.setProgram("pigz");
    pigz.setArguments({"-c", "-k", "-9"}); // -c = write to stdout

    pigz.setProcessChannelMode(QProcess::SeparateChannels);
    pigz.start();

    pigz.write(data);
    pigz.closeWriteChannel(); // Signal that we're done sending input

    pigz.waitForFinished(-1); // Wait indefinitely

    QByteArray deflated = pigz.readAllStandardOutput();
    QByteArray errors = pigz.readAllStandardError();

    if (!errors.isEmpty()) {
        std::cout << "pigz stderr: " << errors.toStdString() << std::endl;
    }

    return deflated;
}
