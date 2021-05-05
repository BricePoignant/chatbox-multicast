
#include <QtWidgets>

#include <QString>

#include "chatdialog.h"

#include "receiver.h"

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent),
      groupAddress4(QStringLiteral("239.255.43.21")),
      groupAddress6(QStringLiteral("ff12::2115"))
{
    setupUi(this);

    IPV6tr =false;
    int TTL =10; // Détermine le TTL du protocole UDP
    Pseudo = lineEdit_2->text();

    //Paramètres de l'ui
    tableFormat.setBorder(0);
    lineEdit->setFocusPolicy(Qt::StrongFocus);
    textEdit->setFocusPolicy(Qt::NoFocus);
    textEdit->setReadOnly(true);
    label_3->setScaledContents(true);

    //Setup connection

    // force binding to their respective families
    udpSocket4.bind(QHostAddress(QHostAddress::AnyIPv4), 0);
    udpSocket6.bind(QHostAddress(QHostAddress::AnyIPv6), udpSocket4.localPort());


    udpSocket4.setSocketOption(QAbstractSocket::MulticastTtlOption, TTL);

    //Mise en place des connection SIGNAL/PORTS

    connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
    connect(lineEdit_2, SIGNAL(textChanged(const QString &)), this, SLOT(chgtPseudo())); //Permet de connecter le changement de texte au changement de pseudo
    connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(chckSt()));
    connect(pushButton, SIGNAL(pressed()), this, SLOT(DownloadImage()));

}



void ChatDialog::DownloadImage()
{
    std::string strAvatarUrl = lineEdit_3->text().toStdString();

   QUrl url(QString().fromStdString(strAvatarUrl));
   QNetworkAccessManager manager;
   QEventLoop loop;

   QLabel &lable=*label_3;

   lable.clear();

   QNetworkReply *reply = manager.get(QNetworkRequest(url));

   QObject::connect(reply, &QNetworkReply::finished, &loop, [&reply, &loop, &lable]() {
       if (reply->error() == QNetworkReply::NoError)
       {
           QByteArray jpegData = reply->readAll();
           QPixmap pixmap;
           pixmap.loadFromData(jpegData);
           if (!pixmap.isNull())
           {
               lable.setPixmap(pixmap);
           }
       }
       loop.quit();
     });

     loop.exec();

     LoadImg();
}

void ChatDialog::LoadImg()
{
    const QPixmap* pixmap = label_3->pixmap();

    if (pixmap != nullptr)
    {
        QImage image( pixmap->toImage() );

        QPixmap monPixmap = monPixmap.fromImage(image);

        sendImage(image);
    }
    else {
        appendMessage("Erreur mauvais url");
    }
}

void ChatDialog::chckSt()
{
    if (checkBox->checkState()==Qt::Checked) {
        IPV6tr =true;
    }
    else {
        IPV6tr =false;
    }
    emit newTr(IPV6tr);
}

void ChatDialog::appendMessage(const QString &message)
{
    if (message.isEmpty())
        return;

    QTextCursor cursor(textEdit->textCursor());
    cursor.movePosition(QTextCursor::End);
    QTextTable *table = cursor.insertTable(1, 2, tableFormat);
    //table->cellAt(0, 0).firstCursorPosition().insertText('<' + from + "> ");
    table->cellAt(0, 1).firstCursorPosition().insertText(message);
    QScrollBar *bar = textEdit->verticalScrollBar();
    bar->setValue(bar->maximum());
}


void ChatDialog::sendImage(const QImage &img)
{
    if (!img.isNull()) {
        QBuffer buffer;
        QImageWriter writer(&buffer, "JPG");
        //qint64 value = Q_INT64_C(932838457459459);
        writer.write(img);
        QByteArray data;
        data.append(buffer.data());

        std::vector<QByteArray> parts;

        int size = 65000; //65000
        int i = 0;

        while (data.size() > (i+1)*size) {
            parts.emplace_back(data.mid(i * size, size));
            i++;
        }

        QByteArray datat;
        for (int j = 0; j < i; j++) {
            datat.append(parts.at(j));
        }


        if (data.size()!=datat.size()){
            parts.emplace_back(data.mid(i * size, data.size()-datat.size()));
            i++;
        }
        /*
        datat.clear();
        for (int j = 0; j < i; j++) {
            datat.append(parts.at(j));
        }


        appendMessage(QString::number(data.size()));
        appendMessage(QString::number(datat.size()));


        if (data==datat) {
            appendMessage("C'est bon frere");
        }

        */

        QString text = "Image reçue de " + Pseudo;
        QByteArray datagram = text.toUtf8();

        if (IPV6tr == false) {
            if (i==0) {
                udpSocket4.writeDatagram(data, groupAddress4, 45454);
            }
            else {
                for (int j = 0; j < i; j++) {
                    udpSocket4.writeDatagram(parts.at(j), groupAddress4, 45454);
                    //QThread::msleep(5);
                }
            }
            QThread::msleep(100);
            udpSocket4.writeDatagram(datagram, groupAddress4, 45454);
        }
        else {
            if (udpSocket6.state() == QAbstractSocket::BoundState) {
                if (i==0) {
                    udpSocket6.writeDatagram(data, groupAddress6, 45454);
                }
                else {
                    for (int j = 0; j < i; j++) {
                        udpSocket6.writeDatagram(parts.at(j), groupAddress6, 45454);
                    }
                }
                QThread::msleep(100);
                udpSocket6.writeDatagram(datagram, groupAddress6, 45454);
            }
        }

        QImage image;
        image.loadFromData(data, "PNG");

        label_3->clear();
        QPixmap monPixmap = monPixmap.fromImage(image);
        label_3->setPixmap(monPixmap);
    }
}

void ChatDialog::LoadImageFromOthr(const QImage &img)
{
    if (!img.isNull()) {
        QPixmap monPixmap = monPixmap.fromImage(img);
        label_3->clear();
        label_3->setPixmap(monPixmap);
    }
}



void ChatDialog::returnPressed()
{
    QString text = lineEdit->text();
    if (text.isEmpty())
        return;

    if (text.startsWith(QChar('/'))) {
        QColor color = textEdit->textColor();
        textEdit->setTextColor(Qt::red);
        textEdit->append(tr("! Unknown command: %1")
                         .arg(text.left(text.indexOf(' '))));
        textEdit->setTextColor(color);
    } else {
        QString temp='<' + Pseudo + "> ";
        temp.append(text);
        QByteArray datagram = temp.toUtf8();

        if (IPV6tr == false) {
            udpSocket4.writeDatagram(datagram, groupAddress4, 45454);
        }
        else {
            if (udpSocket6.state() == QAbstractSocket::BoundState)
                udpSocket6.writeDatagram(datagram, groupAddress6, 45454);
        }
    }
    lineEdit->clear();
}

void ChatDialog::chgtPseudo()
{
    Pseudo = lineEdit_2->text();
}
