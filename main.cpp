#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "manager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<Manager>("com.sensor", 1, 0, "Manager");

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    // 加载 QML 文件
    engine.loadFromModule("sensor", "Main");

    return app.exec();
}
