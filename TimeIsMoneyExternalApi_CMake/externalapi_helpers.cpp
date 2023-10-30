#include "externalapi_helpers.hpp"

[[ nodiscard ]] bool valid_req_data_format(QString &req_data)
{
    auto byte_array = QByteArray::fromStdString(req_data.toStdString());
    QJsonParseError parsing_error;
    
    auto json_doc = QJsonDocument::fromJson(byte_array, &parsing_error);
    
    if (json_doc.isEmpty())
    {
        return false;
    }
    
    // Returns an empty object if the document contains an array.
    auto json_obj = json_doc.object();
    if (json_obj.isEmpty())
    {
        return false;
    }
    
    // If the key does not exist QJsonValue::Undefined is returned.
    auto start_data = json_obj.value("start");
    if (start_data.isUndefined())
    {
        return false;
    }
    
    // If the key does not exist QJsonValue::Undefined is returned.
    auto stop_data = json_obj.value("stop");
    if (stop_data.isUndefined())
    {
        return false;
    }
    
    return true;
}
