#include "TestRequestMapper.h"

#include <QtTest>
#include <QCoreApplication>
#include <QSettings>
#include <QTemporaryFile>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <QThread>

#include "httplistener.h"
#include "RequestMapper.h"
#include "FakeSimulationSink.h"

#include "nlohmann/json.hpp"

TestRequestMapper::TestRequestMapper()
    : settings(nullptr), settingsFile(nullptr), listener(nullptr),
      mapper(nullptr), sink(nullptr), nam(nullptr), port(0)
{
}

TestRequestMapper::~TestRequestMapper()
{
}

void TestRequestMapper::initTestCase()
{
    // Backing file for QSettings. QtWebApp's HttpListener reads host/port and
    // thread-pool keys directly off the QSettings object (which must already
    // be positioned at the [listener] group, exactly like main.cpp does).
    // QSettings caches values in memory; the file just needs a valid path.
    this->settingsFile = new QTemporaryFile;
    this->settingsFile->open();
    this->settingsFile->close();

    this->settings = new QSettings(this->settingsFile->fileName(), QSettings::IniFormat);
    this->settings->beginGroup("listener");
    this->settings->setValue("host", "127.0.0.1");
    this->settings->setValue("port", 0);  // ephemeral — HttpListener binds, then serverPort() resolves it
    this->settings->setValue("minThreads", 1);
    this->settings->setValue("maxThreads", 4);
    this->settings->setValue("cleanupInterval", 1000);
    this->settings->setValue("readTimeout", 10000);
    this->settings->setValue("maxRequestSize", 16000);
    this->settings->setValue("maxMultiPartSize", 1000000);

    this->sink = new FakeSimulationSink;
    this->mapper = new RequestMapper(this->sink);
    this->listener = new stefanfrings::HttpListener(this->settings, this->mapper);
    this->port = this->listener->serverPort();
    QVERIFY2(this->port > 0, "HttpListener failed to bind an ephemeral port");

    this->nam = new QNetworkAccessManager;
}

void TestRequestMapper::cleanupTestCase()
{
    // Stop any simulations we started so their threads don't outlive the sink.
    this->stopAllStartedSimulations();

    delete this->nam;
    delete this->listener;
    delete this->mapper;
    delete this->sink;
    delete this->settings;
    delete this->settingsFile;

    this->nam = nullptr;
    this->listener = nullptr;
    this->mapper = nullptr;
    this->sink = nullptr;
    this->settings = nullptr;
    this->settingsFile = nullptr;
}

TestRequestMapper::Response TestRequestMapper::post(const QString& path, const QByteArray& body)
{
    QUrl url;
    url.setScheme("http");
    url.setHost("127.0.0.1");
    url.setPort(this->port);
    url.setPath(path);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = this->nam->post(request, body);

    // Drive a local event loop until either the reply completes or the guard
    // timer fires. QNetworkAccessManager is async by design.
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start(5000);
    loop.exec();

    Response response;
    if (!reply->isFinished()) {
        reply->abort();
        response.status = 0;
        response.body = "TIMEOUT";
    } else {
        QVariant s = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        response.status = s.isValid() ? s.toInt() : 0;
        response.body = reply->readAll();
    }
    reply->deleteLater();
    return response;
}

void TestRequestMapper::startMissingId()
{
    // Body without "_id": nlohmann::json::at("_id") throws, caught by the
    // nlohmann::detail::exception handler → 402 with the "field missing" body.
    Response r = this->post("/simulations/start", QByteArray("{}"));
    QCOMPARE(r.status, 402);
    QCOMPARE(r.body, QByteArray("Simulation '_id' field missing"));
}

void TestRequestMapper::startInvalidSimulationJson()
{
    // "_id" + "instance" present, but the rest of the Simulation contract is
    // broken (no url/timeStep/scenery...). Simulation's constructor throws
    // std::runtime_error, caught by the inner catch → 402, body = e.what().
    Response r = this->post("/simulations/start", QByteArray(
        R"({"_id":"sim1","instance":"inst1","primary":true})"));
    QCOMPARE(r.status, 402);
    QVERIFY2(r.body.startsWith("Missing '"),
             QString("Expected a 'Missing ...' runtime_error body, got: %1")
                .arg(QString::fromUtf8(r.body)).toLocal8Bit().constData());
}

void TestRequestMapper::stopUnknownId()
{
    Response r = this->post("/simulations/stop", QByteArray(
        R"({"_id":"nonexistent"})"));
    QCOMPARE(r.status, 412);
    QCOMPARE(r.body, QByteArray("Simulation not paused or running"));
}

void TestRequestMapper::pauseUnknownId()
{
    // Documents the current behavior: pause on an unknown _id hits the early
    // return at RequestMapper.cpp:97-98 BEFORE the isRunning() check, so the
    // status stays at the default 204 with no body. A 412 would be more
    // consistent with the stop path, but this test locks the present contract
    // so the regression is visible the day someone fixes that branch.
    Response r = this->post("/simulations/pause", QByteArray(
        R"({"_id":"nonexistent"})"));
    QCOMPARE(r.status, 204);
    QVERIFY2(r.body.isEmpty(),
             QString("Expected empty body for 204, got: %1")
                .arg(QString::fromUtf8(r.body)).toLocal8Bit().constData());
}

QByteArray TestRequestMapper::validStartBody(const QString& _id, const QString& instance, double totalTime) const
{
    nlohmann::json sim;
    sim["_id"]              = _id.toStdString();
    sim["instance"]         = instance.toStdString();
    sim["url"]              = "url";
    sim["primary"]          = true;
    sim["timeStep"]         = 0.001;
    sim["totalTime"]        = totalTime;
    sim["frameTime"]        = 0.01;
    sim["logTime"]          = 1.0;
    sim["detailedFramesDiv"] = 10;
    sim["calcNeighTimeInt"] = 0.001;
    sim["neighDistThresMult"] = 1.5;
    sim["multiGPU"]         = false;

    nlohmann::json material;
    material["_id"]              = "mat";
    material["distanceThreshold"] = 1.0;

    nlohmann::json object;
    object["_id"]       = "obj";
    object["material"]  = "mat";
    object["density"]   = 1.0;
    object["dimensions"]["spacing"] = 1;
    object["dimensions"]["length"]  = nlohmann::json::array({1.5, 1.5, 1.5});
    object["position"]  = nlohmann::json::array({0.0, 0.0, 0.0});
    object["velocity"]  = nlohmann::json::array({0.0, 0.0, 0.0});

    sim["scenery"]["_id"]       = "scenery";
    sim["scenery"]["gravity"]   = nlohmann::json::array({0.0, 0.0, 0.0});
    sim["scenery"]["materials"] = nlohmann::json::array({material});
    sim["scenery"]["objects"]["nonSolidObjects"] = nlohmann::json::array({object});
    sim["scenery"]["objects"]["solidObjects"]    = nlohmann::json::array({});

    return QByteArray::fromStdString(sim.dump());
}

void TestRequestMapper::waitForSinkFrames(int minCount, int timeoutMs)
{
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    int elapsed = 0;
    while (this->sink->frameCount() < minCount && elapsed < timeoutMs) {
        timer.start(50);
        loop.exec();
        elapsed += 50;
    }
}

void TestRequestMapper::stopAllStartedSimulations()
{
    // POST stop for every _id we started. Sims that already finished naturally
    // will return 412 (already removed from the set) — that's fine.
    for (const QString& id : this->startedIds)
        this->post("/simulations/stop", QByteArray::fromStdString(
            nlohmann::json{{"_id", id.toStdString()}}.dump()));

    // Give the handler threads time to process deleteLater() so no Simulation
    // accesses the sink after we delete it.
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(3000);
    loop.exec();
}

void TestRequestMapper::startAndStopRunningSimulation()
{
    // Full lifecycle: start a long-running sim via HTTP, let it produce at
    // least one frame, then stop it via HTTP. Both responses must be 204.
    QByteArray body = this->validStartBody("lifecycle", "inst1", 10.0);
    this->startedIds.append("lifecycle");

    Response startResp = this->post("/simulations/start", body);
    QCOMPARE(startResp.status, 204);

    // Wait for at least 1 frame to prove the sim actually started and is
    // running on the GPU.
    this->waitForSinkFrames(1, 5000);
    QVERIFY2(this->sink->frameCount() >= 1,
             "Simulation produced no frames after start — GPU sim never began");

    Response stopResp = this->post("/simulations/stop", QByteArray(
        R"({"_id":"lifecycle"})"));
    QCOMPARE(stopResp.status, 204);
}

void TestRequestMapper::instanceInvalidationStopsOldSim()
{
    // Start a sim, then start a new one with the same _id but a different
    // instance. RequestMapper detects the instance mismatch (line 44-48),
    // calls stop() on the old sim, nulls its pointer, and creates the new one.
    // Both starts should return 204 without crashing.
    QByteArray body1 = this->validStartBody("invalidate", "v1", 10.0);
    QByteArray body2 = this->validStartBody("invalidate", "v2", 10.0);
    this->startedIds.append("invalidate");

    Response r1 = this->post("/simulations/start", body1);
    QCOMPARE(r1.status, 204);

    // Let the first sim run briefly so it has a live thread when invalidated.
    this->waitForSinkFrames(1, 5000);

    Response r2 = this->post("/simulations/start", body2);
    QCOMPARE(r2.status, 204);

    // Stop whichever instance is now active.
    Response stopResp = this->post("/simulations/stop", QByteArray(
        R"({"_id":"invalidate"})"));
    QCOMPARE(stopResp.status, 204);
}
