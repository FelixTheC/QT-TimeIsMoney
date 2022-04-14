#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

#include <QtTest/QtTest>
#include <QObject>


class TestConfig : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        qDebug("Called before everything else.");
    }
};


QTEST_MAIN(TestConfig)

#endif // TEST_CONFIG_H
