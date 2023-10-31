#ifndef UPDATER_HPP
#define UPDATER_HPP

#include <QApplication>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>


class Updater : public QDialog
{
    Q_OBJECT


private slots:
    void _runUpdate();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

public:
    Updater();

private:
    // void init();
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

#endif // UPDATER_HPP
