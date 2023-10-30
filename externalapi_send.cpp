#include "externalapi_send.hpp"

/// @brief Send a start ot stop cmd to the external API.
/// @param db pointer to a QSqlDatabase
/// @param net_manager the QNetworkAccessManager as parameter to be able to use signals.
/// @param send_topic send either `start` or `stop` to external api
void send_cmd(QSharedPointer<QSqlDatabase> &db, QSharedPointer<QNetworkAccessManager> &net_manager, SendTopic &send_topic)
{
    auto base_url = get_base_url_external_api_base_url(db);
    
    if (!base_url.base_url.isEmpty())
    {
        auto api_token = get_external_api_token_by_base_url(db, base_url.pk);
        auto api_data = get_external_api_data_by_base_url(db, base_url.pk);
        
        if (!api_token.token.isEmpty() && !api_data.start_api_path.isEmpty() && !api_data.req_data.isEmpty())
        {
            QNetworkRequest net_req {base_url.base_url + api_data.start_api_path};
            
            auto json_doc = QJsonDocument::fromJson(QByteArray::fromStdString(api_data.req_data.toStdString()));
            auto json_obj = json_doc.object();
            QJsonValue json_val;
            
            if (send_topic == SendTopic::Start)
            {
                json_val = json_obj.value("start");
            }
            
            if (send_topic == SendTopic::Stop)
            {
                json_val = json_obj.value("stop");
            }
            
            auto start_cmd_doc = QJsonDocument();
            auto start_cmd_obj = json_val.toObject();
            
            start_cmd_obj.insert(api_token.kind, QJsonValue(api_token.token));
            start_cmd_doc.setObject(start_cmd_obj);
            
            net_manager->post(net_req, start_cmd_doc.toJson());
        }
    }
}
