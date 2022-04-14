#include "taskdb.h"

TaskDB::TaskDB()
{
    task_db = QSqlDatabase::addDatabase("QSQLITE");
    task_db.setDatabaseName("taskdata.db3");

    if (!task_db.open())
    {
        qWarning("Connection error: No connection to DB.");
    }
    else
    {
        qDebug() << "Connection established";
        QSqlQuery query;
        query.exec("create table if not exists tasks"
                   "("
                   "id             integer      not null constraint tasks_pk primary key autoincrement,"
                   "client         varchar(255) not null,"
                   "task           varchar(255) not null,"
                   "price_per_hour DOUBLE(5,2),"
                   "probono        bool,"
                   "created_at     datetime,"
                   "closed_at      datetime"
                   ");");
        query.exec("create index tasks_client_index on tasks (client);");
        query.exec("create index tasks_client_task_index on tasks (client, task);");
        query.exec("create index tasks_closed_at_index on tasks (closed_at desc);");
        query.exec("create index tasks_created_at_index on tasks (created_at);");
        query.exec("create unique index tasks_id_uindex on tasks (id);");
        qDebug() << query.lastError().text();
    }
}

TaskDB::~TaskDB()
{
    qDebug() << "Connection closed";
    task_db.close();
}

QSqlDatabase
TaskDB::getDB()
{
    return task_db;
}
