#include "externalapi_db.hpp"


bool insert_into_external_api_base_url(QSharedPointer<QSqlDatabase> &db, QString &base_url)
{
    db->open();
    
    if (db->isOpen() && !base_url.isEmpty())
    {
        QSqlQuery query;
        query.prepare("INSERT INTO api_base_url (base_url) VALUES(:base_url)");
        query.bindValue(":base_url", base_url);
        
        auto result = query.exec();
        
        db->close();
        
        return result;
    }
    return false;
}

bool update_base_url_external_api_base_url(QSharedPointer<QSqlDatabase> &db, QString &old_val, QString &new_val)
{
    db->open();
    
    if (db->isOpen() && !new_val.isEmpty())
    {
        QSqlQuery query;
        query.prepare("update api_base_url set base_url = :new_val where base_url = :old_val");
        query.bindValue(":new_val", new_val);
        query.bindValue(":old_val", old_val);
        
        auto result = query.exec();
        
        db->close();
        
        return result;
    }
    return false;
}

BaseUrlApi get_base_url_external_api_base_url(QSharedPointer<QSqlDatabase> &db)
{
    BaseUrlApi result {};
    
    if (db && db->open())
    {
        QSqlQuery query;
        auto res = query.exec("select id, base_url from api_base_url limit 1;");

        if (res)
        {
            query.first();
            result.pk = query.value(0).toInt();
            result.base_url = query.value(1).toString();
        }
        
        db->close();
    }
    
    return result;
}

bool insert_into_external_api_data(QSharedPointer<QSqlDatabase> &db, ExternalApiData &api_data)
{
    db->open();
    
    if (db->isOpen() && !api_data.is_empty())
    {
        QSqlQuery query;
        query.prepare("insert into api_data (start_api, stop_api, req_dat, base_url_pk)"
                      "values (:start_path, :stop_path, :req_data, :base_url_pk);");
        query.bindValue(":start_path", api_data.start_api_path);
        query.bindValue(":stop_path", api_data.stop_api_path);
        query.bindValue(":req_data", api_data.req_data);
        query.bindValue(":base_url_pk", api_data.base_url_pk);
        
        auto result = query.exec();
        
        db->close();
        
        return result;
    }
    return false;
}

bool update_external_api_data(QSharedPointer<QSqlDatabase> &db, ExternalApiData &api_data)
{
    db->open();
    
    if (db->isOpen() && !api_data.is_empty())
    {
        QSqlQuery query;
        query.prepare("update api_data "
                      "set start_path = :start_path "
                      "set stop_path = :stop_path "
                      "set req_data = :req_data "
                      "where base_url_pk = :base_url_pk");
        query.bindValue(":start_path", api_data.start_api_path);
        query.bindValue(":stop_path", api_data.stop_api_path);
        query.bindValue(":req_data", api_data.req_data);
        query.bindValue(":base_url_pk", api_data.base_url_pk);
        
        auto result = query.exec();
        
        db->close();
        
        return result;
    }
    return false;
}

ExternalApiData get_external_api_data_by_base_url(QSharedPointer<QSqlDatabase> &db, uint base_url_pk)
{
    ExternalApiData result {};
    db->open();
    
    if (db->isOpen())
    {
        QSqlQuery query;
        query.exec("select start_path, stop_path, req_data, base_url_pk "
                   "from api_data where base_url_pk = :base_url_pk limit 1;");
        query.bindValue(":base_url_pk", base_url_pk);
        
        if (query.first())
        {
            result.start_api_path = query.value(0).toString();
            result.stop_api_path = query.value(1).toString();
            result.req_data = query.value(2).toString();
            result.base_url_pk = query.value(3).toInt();
        }
        
        db->close();
    }
    
    return result;
}

bool insert_into_external_api_token(QSharedPointer<QSqlDatabase> &db, ExternalApiToken &api_token)
{
    db->open();
    
    if (db->isOpen() && !api_token.is_empty())
    {
        QSqlQuery query;
        query.prepare("insert into api_token (kind, token, base_url_pk) "
                      "VALUES(:kind, :token, :base_url_pk)");
        query.bindValue(":kind", api_token.kind);
        query.bindValue(":token", api_token.token);
        query.bindValue(":base_url_pk", api_token.base_url_pk);
        
        auto result = query.exec();
        
        db->close();
        
        return result;
    }
    return false;
}

bool update_external_api_token(QSharedPointer<QSqlDatabase> &db, ExternalApiToken &api_token)
{
    db->open();
    
    if (db->isOpen() && !api_token.is_empty())
    {
        QSqlQuery query;
        query.prepare("update api_token "
                      "set kind = :kind "
                      "set token = :token "
                      "where base_url_pk = :base_url_pk");
        query.bindValue(":kind", api_token.kind);
        query.bindValue(":token", api_token.token);
        query.bindValue(":base_url_pk", api_token.base_url_pk);
        
        auto result = query.exec();
        
        db->close();
        
        return result;
    }
    return false;
}

ExternalApiToken get_external_api_token_by_base_url(QSharedPointer<QSqlDatabase> &db, uint base_url_pk)
{
    ExternalApiToken result {};
    db->open();
    
    if (db->isOpen())
    {
        QSqlQuery query;
        query.exec("select kind, token, base_url_pk "
                   "from api_token where base_url_pk = :base_url_pk limit 1;");
        query.bindValue(":base_url_pk", base_url_pk);
        
        if (query.first())
        {
            result.kind = query.value(0).toString();
            result.token = query.value(1).toString();
            result.base_url_pk = query.value(2).toInt();
        }
        
        db->close();
    }
    
    return result;
}

bool ExternalApiData::is_empty()
{
    return start_api_path.isEmpty() && stop_api_path.isEmpty() && req_data.isEmpty();
}

bool ExternalApiToken::is_empty()
{
    return token.isEmpty() && kind.isEmpty();
}
