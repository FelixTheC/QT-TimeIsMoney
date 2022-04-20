#include "tasklist.h"


TaskList::TaskList(QSqlDatabase *database, QDialog *parent)
    : QDialog{parent}
{
    db = database;

    auto *clientLabel = new QLabel(this);
    clientLabel->setText("Client:");
    auto *clientField = new QLineEdit(this);

    auto *taskLabel = new QLabel(this);
    taskLabel->setText("Task:");
    auto *taskField = new QLineEdit(this);

    auto *fromLabel = new QLabel(this);
    fromLabel->setText("From:");
    auto *fromField = new QLineEdit(this);

    auto *toLabel = new QLabel(this);
    toLabel->setText("To:");
    auto *toField = new QLineEdit(this);

    connect(clientField, &QLineEdit::textEdited, this, &TaskList::filterClient_changed);
    connect(taskField, &QLineEdit::textEdited, this, &TaskList::filterTask_changed);
    connect(fromField, &QLineEdit::textEdited, this, &TaskList::filterFrom_changed);
    connect(toField, &QLineEdit::textEdited, this, &TaskList::filterTo_changed);

    initModel();
    initTable();

    auto *hLayoutClient = new QHBoxLayout();
    hLayoutClient->addWidget(clientLabel);
    hLayoutClient->addWidget(clientField);

    auto *hLayoutTask = new QHBoxLayout();
    hLayoutTask->addWidget(taskLabel);
    hLayoutTask->addWidget(taskField);

    auto *hLayoutFrom = new QHBoxLayout();
    hLayoutTask->addWidget(fromLabel);
    hLayoutTask->addWidget(fromField);

    auto *hLayoutTo = new QHBoxLayout();
    hLayoutTask->addWidget(toLabel);
    hLayoutTask->addWidget(toField);

    auto *boxLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    boxLayout->addItem(hLayoutClient);
    boxLayout->addItem(hLayoutTask);
    boxLayout->addItem(hLayoutFrom);
    boxLayout->addItem(hLayoutTo);
    boxLayout->addWidget(table);

}

void
TaskList::resizeEvent(QResizeEvent *)
{
    table->resize(this->width(), this->height());
}

void
TaskList::initModel()
{
    model = new QSqlTableModel(nullptr, *db);
    model->setTable("tasks");
    model->setSort(6, Qt::DescendingOrder);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Id"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Name"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Task"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Price"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Probono"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Start"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("End"));
    model->select();
}

void
TaskList::initTable()
{
    table = new QTableView(this);
    table->setShowGrid(true);
    table->setModel(this->model);
    table->resizeColumnsToContents();
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void
TaskList::filter_changed()
{
    db->open();
    QString query = "";
    if (!clientSearch.isEmpty())
        query += QString("client LIKE '%%1%'").arg(clientSearch);
    if (!taskSearch.isEmpty())
        query += QString("task LIKE '%%1%'").arg(taskSearch);
    if(!fromSearch.isEmpty())
        query += QString("created_at >= '%1'").arg(fromSearch);
    if(!toSearch.isEmpty())
        query += QString("closed_at <= '%1'").arg(toSearch);

    if (query.isEmpty())
        model->setFilter("id > 0");
    else
        model->setFilter(query);

    model->select();
    db->close();
}

void
TaskList::filterClient_changed(const QString &value)
{
    clientSearch = value;
    filter_changed();
}

void
TaskList::filterTask_changed(const QString &value)
{
    taskSearch = value;
    filter_changed();
}

void
TaskList::filterFrom_changed(const QString &value)
{
    fromSearch = value;
    filter_changed();
}

void
TaskList::filterTo_changed(const QString &value)
{
    toSearch = value;
    filter_changed();
}

