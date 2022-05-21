#ifndef INVOICE_H
#define INVOICE_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTextDocumentWriter>
#include <QPrinter>
#include <QPainter>
#include <QPaintDevice>
#include <QFileDialog>
#include <QMessageBox>
#include <QDate>
#include <QPageSize>


namespace Ui {
class Invoice;
}

class Invoice : public QMainWindow
{
    Q_OBJECT

public:
    explicit Invoice(QSqlDatabase *&database, QWidget *parent = nullptr);
    ~Invoice();

private slots:
    void on_calendarStart_clicked(const QDate &date);
    void on_calendarEnd_clicked(const QDate &date);
    void on_btnGenerate_clicked();

    void on_btnSaveSettings_clicked();

private:
    Ui::Invoice *ui;
    QSqlDatabase *database;

    void initDateWidgets();
    void initComboBoxItems();
    void save_invoice_settings();
    void update_invoice_settings();
    void initInvoiceView();

    [[nodiscard]] double getTax();
    [[nodiscard]] double getExchangeRate();

    [[nodiscard]] bool invoiceSettingsExist();
};

#endif // INVOICE_H
