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
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::writeMessage(QString str) {
    QDateTime dateTime = QDateTime::currentDateTime();
    ui->textBrowser->append(dateTime.toString(Qt::ISODate).replace("T", " ") + " " + str + "\n");
}

void MainWindow::on_pushButton_4_clicked()
{
    QString newIP =  ui->lineEdit->text();
    QRegularExpression reg("^[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}$");
    if(reg.match(newIP).hasMatch()){
        QListWidgetItem *itm = new QListWidgetItem;
        itm->setText(newIP);
        ui->listWidget->addItem(itm);
        writeMessage("IP добавлен в белый список: " + newIP);
    }
    else {
        writeMessage("Ошибка добавления IP: " + newIP);
    }

}


void MainWindow::on_pushButton_5_clicked()
{
    QList<QListWidgetItem*> items = ui->listWidget->selectedItems();
    foreach(QListWidgetItem * item, items)
    {
        writeMessage("IP удалён из белого списка: " + item->text());
        delete ui->listWidget->takeItem(ui->listWidget->row(item));
    }
}

