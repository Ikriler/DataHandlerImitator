#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server(Ui::MainWindow *ui);
    QTcpSocket *socket;
    Ui::MainWindow *ui;

private:
    QVector<QTcpSocket*> sockets;
    QByteArray data;
    void SendToClient(QString str);
    qint16 nextBlockSize;
    void writeMessage(QString str);

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
};

#endif // SERVER_H
