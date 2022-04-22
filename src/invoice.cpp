#include "invoice.h"
#include "ui_invoice.h"
#include <QSqlError>

Invoice::Invoice(QSqlDatabase *database, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Invoice)
{
    this->database = database;

    ui->setupUi(this);
    ui->comboBoxClients->addItem("");

    initDateWidgets();
    initComboBoxItems();
    initInvoiceView();
}

Invoice::~Invoice()
{
    delete ui;
}

void
Invoice::on_calendarStart_clicked(const QDate &date)
{
    // don't allow an end date before start date
    if (ui->calendarEnd->selectedDate() < date)
    {
        ui->calendarEnd->setMinimumDate(date);
        ui->calendarEnd->setSelectedDate(date);
        ui->label_endDate->setText(date.toString(Qt::ISODate));
    }

    if (ui->calendarEnd->minimumDate() > date)
    {
        ui->calendarEnd->setMinimumDate(date);
    }

    auto label = ui->label_startDate;
    label->setText(date.toString(Qt::ISODate));
}


void
Invoice::on_calendarEnd_clicked(const QDate &date)
{
    auto label = ui->label_endDate;
    label->setText(date.toString(Qt::ISODate));
}


void
Invoice::on_btnGenerate_clicked()
{
    QString fileName = QFileDialog::getSaveFileName((QWidget* )0, "Export PDF", QString(), "*.pdf");
    if (QFileInfo(fileName).suffix().isEmpty())
    {
        fileName.append(".pdf");
    }

    auto client = ui->comboBoxClients->currentText();
    auto startDate = ui->calendarStart->selectedDate().toString(Qt::ISODate);
    auto endDate = ui->calendarEnd->selectedDate().toString(Qt::ISODate);
    auto userCurrency = ui->lineEdit_userCurrency->text();
    double totalPrice = 0;

    auto query_string = QString("select date,\
        round(sum(duration) * price_per_hour, 2) as total_price,\
        sum(duration) as total_duration,\
        price_per_hour,\
        task\
        from (\
            select strftime('%d', created_at) as day,\
                   strftime('%Y-%m-%d', created_at) as date,\
                   round((strftime('%s', max(closed_at)) - strftime('%s', min(created_at))) / 3600.0, 2) AS duration,\
                   price_per_hour,\
                   task\
             from tasks\
                   where client='%1'\
                   and created_at>='%2'\
                   and closed_at<='%3'\
             group by id, day, task\
             order by closed_at, strftime(closed_at, '%H-%M')\
            )\
        group by day, task;").arg(client, startDate, endDate);

    database->open();

    QSqlQuery query;
    query.exec(query_string);

    auto table = QString("<div>"
                         "<table width=100%>"
                         "<thead align=center>"
                         "<tr>"
                         "<th>Date</th>"
                         "<th>Task</th>"
                         "<th>Price(%1) p.h.</th>"
                         "<th>Duration(h)</th>"
                         "<th>Sum(%1)</th>"
                         "</tr>"
                         "</thead>"
                         "<tbody align=justify>").arg(userCurrency);

    while (query.next())
    {
        table.append(QString("<tr>"
                             "<td>%1</td>"
                             "<td>%2</td>"
                             "<td>%3</td>"
                             "<td>%4</td>"
                             "<td>%5</td>"
                             "</tr>"
                            ).arg(
                            query.value(0).toString(),
                            query.value(4).toString(),
                            query.value(3).toString(),
                            query.value(2).toString(),
                            query.value(1).toString()
                            )
                    );
        totalPrice += query.value(1).toDouble();
    }

    database->close();

    auto currency = ui->lineEdit_clientCurrency->text();
    if (currency != userCurrency)
    {
        bool *ok = new bool(false);
        auto exchange = ui->lineEdit_exchange->text().toDouble(ok);
        if (ok)
        {
            totalPrice *= exchange;
        }
    }


    bool *ok = new bool(false);
    double tax = 1;
    auto ttax = ui->lineEdit_tax->text().toDouble(ok);
    if (ok)
    {
        tax += (ttax / 100);
    }

    auto priceWithTax = QString::number(totalPrice * tax, 'f', 2);

    table.append("</tbody></table></div><p> </p><p> </p>");
    table.append(QString("<div align=right>Total price: <b>%1 %2</b></div>").arg(QString::number(totalPrice, 'f', 2), currency));
    table.append(QString("<div align=right>Total price with tax(%3): <b>%1 %2</b></div>"
                         ).arg(priceWithTax,
                          currency,
                          QString::number(ttax, 'f', 2)
                          ));

    QString html =
        QString("<div align=right>"
                "%1"
                "</div>"
                "%2"
                "</div>"
                "<h1 align=center>Invoice %3</h1>"
                "<p> </p>"
                "%4"
                "%5").arg(
                QDate::currentDate().toString(Qt::ISODate),
                ui->textEdit_header->toHtml(),
                client,
                table,
                ui->textEdit_footer->toHtml());

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(5, 5, 5, 5));
    printer.setOutputFileName(fileName);

    QTextDocument htmlDoc;
    htmlDoc.setHtml(html);
    htmlDoc.print(&printer);

    QMessageBox::information(this, "File saved", QString("File saved under: %1").arg(fileName));

}

void
Invoice::initDateWidgets()
{
    auto currentDate = QDate::currentDate();
    auto nextDay = currentDate.addDays(1);

    ui->calendarStart->setSelectedDate(currentDate);
    ui->calendarEnd->setSelectedDate(nextDay);
    ui->calendarEnd->setMinimumDate(currentDate);

    on_calendarStart_clicked(currentDate);
    on_calendarEnd_clicked(nextDay);
}

void
Invoice::initComboBoxItems()
{
    database->open();

    QSqlQuery query;
    query.exec("SELECT DISTINCT client FROM tasks");

    while (query.next())
    {
        ui->comboBoxClients->addItem(query.value(0).toString());
    }

    database->close();
}

void
Invoice::save_invoice_settings()
{
    database->open();

    QSqlQuery query;
    query.prepare("INSERT INTO invoice_settings (tax, "
                  "user_currency, "
                  "client_currency, "
                  "exchange_rate, "
                  "header, "
                  "footer) "
                  "VALUES(:tax, :uc, :cc, :exr, :header, :footer)");
    query.bindValue(":tax", getTax());
    query.bindValue(":uc", ui->lineEdit_userCurrency->text());
    query.bindValue(":cc", ui->lineEdit_clientCurrency->text());
    query.bindValue(":exr", getExchangeRate());
    query.bindValue(":header", ui->textEdit_header->toHtml());
    query.bindValue(":footer", ui->textEdit_footer->toHtml());
    query.exec();

    database->close();
}

void
Invoice::update_invoice_settings()
{
    database->open();

    QSqlQuery query;
    query.prepare("UPDATE invoice_settings "
                  "SET tax=:tax, "
                  "user_currency=:uc, "
                  "client_currency=:cc, "
                  "exchange_rate=:exr, "
                  "header=:header, "
                  "footer=:footer "
                  "WHERE id>0");
    query.bindValue(":tax", getTax());
    query.bindValue(":uc", ui->lineEdit_userCurrency->text());
    query.bindValue(":cc", ui->lineEdit_clientCurrency->text());
    query.bindValue(":exr", getExchangeRate());
    query.bindValue(":header", ui->textEdit_header->toHtml());
    query.bindValue(":footer", ui->textEdit_footer->toHtml());
    query.exec();

    database->close();
}

double
Invoice::getTax()
{
    bool *ok = new bool(false);
    double tax = ui->lineEdit_tax->text().toDouble(ok);
    if (ok)
        return tax;
    return 1.00;
}

double
Invoice::getExchangeRate()
{
    bool *ok = new bool(false);
    double rate = ui->lineEdit_exchange->text().toDouble(ok);
    if (ok)
        return rate;
    return 1.00;
}

bool
Invoice::invoiceSettingsExist()
{
    database->open();

    QSqlQuery query;
    query.exec("SELECT COUNT(*) FROM invoice_settings");
    query.first();
    auto result = query.value(0).toInt();

    database->close();

    return result > 0;
}

void
Invoice::initInvoiceView()
{
    if (invoiceSettingsExist())
    {
        database->open();

        QSqlQuery query;
        query.exec("SELECT * FROM invoice_settings LIMIT 1");
        query.first();

        ui->lineEdit_tax->setText(query.value(1).toString());
        ui->lineEdit_userCurrency->setText(query.value(2).toString());
        ui->lineEdit_clientCurrency->setText(query.value(3).toString());
        ui->lineEdit_exchange->setText(query.value(4).toString());
        ui->textEdit_header->setText(query.value(5).toString());
        ui->textEdit_footer->setText(query.value(6).toString());

        database->close();
    }
}

void Invoice::on_btnSaveSettings_clicked()
{
    if (invoiceSettingsExist())
        update_invoice_settings();
    else
        save_invoice_settings();
}

