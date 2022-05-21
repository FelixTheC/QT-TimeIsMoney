#ifndef TASKLIST_H
#define TASKLIST_H

#include <QAbstractItemView>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSqlTableModel>
#include <QTableView>

#include <taskdb.h>


class TaskList : public QDialog
{
    Q_OBJECT
public:
    explicit TaskList(QSqlDatabase *&database, QDialog *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *);

private:
    QTableView *table;
    QSqlTableModel *model;
    QSqlDatabase *db;

    QString clientSearch = "";
    QString taskSearch = "";
    QString fromSearch = "";
    QString toSearch = "";

    void initModel();
    void initTable();
    void filter_changed();


private Q_SLOTS:

    void filterClient_changed(const QString &value);
    void filterTask_changed(const QString &value);
    void filterFrom_changed(const QString &value);
    void filterTo_changed(const QString &value);

};

#endif // TASKLIST_H
