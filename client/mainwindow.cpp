#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit_2->setText("127.0.0.1");
    ui->lineEdit_3->setText("2323");

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    nextBlockSize = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{

    QString ipAddress = ui->lineEdit_2->text();
    QString port = ui->lineEdit_3->text();

    socket->connectToHost(ipAddress, port.toInt());
//    if(socket->state() == QTcpSocket::ConnectedState) {
//        ui->textBrowser->append("connected success " + ipAddress + " " + port);
//    }
//    else {
//        ui->textBrowser->append("connected error " + ipAddress + " " + port);
//    }
}

void MainWindow::SendToServer(QString str)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Version::Qt_6_4);
    out << qint16(0) << str;
    out.device()->seek(0);
    out << quint16(data.size() - sizeof(quint16));
    socket->write(data);
    ui->lineEdit->clear();
}

void MainWindow::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Version::Qt_6_4);
    if(in.status() == QDataStream::Ok) {
        for(;;) {
            if(nextBlockSize == 0) {
                if(socket->bytesAvailable() < 2){
                    break;
                }
                in >> nextBlockSize;
            }
            if(socket->bytesAvailable() < nextBlockSize) {
                break;
            }
            QString str;
            in >> str;
            nextBlockSize = 0;
            ui->textBrowser->append(str);
            break;
        }
    }
    else {
        ui->textBrowser->append("read error");
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    SendToServer(ui->lineEdit->text());
}

