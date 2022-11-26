#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Ui::MainWindow *ui;
    void writeMessage(QString str);
    void updateWhiteListIP();
    void initWhiteListIP();
    void initPathToLog();

public slots:

    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
private slots:
    void on_pushButton_clicked();
};
#endif // MAINWINDOW_H
