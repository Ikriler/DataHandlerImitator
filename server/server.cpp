#include "server.h"
#include "ui_mainwindow.h"

Server::Server(Ui::MainWindow *ui)
{
    if(this->listen(QHostAddress::Any, 2323)) {
        qDebug() << "start";
    }
    else {
        qDebug() << "error";
    }
    nextBlockSize = 0;
    this->ui = ui;
}

void Server::incomingConnection(qintptr socketDescriptor) {
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    sockets.push_back(socket);

    QString str;

    str = socket->peerName();
    ui->textBrowser->append(str + " connected");
}

void Server::writeMessage(QString str) {
    QDateTime dateTime = QDateTime::currentDateTime();
    ui->textBrowser->append(dateTime.toString(Qt::ISODate).replace("T", " ") + " " + str + "\n");
}

void Server::slotReadyRead() {
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Version::Qt_6_4);
    if(in.status() == QDataStream::Ok) {
//        qDebug() << "read...";
//        QString str;
//        in >> str;
//        qDebug() << str;
//        SendToClient(str + str);
        for(;;) {
            if(nextBlockSize == 0) {
                qDebug() << "nextBlockSize = 0";
                if(socket->bytesAvailable() < 2){
                    qDebug() << "data < 2, break";
                    break;
                }
                in >> nextBlockSize;
                qDebug() << "nextBlockSize = " << nextBlockSize;
            }
            if(socket->bytesAvailable() < nextBlockSize) {
                qDebug() << "data not full, break";
                break;
            }
            QString str;
            in >> str;
            nextBlockSize = 0;
            ui->textBrowser->append(str);
            qDebug() << str;
            SendToClient(str + str);
            break;
        }
    }
    else {
        qDebug() << "DataStream error";
    }
}

void Server::SendToClient(QString str) {
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Version::Qt_6_4);
    out << qint16(0) << str;
    out.device()->seek(0);
    out << quint16(data.size() - sizeof(quint16));
    out << str;
    socket->write(data);
}
