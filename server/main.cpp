#include <QApplication>
#include <QtGui>
#include "myserver.h"
#include "database.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    try
    {
        MyServer server(2323);
        server.show();
        return app.exec();
    }
    catch (const std::exception& ex)
    {
        qDebug() << ex.what();
    }
}

