#ifndef TASKDB_H
#define TASKDB_H

#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>



class TaskDB
{
public:
    TaskDB();
    ~TaskDB();
    QSqlDatabase getDB();

private:
    QSqlDatabase task_db;
};

#endif // TASKDB_H
