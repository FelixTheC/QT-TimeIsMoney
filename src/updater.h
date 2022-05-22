#ifndef UPDATER_H
#define UPDATER_H

#include <QDialog>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>

#include "qapplication.h"

class Updater : public QDialog
{
    Q_OBJECT


private slots:
    void _runUpdate();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

public:
    Updater();

private:
    void init();
    bool update_available = false;
    void checkForUpdate() noexcept;
    void runUpdate() noexcept;
    void display() noexcept;
    QMessageBox *msgBox;
    QLabel *update_label;
    QProcess process;
    QString path = "";
    QString programm = "";
    void displayError() noexcept;

};

#endif // UPDATER_H
