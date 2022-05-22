#include "updater.h"


Updater::Updater()
{
    connect(&process,
            &QProcess::finished,
            this,
            &Updater::processFinished);

    if (QSysInfo::productType().toLower().contains("windows"))
        programm = "maintenancetool.exe";
    else
        programm = "maintenancetool";

    path = QDir(QApplication::applicationDirPath()).absoluteFilePath(programm);

    QFile file = QFile(path);
    if (!file.exists())
        displayError();

    checkForUpdate();
}

void
Updater::checkForUpdate() noexcept
{
    if (process.state() == QProcess::NotRunning)
    {
        QStringList args;

        args.append("--checkupdates");
        process.start(path, args);
    }
}


void
Updater::runUpdate() noexcept
{
    QStringList args;
    args.append("--updater");
    process.startDetached(programm, args);
}

void
Updater::display() noexcept
{
    QString msg;
    if (update_available)
        msg = "An UPDATE is available!!";
    else
        msg = "Nothing found.";

    msgBox = new QMessageBox(QMessageBox::Information,
                             "Updater",
                             msg,
                             QMessageBox::Ok | QMessageBox::Cancel);

    connect(msgBox, &QMessageBox::accepted, this, &Updater::_runUpdate);
    msgBox->show();
}

void
Updater::_runUpdate()
{
    runUpdate();
}

void
Updater::processFinished(int exit_code, QProcess::ExitStatus exit_status)
{
    qDebug() << exit_code << exit_status;

    auto std_out = process.readAllStandardOutput();
    QString std_out_str = QString::fromLocal8Bit(std_out);
    update_available = !std_out_str.contains("Warning");

    display();
}

void
Updater::displayError() noexcept
{
    QMessageBox::warning(this,
                         "Updater",
                         "Version does not support automatical update checks");
}
