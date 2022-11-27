#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QListWidgetItem>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initWhiteListIP();
    initPathToLog();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initWhiteListIP() {
    QFile file("whiteIP.txt");
    if(file.exists()) {
        file.open(QFile::ReadOnly);
        QByteArray data = file.readAll();
        QList<QString> ips = QString::fromUtf8(data).split(":");
        for(int i = 0; i < ips.count() - 1; i++) {
            QListWidgetItem *itm = new QListWidgetItem;
            itm->setText(ips.at(i));
            ui->listWidget->addItem(itm);
        }
    }
}

void MainWindow::initPathToLog() {
    QFile file("pathToLog.txt");
    if(file.exists()) {
        file.open(QFile::ReadOnly);
        QByteArray data = file.readAll();
        QString pathToLog = QString::fromUtf8(data);
        ui->lineEdit_2->setText(pathToLog);
    }
}


void MainWindow::writeMessage(QString str) {
    QDateTime dateTime = QDateTime::currentDateTime();
    ui->textBrowser->append(dateTime.toString(Qt::ISODate).replace("T", " ") + " " + str + "\n");
}

void MainWindow::on_pushButton_4_clicked()
{
    QString newIP =  ui->lineEdit->text();
    QRegularExpression reg("^[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}$");

    QList<QListWidgetItem *> items = ui->listWidget->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
    QList<QString> texts;
    foreach(QListWidgetItem *item, items)
      texts.append(item->text());

    if(reg.match(newIP).hasMatch() && !texts.contains(newIP)){
        QListWidgetItem *itm = new QListWidgetItem;
        itm->setText(newIP);
        ui->listWidget->addItem(itm);
        writeMessage("IP добавлен в белый список: " + newIP);
    }
    else {
        writeMessage("Ошибка добавления IP: " + newIP);
    }
    updateWhiteListIP();

}


void MainWindow::on_pushButton_5_clicked()
{
    QList<QListWidgetItem*> items = ui->listWidget->selectedItems();
    foreach(QListWidgetItem * item, items)
    {
        writeMessage("IP удалён из белого списка: " + item->text());
        delete ui->listWidget->takeItem(ui->listWidget->row(item));
    }
    updateWhiteListIP();
    if(server != nullptr) {
         server->reconnectDevices();
    }
}


void MainWindow::updateWhiteListIP() {
    QFile file("whiteIP.txt");

    QList<QListWidgetItem *> items = ui->listWidget->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
    QList<QString> texts;
    foreach(QListWidgetItem *item, items)
      texts.append(item->text());


    QByteArray ipList;

    foreach(const QString &str, texts) {
        ipList.append(str.toUtf8() + ":");
    }

    file.open(QFile::WriteOnly);

    file.flush();

    file.write(ipList);

    file.close();
}


void MainWindow::on_pushButton_clicked()
{
    QString directoryForLog = QFileDialog::getExistingDirectory();

    QFile file("pathToLog.txt");

    file.flush();

    QByteArray pathToLog;

    pathToLog.append(directoryForLog.toUtf8());

    file.open(QFile::WriteOnly);

    file.write(pathToLog);

    file.close();

    ui->lineEdit_2->setText(directoryForLog);
}

