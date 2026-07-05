#include "Sender.h"

#include "restclient-cpp/connection.h"

Sender::Sender()
{
    RestClient::init();
}

Sender& Sender::getInstance()
{
    static Sender instance; // Guaranteed to be destroyed.
                            // Instantiated on first use.
    return instance;
}

RestClient::Response Sender::send(const std::string &url, const std::string &contentType, const std::string &data)
{
    RestClient::Connection conn = RestClient::Connection(url);

    RestClient::HeaderFields headers;
    headers["Content-Type"] = contentType;
    conn.SetHeaders(headers);

    RestClient::Response response = conn.post("", data);

    return response;

    //return RestClient::post(url, contentType, data);
}
