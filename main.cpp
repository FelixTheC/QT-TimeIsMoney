#include "externalapi.hpp"

#include <QApplication>
#include "../TimeIsMoneyDB_CMake/include/taskdb.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    auto db = TaskDB();
    
    auto dbptr = db.getDBPtr();
    ExternalApi w {dbptr};
    w.show();
    return a.exec();
}
