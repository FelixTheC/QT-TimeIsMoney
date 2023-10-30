//
// Created by felix on 28.10.23.
//

#ifndef TIMEISMONEYEXTERNALAPI_CMAKE_EXTERNALAPI_DB_HPP
#define TIMEISMONEYEXTERNALAPI_CMAKE_EXTERNALAPI_DB_HPP

#include <QString>
#include <QSqlDatabase>
#include <QSharedPointer>
#include <QSqlQuery>

struct BaseUrlApi
{
    uint pk = 0;
    QString base_url {};
};

bool insert_into_external_api_base_url(QSharedPointer<QSqlDatabase> &db, QString &base_url);

bool update_base_url_external_api_base_url(QSharedPointer<QSqlDatabase> &db, QString &old_val, QString &new_val);

BaseUrlApi get_base_url_external_api_base_url(QSharedPointer<QSqlDatabase> &db);


struct ExternalApiData
{
    QString start_api_path {};
    QString stop_api_path {};
    QString req_data {};
    uint base_url_pk = 0;
    
    bool is_empty();
};

bool insert_into_external_api_data(QSharedPointer<QSqlDatabase> &db, ExternalApiData &api_data);
bool update_external_api_data(QSharedPointer<QSqlDatabase> &db, ExternalApiData &api_data);
ExternalApiData get_external_api_data_by_base_url(QSharedPointer<QSqlDatabase> &db, uint base_url_pk);


struct ExternalApiToken
{
    QString kind {};
    QString token {};
    uint base_url_pk = 0;
    
    bool is_empty();
};

bool insert_into_external_api_token(QSharedPointer<QSqlDatabase> &db, ExternalApiToken &api_token);
bool update_external_api_token(QSharedPointer<QSqlDatabase> &db, ExternalApiToken &api_token);
ExternalApiToken get_external_api_token_by_base_url(QSharedPointer<QSqlDatabase> &db, uint base_url_pk);

#endif //TIMEISMONEYEXTERNALAPI_CMAKE_EXTERNALAPI_DB_HPP https://fyptt.to/tiktok-nudes/
