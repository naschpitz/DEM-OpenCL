#ifndef TEST_REQUEST_MAPPER_H
#define TEST_REQUEST_MAPPER_H

#include <QObject>
#include <QNetworkAccessManager>

class QSettings;
class QTemporaryFile;
namespace stefanfrings
{
  class HttpListener;
}
class RequestMapper;
class FakeSimulationSink;

// Phase 3: HTTP control-plane integration tests. Drives the real QtWebApp
// HttpListener + RequestMapper over an actual TCP loopback socket with
// QNetworkAccessManager as the client. No service() mocking — every request
// travels the full HTTP round-trip the production server handles.
//
// The listener binds 127.0.0.1:0 (ephemeral port) so tests never collide with
// a running DEM instance and don't need DEM.ini. A FakeSimulationSink is
// injected so no RequestSender singleton (and thus no DEM.ini lookup) is ever
// constructed; the error-path cases below never reach Simulation::run().
class TestRequestMapper : public QObject
{
    Q_OBJECT

  public:
    TestRequestMapper();
    ~TestRequestMapper();

  private slots:
    void initTestCase();
    void cleanupTestCase();

    void startMissingId();
    void startInvalidSimulationJson();
    void stopUnknownId();
    void pauseUnknownId();
    void startAndStopRunningSimulation();
    void instanceInvalidationStopsOldSim();

  private:
    struct Response {
        int status;
        QByteArray body;
    };
    Response post(const QString& path, const QByteArray& body);
    QByteArray validStartBody(const QString& _id, const QString& instance, double totalTime) const;
    void waitForSinkFrames(int minCount, int timeoutMs);
    void stopAllStartedSimulations();

    QSettings* settings;
    QTemporaryFile* settingsFile;
    stefanfrings::HttpListener* listener;
    RequestMapper* mapper;
    FakeSimulationSink* sink;
    QNetworkAccessManager* nam;
    int port;
    QStringList startedIds;
};

#endif
