// File: mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
    , server(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("TCP Server");

    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &MainWindow::newConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
    qDeleteAll(clients);
}

void MainWindow::on_startButton_clicked()
{
    if(server->isListening()){
        server->close();
        ui->startButton->setText("Start Server");
        logMessage("Server stopped");
    }
    else{
        int port = ui->portSpin->value();
        if(server->listen(QHostAddress::Any, port)){
            ui->startButton->setText("Stop Server");
            logMessage(QString("Server started on port %1").arg(port));
        }
        else{
            logMessage("Failed to start server: " + server->errorString());
        }
    }
}

void MainWindow::on_sendButton_clicked()
{
    QString message = ui->messageEdit->text();
    if(message.isEmpty()) return;

    foreach(QTcpSocket *client, clients){
        client->write(message.toUtf8());
    }
    logMessage("Sent to all clients: " + message);
    ui->messageEdit->clear();
}

void MainWindow::newConnection()
{
    QTcpSocket *client = server->nextPendingConnection();
    clients.append(client);
    connect(client, &QTcpSocket::disconnected, this, &MainWindow::clientDisconnected);
    connect(client, &QTcpSocket::readyRead, this, &MainWindow::readyRead);

    logMessage(QString("New client connected: %1:%2")
               .arg(client->peerAddress().toString())
               .arg(client->peerPort()));
    updateClientList();
}

void MainWindow::clientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if(!client) return;

    logMessage(QString("Client disconnected: %1:%2")
              .arg(client->peerAddress().toString())
              .arg(client->peerPort()));
    clients.removeOne(client);
    client->deleteLater();
    updateClientList();
}

void MainWindow::readyRead()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if(!client) return;

    QString message = QString::fromUtf8(client->readAll());
    logMessage(QString("Received from %1:%2 - %3")
               .arg(client->peerAddress().toString())
               .arg(client->peerPort())
               .arg(message));
}

void MainWindow::updateClientList()
{
    ui->clientList->clear();
    foreach(QTcpSocket *client, clients){
        ui->clientList->addItem(QString("%1:%2")
                               .arg(client->peerAddress().toString())
                               .arg(client->peerPort()));
    }
}

void MainWindow::logMessage(const QString &message)
{
    ui->logEdit->appendPlainText(QDateTime::currentDateTime().toString("[hh:mm:ss] ") + message);
}
