#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include "ui_chatdialog.h"

#include <QtNetwork>
#include <QtCore>

class ChatDialog : public QDialog, private Ui::ChatDialog
{
    Q_OBJECT

public:
    ChatDialog(QWidget *parent = 0);

public slots:
    void appendMessage(const QString &message);
    void LoadImageFromOthr(const QImage &img);

private slots:
    void sendImage(const QImage &img);
    void DownloadImage();
    void LoadImg();
    void chgtPseudo();
    void chckSt();
    void returnPressed();


signals:
    void newTr(const bool &IPV6tr);

private:
    bool IPV6tr;
    QUdpSocket udpSocket4;
    QUdpSocket udpSocket6;
    QHostAddress groupAddress4;
    QHostAddress groupAddress6;
    QString Pseudo;
    QTextTableFormat tableFormat;
};

#endif
