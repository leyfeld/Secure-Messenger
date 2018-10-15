#include <QApplication>
#include "myclient.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    try
    {
        MyClient client("localhost", 2323);
        client.show();
        return app.exec();
    }
    catch (const std::exception& ex)
    {
        qDebug() << ex.what();
    }
}
