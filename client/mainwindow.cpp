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
    connect(socket, &QTcpSocket::connected, this, &MainWindow::hostFoundMessage);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::disconnectMessage);
    nextBlockSize = 0;
}

void MainWindow::writeMessage(QString str) {
    QDateTime dateTime = QDateTime::currentDateTime();
    ui->textBrowser->append(dateTime.toString(Qt::ISODate).replace("T", " ") + " " + str + "\n");
}

void MainWindow::hostFoundMessage() {

    ui->pushButton->setEnabled(false);
    ui->pushButton_3->setEnabled(true);

    ui->lineEdit_2->setEnabled(false);
    ui->lineEdit_3->setEnabled(false);

    writeMessage("Успешно подключено!");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString ipAddress = ui->lineEdit_2->text();
    QString port = ui->lineEdit_3->text();


    writeMessage("Подключение к " + ipAddress + ":" + port);

    socket->connectToHost(ipAddress, port.toInt());

}

void MainWindow::SendToServer(QString str)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out << quint16(0);


    foreach(QChar c, str) {
        out << c;
    }

    out.device()->seek(0);
    out << quint16(data.size() - sizeof(quint16));
    socket->write(data);
    ui->lineEdit->clear();
}

void MainWindow::disconnectMessage() {
    QString ipAddress = ui->lineEdit_2->text();
    QString port = ui->lineEdit_3->text();

    ui->lineEdit_2->setEnabled(true);
    ui->lineEdit_3->setEnabled(true);

    writeMessage("Отключение от " + ipAddress + ":" + port);

    ui->pushButton->setEnabled(true);
    ui->pushButton_3->setEnabled(false);
}

void MainWindow::disconnect() {
    socket->disconnectFromHost();
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
            writeMessage("Ответ сервера: " + str);
            break;
        }
    }
    else {
        writeMessage("Ошибка чтения");
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    QString sendMessage = ui->lineEdit->text();
    writeMessage("Отправленное сообщение: " + sendMessage);
    SendToServer(sendMessage);
}

void MainWindow::on_pushButton_3_clicked()
{
    MainWindow::disconnect();
}

