#include <QMessageBox>

#include "config.hpp"

Config::Config()
{
    initConfigFileIfNotExists();
    jsonObject = loadConfigFile(nullptr).object();
}

Config::Config(QWidget *parent)
{
    initConfigFileIfNotExists();
    jsonObject = loadConfigFile(parent).object();
}

void
Config::initConfigFileIfNotExists()
{
    QFile loadFile(configFile);
    if (!loadFile.exists())
    {
        if (!loadFile.open(QIODevice::ReadWrite))
        {
            qWarning("Could not open config file.");
            return;
        }

        QJsonObject newJsonConfig = QJsonObject();
        newJsonConfig["Port"] = "/dev/ttyACM0";
        newJsonConfig["Baudrate"] = 9600;

        auto jsonDoc = QJsonDocument(newJsonConfig);

        loadFile.write(jsonDoc.toJson());
        loadFile.close();
    }
}

QJsonDocument
Config::loadConfigFile(QWidget *parent)
{
    QFile loadFile(configFile);
    QJsonDocument jsonConfig;

    if (!loadFile.open(QIODevice::ReadOnly))
    {
        if (parent == nullptr)
            qWarning("Could not open config file");
        else
            QMessageBox::warning(parent, "Configuration failure", "Could not open config file");
        loadFile.close();
        return {};
    }

    QByteArray tmp = loadFile.readAll();
    jsonConfig = QJsonDocument::fromJson(tmp);

    loadFile.close();
    return jsonConfig;
}

QString
Config::getPort()
{
    QString port = "";

    if (jsonObject.contains("Port"))
        port = jsonObject["Port"].toString();

    return port;
}

unsigned int
Config::getBaudrate()
{
    unsigned int rate = 0;
    if (jsonObject.contains("Baudrate"))
        rate = jsonObject["Baudrate"].toInt();

    return rate;
}

void
Config::setPort(const QString &port)
{
    QFile loadFile(configFile);

    if (!loadFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open save file");
        loadFile.close();
        return;
    }

    jsonObject.insert("Port", port);
    auto jsonDoc = QJsonDocument(jsonObject);

    loadFile.write(jsonDoc.toJson());
    loadFile.close();
}

void
Config::setBaudrate(const unsigned int &baudrate)
{
    QFile loadFile(configFile);

    if (!loadFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open save file");
        loadFile.close();
        return;
    }

    jsonObject.insert("Baudrate", QJsonValue::fromVariant(QVariant(baudrate)));
    auto jsonDoc = QJsonDocument(jsonObject);

    loadFile.write(jsonDoc.toJson());
    loadFile.close();
}
