#ifndef SENDER_H
#define SENDER_H

#include <QtWidgets>
#include <QtNetwork>
#include <QtCore>

class Sender : public QDialog
{
    Q_OBJECT

public:
    explicit Sender(QWidget *parent = nullptr);

private slots:
    void ttlChanged(int newTtl);
    void startSending();
    void sendDatagram();

private:
    QLabel *statusLabel = nullptr;
    QPushButton *startButton = nullptr;
    QUdpSocket udpSocket4;
    QUdpSocket udpSocket6;
    QTimer timer;
    QHostAddress groupAddress4;
    QHostAddress groupAddress6;
    int messageNo = 1;
};

#endif
