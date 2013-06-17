/***********************************************************************************
 *  Copyright (C) 2013 Michał Bień.
 *
 *     This file is part of Formoza.
 *
 *  Formoza is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Formoza is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Formoza; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 ***********************************************************************************/

#include "connection.hpp"
#include "mainwindow.hpp"

Connection::Connection(MainWindow *parent):
    parent(parent),
    socket(new QTcpSocket(this)),
    cryptPass(new QCryptographicHash(QCryptographicHash::Sha512))
{
    connect(socket, SIGNAL(connected()), SLOT(setConnected()));
    connect(socket, SIGNAL(disconnected()), SLOT(setDisconnected()));
    connect(socket, SIGNAL(readyRead()),	SLOT(updateMessage()));
}

Connection::~Connection()
{

}

void Connection::zaloguj(QString hostname)
{
    QHostInfo::lookupHost(hostname, this, SLOT(connectHost(QHostInfo)));
}

void Connection::connectHost(QHostInfo hostinfo)
{
    //socket->connectToHost(hostinfo.addresses().first(), 2000);
    socket->connectToHost(QHostAddress("127.0.0.1"), 2000);
}

void Connection::setConnected()
{
    cryptPass->addData(parent->getLoginDane().second.toUtf8().data());
    QByteArray pass = cryptPass->result();
    pass = pass.toHex();
    QString passSTR = QString(pass.constData());

    delete cryptPass;
    cryptPass = new QCryptographicHash(QCryptographicHash::Sha512);

    if(sendCommand("log", parent->getLoginDane().first)!=QString("ok"))
    {
        setDisconnected();
        return;
    }

    if(sendCommand("pas", passSTR)!=QString("ok"))
    {
        setDisconnected();
        return;
    }

    if(sendCommand("lin", "null")!=QString("ok"))
    {
        setDisconnected();
        return;
    }

    parent->connected();

    parent->setDane();

    parent->setGrupy();

    parent->showPowiadomienia();
}

void Connection::setDisconnected()
{
    socket->disconnectFromHost();
    parent->disconnected();
}

QString Connection::sendCommand(QString cmd, QString arg)
{
    disconnect(socket, SIGNAL(readyRead()), this,	SLOT(updateMessage()));
    QString returnvalue;
    socket->write(cmd.toUtf8().data());
    socket->waitForBytesWritten();
    socket->waitForReadyRead();
    QString received(socket->readAll());
    if (received==QString("ack"))
    {
        socket->write(arg.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();
        received = QString(socket->readAll());
        returnvalue = received;
        QString end("end");
        socket->write(end.toUtf8().data());
        socket->waitForBytesWritten();

        connect(socket, SIGNAL(readyRead()),	SLOT(updateMessage()));

        return returnvalue;
    }

    else
        return QString("NULL");
}

QVector<QString> Connection::getDane()
{
    disconnect(socket, SIGNAL(readyRead()), this,	SLOT(updateMessage()));
    QVector<QString> dane;
    QString ackResponse("ack");
    QString okResponse("ok");
    socket->write("get");
    socket->waitForBytesWritten();
    socket->waitForReadyRead();
    QString received(socket->readAll());
    if (received==QString("ack"))
    {
        socket->write("dan");
        socket->waitForBytesWritten();
        socket->waitForReadyRead();
        dane.push_back(QString(socket->readAll()));
        socket->write(ackResponse.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();
        dane.push_back(QString(socket->readAll()));
        socket->write(ackResponse.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();
        dane.push_back(QString(socket->readAll()));
        socket->write(ackResponse.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();
        dane.push_back(QString(socket->readAll()));
        socket->write(ackResponse.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();
        socket->readAll(); //OK

        QString end("end");
        socket->write(end.toUtf8().data());
        socket->waitForBytesWritten();

        connect(socket, SIGNAL(readyRead()), SLOT(updateMessage()));
    }

    return dane;
}

QVector<QPair<QString, QString> > Connection::getGrupy()
{
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(updateMessage()));
    QVector<QPair<QString, QString> > vector; //First REAL second REP
    QString ackResponse("ack");
    socket->write("get");
    socket->waitForBytesWritten();
    socket->waitForReadyRead();
    QString received(socket->readAll());
    if (received==QString("ack"))
    {
        socket->write("gru");
        socket->waitForBytesWritten();
        socket->waitForReadyRead();
        int i = socket->readAll().toInt();
        socket->write(ackResponse.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();

        for(;i>0;i--)
        {
            QPair<QString, QString> grupa;
            grupa.first = QString(socket->readAll());
            socket->write(ackResponse.toUtf8().data());
            socket->waitForBytesWritten();
            socket->waitForReadyRead();
            grupa.second = QString(socket->readAll());
            socket->write(ackResponse.toUtf8().data());
            socket->waitForBytesWritten();
            socket->waitForReadyRead();
            vector.push_back(grupa);
        }

        socket->readAll(); //ACK

        QString end("end");
        socket->write(end.toUtf8().data());
        socket->waitForBytesWritten();

        connect(socket, SIGNAL(readyRead()),	SLOT(updateMessage()));
    }
    return vector;
}

void Connection::updateMessage()
{
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(updateMessage()));
    QString ackResponse("ack");
    QString okResponse ("ok");
    QString messg(socket->readAll());

    if(messg==QString("upd"))
    {
        socket->write(ackResponse.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();
        
        QString arg=QString(socket->readAll());
        
        socket->write(ackResponse.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();

        QString silosc(socket->readAll());

        int ilosc = silosc.toInt();
        socket->write(ackResponse.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();

        QVector<QString> vector;

        for(int i=0; i<ilosc; i++)
        {
            vector.push_back(QString(socket->readAll()));
            socket->write(ackResponse.toUtf8().data());
            socket->waitForBytesWritten();
            socket->waitForReadyRead();
        }

        socket->readAll(); //END

        socket->write(okResponse.toUtf8().data());
        socket->waitForBytesWritten();

        if(arg==QString("wia"))
        {
            parent->wiadomosciDialogUpdate(vector.at(0), vector.at(1));
        }
    }
    connect(socket, SIGNAL(readyRead()), this, SLOT(updateMessage()));
}

QVector<Powiadomienie> Connection::getFirstUpdate()
{
    QVector<Powiadomienie> vector;
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(updateMessage()));
    QString ackResponse("ack");
    socket->write("get");
    socket->waitForBytesWritten();
    socket->waitForReadyRead();
    QString received(socket->readAll());
    if (received==ackResponse)
    {
        socket->write("fup");
        socket->waitForBytesWritten();
        socket->waitForReadyRead();

        int liczba = socket->readAll().toInt();

        socket->write(ackResponse.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();

        //USERS
        for(int i=0; i<liczba; i++)
        {
            Powiadomienie pow;
            pow.type = QString(socket->readAll());
            socket->write(ackResponse.toUtf8().data());
            socket->waitForBytesWritten();
            socket->waitForReadyRead();

            pow.message = QString(socket->readAll());
            socket->write(ackResponse.toUtf8().data());
            socket->waitForBytesWritten();
            socket->waitForReadyRead();

            vector.push_back(pow);
        }

        socket->readAll(); //OK

        QString end("end");
        socket->write(end.toUtf8().data());
        socket->waitForBytesWritten();

        connect(socket, SIGNAL(readyRead()), SLOT(updateMessage()));
    }
    return vector;
}

GroupInfo Connection::getGrupaDane(QString grupa)
{
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(updateMessage()));
    GroupInfo info;
    QString ackResponse("ack");
    QString okResponse("ok");
    socket->write("get");
    socket->waitForBytesWritten();
    socket->waitForReadyRead();
    QString received(socket->readAll());
    if (received==ackResponse)
    {
        socket->write("gdn");
        socket->waitForBytesWritten();
        socket->waitForReadyRead();
        socket->readAll(); //ACK

        socket->write(grupa.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();

        info.grupaOpis = QString(socket->readAll());

        socket->write(ackResponse.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();
        int i = socket->readAll().toInt();

        socket->write(ackResponse.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();

        //USERS
        for(; i>0; i--)
        {
            info.users.push_back(QString(socket->readAll()));
            socket->write(ackResponse.toUtf8().data());
            socket->waitForBytesWritten();
            socket->waitForReadyRead();
        }

        socket->readAll(); //ACK

        QString end("end");
        socket->write(end.toUtf8().data());
        socket->waitForBytesWritten();

        connect(socket, SIGNAL(readyRead()), SLOT(updateMessage()));
    }
    return info;
}

QVector<QString> Connection::getAllUsers()
{
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(updateMessage()));
    QVector<QString> vector;
    QString ackResponse("ack");
    socket->write("get");
    socket->waitForBytesWritten();
    socket->waitForReadyRead();
    QString received(socket->readAll());
    if (received==QString("ack"))
    {
        socket->write("usr");
        socket->waitForBytesWritten();
        socket->waitForReadyRead();
        int i = socket->readAll().toInt();
        socket->write(ackResponse.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();

        for(;i>0;i--)
        {
            vector.push_back(socket->readAll());
            socket->write(ackResponse.toUtf8().data());
            socket->waitForBytesWritten();
            socket->waitForReadyRead();
        }

        socket->readAll(); //ACK

        QString end("end");
        socket->write(end.toUtf8().data());
        socket->waitForBytesWritten();

        connect(socket, SIGNAL(readyRead()), SLOT(updateMessage()));
    }
    return vector;
}

QVector<Wiadomosc> Connection::getChatLog(QString person)
{
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(updateMessage()));
    QVector<Wiadomosc> vector;
    QString ackResponse("ack");
    socket->write("get");
    socket->waitForBytesWritten();
    socket->waitForReadyRead();
    QString received(socket->readAll());
    if (received==ackResponse)
    {
        socket->write("wia");
        socket->waitForBytesWritten();
        socket->waitForReadyRead();
        socket->readAll(); //ACK

        socket->write(person.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();

        int ilosc = socket->readAll().toInt();

        socket->write(ackResponse.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();

        //WIADOMOSC
        for(int i = 0; i<ilosc; i++)
        {
            Wiadomosc wiad;

            wiad.sender = QString(socket->readAll());
            socket->write(ackResponse.toUtf8().data());
            socket->waitForBytesWritten();
            socket->waitForReadyRead();

            wiad.message = QString(socket->readAll());
            socket->write(ackResponse.toUtf8().data());
            socket->waitForBytesWritten();
            socket->waitForReadyRead();

            wiad.date = QString(socket->readAll());
            socket->write(ackResponse.toUtf8().data());
            socket->waitForBytesWritten();
            socket->waitForReadyRead();

            vector.push_back(wiad);
        }

        socket->readAll(); //OK

        QString end("end");
        socket->write(end.toUtf8().data());
        socket->waitForBytesWritten();

        connect(socket, SIGNAL(readyRead()), SLOT(updateMessage()));
    }
    return vector;
}

void Connection::sendMessage(QString receiver, QString message)
{
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(updateMessage()));
    QString ackResponse("ack");
    socket->write("set");
    socket->waitForBytesWritten();
    socket->waitForReadyRead();
    QString received(socket->readAll());
    if (received==ackResponse)
    {
        socket->write("wia");
        socket->waitForBytesWritten();
        socket->waitForReadyRead();
        socket->readAll(); //ACK

        socket->write(receiver.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();
        socket->readAll(); //ACK

        socket->write(message.toUtf8().data());
        socket->waitForBytesWritten();
        socket->waitForReadyRead();
    }

    socket->readAll(); //OK

    QString end("end");
    socket->write(end.toUtf8().data());
    socket->waitForBytesWritten();

    connect(socket, SIGNAL(readyRead()),SLOT(updateMessage()));
}
