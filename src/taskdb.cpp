#include "taskdb.h"

TaskDB::TaskDB()
{
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("taskdata.db3");
    task_db = new QSqlDatabase(db);

    if (!task_db->open())
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
        query.exec("create table invoice_settings\
            (\
                id integer not null\
                    constraint invoice_settings_pk\
                        primary key autoincrement,\
                tax double,\
                user_currency varchar(10),\
                client_currency varchar(10),\
                exchange_rate double,\
                header text,\
                footer text\
                   );");

        query.exec("create unique index invoice_settings_id_uindex on invoice_settings (id);");
        qDebug() << query.lastError().text();
    }
}

TaskDB::~TaskDB()
{
    task_db->close();
}

QSqlDatabase
TaskDB::getDB()
{
    return *task_db;
}

QSqlDatabase
*TaskDB::getDBPtr()
{
    return task_db;
}
