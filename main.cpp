#include "chatdialog.h"

#include <QApplication>

#include "receiver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChatDialog dialog;
    Receiver receiver;

    dialog.setFixedSize(dialog.sizeHint()); //Permet de verouiller la taille de la fenêtre à la taille recommandée

    QObject::connect(&receiver, SIGNAL(newMessage(QString)), &dialog, SLOT(appendMessage(QString)));
    QObject::connect(&receiver, SIGNAL(newImage(QImage)), &dialog, SLOT(LoadImageFromOthr(QImage)));
    QObject::connect(&dialog, SIGNAL(newTr(bool)), &receiver, SLOT(chgtTr(bool)));

    //receiver.show();
    dialog.show();
    return a.exec();
}
