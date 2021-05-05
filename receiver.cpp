#include <QtWidgets>
#include <QtNetwork>

#include "receiver.h"

Receiver::Receiver(QWidget *parent)
    : QDialog(parent),
      groupAddress4(QStringLiteral("239.255.43.21")),
      groupAddress6(QStringLiteral("ff12::2115"))
{

    statusLabel = new QLabel(tr("Listening for multicast messages on both IPv4 and IPv6"));
    auto quitButton = new QPushButton(tr("&Quit"));

    auto buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(quitButton);
    buttonLayout->addStretch(1);

    auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(statusLabel);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    IPV6tr=false;

    setWindowTitle(tr("Multicast Receiver"));

    udpSocket4.bind(QHostAddress::AnyIPv4, 45454, QUdpSocket::ShareAddress);
    udpSocket4.joinMulticastGroup(groupAddress4);

    if (!udpSocket6.bind(QHostAddress::AnyIPv6, 45454, QUdpSocket::ShareAddress) ||
            !udpSocket6.joinMulticastGroup(groupAddress6))
        statusLabel->setText(tr("Listening for multicast messages on IPv4 only"));

    connect(&udpSocket4, SIGNAL(readyRead()),
            this, SLOT(processPendingDatagrams()));
    connect(&udpSocket6, &QUdpSocket::readyRead, this, &Receiver::processPendingDatagrams);
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));


}

void Receiver::chgtTr(const bool &chgt) {
    IPV6tr=chgt;
}

void Receiver::processPendingDatagrams()
{
    QByteArray datagram;

    QByteArray data;

    QImage image;


    if (IPV6tr==false) {
    // using QUdpSocket::readDatagram (API since Qt 4)
        while (udpSocket4.hasPendingDatagrams()) {

           datagram.resize(int(udpSocket4.pendingDatagramSize()));
           udpSocket4.readDatagram(datagram.data(), datagram.size());
           statusLabel->setText(tr("Received IPv4 datagram: \"%1\"")
                                .arg(datagram.constData()));

           data.append(datagram);


           if (!udpSocket4.hasPendingDatagrams()) {

           image.loadFromData(data, "JPG");


           if (!image.isNull()) {
               emit newImage(image);
           }

           else {
               emit newMessage(QString(datagram));
           }


           }

        }
    }

    else {
    // using QUdpSocket::receiveDatagram (API since Qt 5.8)
        while (udpSocket6.hasPendingDatagrams()) {
            QNetworkDatagram dgram = udpSocket6.receiveDatagram();
            statusLabel->setText(statusLabel->text() +
                                 tr("\nReceived IPv6 datagram from [%2]:%3: \"%1\"")
                                 .arg(dgram.data().constData(), dgram.senderAddress().toString())
                               .arg(dgram.senderPort()));

            data.append(dgram.data());


            if (!udpSocket6.hasPendingDatagrams()) {

            image.loadFromData(data, "JPG");


            if (!image.isNull()) {
                emit newImage(image);
            }

            else {
                emit newMessage(QString(dgram.data()));
            }


            }
        }
    }
}
