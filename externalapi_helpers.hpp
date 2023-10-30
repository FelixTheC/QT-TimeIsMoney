#ifndef TIMEISMONEYEXTERNALAPI_CMAKE_EXTERNALAPI_HELPERS_HPP
#define TIMEISMONEYEXTERNALAPI_CMAKE_EXTERNALAPI_HELPERS_HPP

#include <QByteArray>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>

const std::string REQ_DATA_EXAMPLE = R"({"start": {"command": "foo", "user": 123},  "stop": {"command": "foo", "user": 123}})";
[[ nodiscard ]] bool valid_req_data_format(QString &req_data);

#endif //TIMEISMONEYEXTERNALAPI_CMAKE_EXTERNALAPI_HELPERS_HPP
