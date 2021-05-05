#ifndef RECEIVER_H
#define RECEIVER_H


#include <QDialog>
#include <QHostAddress>
#include <QUdpSocket>
#include <QtCore>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class Receiver : public QDialog
{
    Q_OBJECT

public:
    explicit Receiver(QWidget *parent = nullptr);

private slots:
    void chgtTr(const bool &chgt);
    void processPendingDatagrams();

signals :
    void newMessage(const QString &message);
    void newImage(const QImage &img);

private:
    bool IPV6tr;
    QLabel *statusLabel = nullptr;
    QUdpSocket udpSocket4;
    QUdpSocket udpSocket6;
    QHostAddress groupAddress4;
    QHostAddress groupAddress6;
};

#endif
