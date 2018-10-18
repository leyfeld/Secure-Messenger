#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QWindow>
#include <QQmlContext>
#include "chatprotocol.h"
#include "qmlconnect.h"
#include <QFile>
#include <QByteArray>


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Material");
    QQmlApplicationEngine engine;
    qmlConnect obj;
    engine.rootContext()->setContextProperty("qmlConnection", &obj);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;
    QObject* myroot = engine.rootObjects()[0];
    obj.SetRootObj(myroot);
    return app.exec();
}

