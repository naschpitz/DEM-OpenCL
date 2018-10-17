#include "RequestSender.h"

#include <QJsonDocument>
#include "restclient-cpp/restclient.h"

RequestSender::RequestSender()
{
}

void RequestSender::newFrame(QJsonObject frame)
{
    QJsonDocument document(frame);
    QByteArray data = document.toJson();

    RestClient::Response r = RestClient::post("http://127.0.0.1:3000/api/frames/add", "application/json", data.toStdString());
}
