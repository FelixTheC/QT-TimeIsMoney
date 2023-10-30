#ifndef EXTERNALAPI_HPP
#define EXTERNALAPI_HPP

#include <QDialog>
#include <QMessageBox>
#include <QSqlDatabase>

#include "externalapi_db.hpp"
#include "externalapi_helpers.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class ExternalApi; }
QT_END_NAMESPACE

class ExternalApi : public QDialog
{
    Q_OBJECT

private slots:
    void save_settings();
    void close_settings();
    
public:
    explicit ExternalApi(QSharedPointer<QSqlDatabase> &db, QWidget *parent = nullptr);
    ~ExternalApi() override;

private:
    Ui::ExternalApi *ui;
    QSharedPointer<QSqlDatabase> db;
    
    bool has_error;
    bool base_url_exists();
    bool api_token_exists(uint base_url_pk);
    bool api_data_exists(uint base_url_pk);
    
    BaseUrlApi get_base_url();
    ExternalApiToken get_external_api_token(uint base_url_pk);
    ExternalApiData get_external_api_data(uint base_url_pk);
    
    bool update_external_api_token(uint base_url_pk);
    bool update_external_api_data(uint base_url_pk);
    
    bool insert_external_api_token(uint base_url_pk);
    bool insert_external_api_data(uint base_url_pk);
    
    void validate_req_data();
    void display_req_data_warning();
    void add_base_url_validator();
    void add_path_validators();
    void init_data_from_db();
};
#endif // EXTERNALAPI_HPP
