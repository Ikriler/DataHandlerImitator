#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QByteArray data;
    void SendToServer(QString str);
    qint16 nextBlockSize;
    void writeMessage(QString str);
    void disconnectMessage();

public slots:
    void slotReadyRead();
    void disconnect();
    void hostFoundMessage();
};
#endif // MAINWINDOW_H
