//
// Created by felix on 29.10.23.
//

#ifndef TIMEISMONEYEXTERNALAPI_CMAKE_EXTERNALAPI_SEND_HPP
#define TIMEISMONEYEXTERNALAPI_CMAKE_EXTERNALAPI_SEND_HPP

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QSqlDatabase>
#include <QSharedPointer>

#include "externalapi_db.hpp"

enum class SendTopic
{
    Start,
    Stop
};

/// @brief Send a start ot stop cmd to the external API.
/// @param db pointer to a QSqlDatabase
/// @param net_manager the QNetworkAccessManager as parameter to be able to use signals.
/// @param send_topic send either `start` or `stop` to external api
void send_cmd(QSharedPointer<QSqlDatabase> &db, QSharedPointer<QNetworkAccessManager> &net_manager, SendTopic &send_topic);

#endif //TIMEISMONEYEXTERNALAPI_CMAKE_EXTERNALAPI_SEND_HPP
