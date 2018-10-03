#include <QApplication>
#include <QtGui>
#include "myserver.h"
#include "database.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //MyServer server(2323);
    QString name = "Kate";
    ServDb test("/Users/leyfeld/Documents/projects/databases/table2_db.db");
    test.IsHasClient(name);
   // server.show();
    return app.exec();
}

