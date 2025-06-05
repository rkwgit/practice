// File: mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QDateTime>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startButton_clicked();
    void on_sendButton_clicked();
    void newConnection();
    void clientDisconnected();
    void readyRead();

private:
    Ui::MainWindow *ui;
    QTcpServer *server;
    QList<QTcpSocket*> clients;

    void updateClientList();
    void logMessage(const QString &message);
};

#endif // MAINWINDOW_H
