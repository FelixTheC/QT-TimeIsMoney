#include "externalapi.hpp"
#include "ui_externalapi.h"

ExternalApi::ExternalApi(QSharedPointer<QSqlDatabase> &db_ptr, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ExternalApi)
{
    ui->setupUi(this);
    db = db_ptr;
    
    connect(ui->externalApiAddButtonBox, SIGNAL(accepted()), this, SLOT(save_settings()));
    connect(ui->externalApiAddButtonBox, SIGNAL(rejected()), this, SLOT(close_settings()));
    
    init_data_from_db();
    
    add_base_url_validator();
    add_path_validators();
}

ExternalApi::~ExternalApi()
{
    delete ui;
}

void ExternalApi::save_settings()
{
    validate_req_data();
    
    auto user_base_url = ui->baseUrlLineEdit->text();
    BaseUrlApi base_url_api {};
    
    if (base_url_exists())
    {
        base_url_api = get_base_url();
        
        if (base_url_api.base_url != ui->baseUrlLineEdit->text())
        {
            update_base_url_external_api_base_url(db, base_url_api.base_url, user_base_url);
        }
    }
    else
    {
        insert_into_external_api_base_url(db, user_base_url);
        base_url_api = get_base_url();
    }
    
    if (api_token_exists(base_url_api.pk))
    {
        update_external_api_token(base_url_api.pk);
    }
    else
    {
        insert_external_api_token(base_url_api.pk);
    }
    
    if (api_data_exists(base_url_api.pk))
    {
        update_external_api_data(base_url_api.pk);
    }
    else
    {
        insert_external_api_data(base_url_api.pk);
    }
    
    db->commit();
    db->close();
    
    if (!has_error)
    {
        QMessageBox msg_box;
        msg_box.setIcon(QMessageBox::Information);
        msg_box.setText("Data saved.");
        msg_box.exec();
        
        ExternalApi::close();
    }
}

void ExternalApi::close_settings()
{
    ExternalApi::close();
}

bool ExternalApi::base_url_exists()
{
    return !get_base_url_external_api_base_url(db).base_url.isEmpty();
}

bool ExternalApi::api_token_exists(uint base_url_pk)
{
    return !get_external_api_token_by_base_url(db, base_url_pk).token.isEmpty();
}

bool ExternalApi::api_data_exists(uint base_url_pk)
{
    return !get_external_api_data_by_base_url(db, base_url_pk).start_api_path.isEmpty();
}

BaseUrlApi ExternalApi::get_base_url()
{
    return get_base_url_external_api_base_url(db);
}

ExternalApiToken ExternalApi::get_external_api_token(uint base_url_pk)
{
    return get_external_api_token_by_base_url(db, base_url_pk);
}

ExternalApiData ExternalApi::get_external_api_data(uint base_url_pk)
{
    return get_external_api_data_by_base_url(db, base_url_pk);
}

bool ExternalApi::update_external_api_token(uint base_url_pk)
{
    auto token_kind = ui->tokenKindComboBox->currentText();
    auto token = ui->tokenValueLineEdit->text();
    
    auto api_token = get_external_api_token(base_url_pk);
    
    if (token != api_token.token || token_kind != api_token.kind)
    {
        api_token.token = token;
        api_token.kind = token_kind;
        
        return ::update_external_api_token(db, api_token);
    }
    
    return false;
}

bool ExternalApi::update_external_api_data(uint base_url_pk)
{
    auto start_path = ui->startApiLineEdit->text();
    auto stop_path = ui->stopApiLineEdit->text();
    auto req_data = ui->requestDataTextEdit->toPlainText();
    
    auto api_data = get_external_api_data(base_url_pk);
    
    if (api_data.start_api_path != start_path ||
        api_data.stop_api_path != stop_path ||
        api_data.req_data != req_data)
    {
        api_data.start_api_path = start_path;
        api_data.stop_api_path = stop_path;
        api_data.req_data = req_data;
        
        ::update_external_api_data(db, api_data);
    }
    
    return false;
}

bool ExternalApi::insert_external_api_token(uint base_url_pk)
{
    auto token_kind = ui->tokenKindComboBox->currentText();
    auto token = ui->tokenValueLineEdit->text();
    
    ExternalApiToken api_token {token_kind, token, base_url_pk};
    
    return ::insert_into_external_api_token(db, api_token);
}

bool ExternalApi::insert_external_api_data(uint base_url_pk)
{
    auto start_path = ui->startApiLineEdit->text();
    auto stop_path = ui->stopApiLineEdit->text();
    auto req_data = ui->requestDataTextEdit->toPlainText();
    
    ExternalApiData api_data {start_path, stop_path, req_data, base_url_pk};
    
    return ::insert_into_external_api_data(db, api_data);
}

void ExternalApi::display_req_data_warning()
{
    QMessageBox msg_box;
    msg_box.setIcon(QMessageBox::Warning);
    msg_box.setText("The format of the `RequestData` is invalid.");
    
    QString txt = "Correct format would be.\n" + QString::fromStdString(REQ_DATA_EXAMPLE);
    msg_box.setDetailedText(txt);
    msg_box.setInformativeText(ui->requestDataTextEdit->toPlainText());
    msg_box.exec();
}

void ExternalApi::add_base_url_validator()
{
    QRegularExpression rx(R"(^http(s?):\/\/\w+.{1}\w{2,6}\/$)");
    QValidator *validator = new QRegularExpressionValidator(rx, this);
    
    ui->baseUrlLineEdit->setValidator(validator);
}

void ExternalApi::add_path_validators()
{
    QRegularExpression api_path_rx(R"(^\w+[\/\w]+)");
    QValidator *api_path_validator = new QRegularExpressionValidator(api_path_rx, this);
    
    ui->startApiLineEdit->setValidator(api_path_validator);
    ui->stopApiLineEdit->setValidator(api_path_validator);
}

void ExternalApi::validate_req_data()
{
    auto req_data = ui->requestDataTextEdit->toPlainText();
    has_error = false;
    
    if (!valid_req_data_format(req_data))
    {
        has_error = true;
        display_req_data_warning();
    }
}

void ExternalApi::init_data_from_db()
{
    if (base_url_exists())
    {
        auto base_url_api = get_base_url();

        ui->baseUrlLineEdit->setText(base_url_api.base_url);

        if (api_token_exists(base_url_api.pk))
        {
            auto api_token = get_external_api_token(base_url_api.pk);
            ui->tokenValueLineEdit->setText(api_token.token);
            auto idx = ui->tokenKindComboBox->findText(api_token.kind);
            ui->tokenKindComboBox->setCurrentIndex(idx);
        }

        if (api_data_exists(base_url_api.pk))
        {
            auto api_data = get_external_api_data(base_url_api.pk);

            ui->startApiLineEdit->setText(api_data.start_api_path);
            ui->stopApiLineEdit->setText(api_data.stop_api_path);
            ui->requestDataTextEdit->setText(api_data.req_data);
        }
    }
}

