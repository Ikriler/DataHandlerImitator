#include "server.h"
#include "ui_mainwindow.h"
#include <QFile>

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
    //connect(ui->listWidget, &QListWidget::currentRowChanged, this, &Server::reconnectDevices);
}

void Server::reconnectDevices() {
    QList<QListWidgetItem *> items = ui->listWidget->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
    QList<QString> texts;
    foreach(QListWidgetItem *item, items)
      texts.append(item->text());

    foreach(QTcpSocket *curSock, sockets) {
        QString curIP = curSock->peerAddress().toString().replace("::ffff:", "");
        qDebug() << curIP;
        qDebug() << texts.size();
        if(!texts.contains(curIP)) {
            qDebug() << curIP + " disconnected";
            curSock->disconnectFromHost();
        }
    }
}

void Server::incomingConnection(qintptr socketDescriptor) {
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Server::disconnected);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    QString str = socket->peerAddress().toString().replace("::ffff:", "");

    QList<QListWidgetItem *> items = ui->listWidget->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
    QList<QString> texts;
    foreach(QListWidgetItem *item, items)
      texts.append(item->text());

    if(!texts.contains(str)) {
        writeMessage("Попытка подключения неизвестного устройства: " + str);
        socket->disconnectFromHost();
        return;
    }

    sockets.push_back(socket);

    writeMessage(str + " подключён");
    updateListView();

}

void Server::disconnected() {
    QString str;
    QTcpSocket *disconnectedSocket = qobject_cast<QTcpSocket *>(sender());
    sockets.removeAll(disconnectedSocket);
    str = disconnectedSocket->peerAddress().toString().replace("::ffff:", "");
    writeMessage(str + " отключён");
    updateListView();
}



void Server::writeMessage(QString str) {
    QString pathToLog = ui->lineEdit_2->text();

    QDateTime dateTime = QDateTime::currentDateTime();
    QString message = dateTime.toString(Qt::ISODate).replace("T", " ") + " " + str + "\n";
    ui->textBrowser->append(message);

    if(pathToLog != "") {
        QFile logFile(pathToLog + "/" + dateTime.toString(Qt::ISODate).split("T")[0] + ".txt");
        logFile.open(QFile::WriteOnly | QIODevice::Append);
        logFile.write(message.toUtf8());
        logFile.write("\n");
        logFile.close();
    }
}

void Server::slotReadyRead() {
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Version::Qt_6_4);
    if(in.status() == QDataStream::Ok) {
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
            QString socketAddress;
            socketAddress = socket->peerAddress().toString().replace("::ffff:", "");
            writeMessage("Сообщение от " + socketAddress + ": " +  str);



            QString outputMessage = str + str;
            SendToClient(outputMessage);
            writeMessage("Ответ сервера для " + socketAddress + ": " +  outputMessage);
            break;
        }
    }
    else {
        qDebug() << "DataStream error";
    }
}

void Server::updateListView() {
    ui->textBrowser_2->clear();
    for(int i = 0; i < sockets.count(); i++) {
        QString str = socket->peerAddress().toString().replace("::ffff:", "");
        ui->textBrowser_2->append(str + "\n");
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
