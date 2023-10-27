#ifndef CONFIG_H
#define CONFIG_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QStringLiteral>


class Config
{
public:
    Config();
    explicit Config(QWidget *parent);
    ~Config() = default;

    QJsonDocument loadConfigFile(QWidget *parent);
    QString getPort();
    unsigned int getBaudrate();

    void setPort(const QString &port);
    void setBaudrate(const unsigned int &baudrate);

private:
    const QString configFile = QStringLiteral("./serialPortConfig.json");
    QJsonObject jsonObject = QJsonObject();

    void initConfigFileIfNotExists();
};

#endif // CONFIG_H
