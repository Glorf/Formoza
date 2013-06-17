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

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QHostInfo>
#include <QtNetwork/QHostAddress>
#include <QCryptographicHash>
#include <QPair>

class MainWindow;

struct GroupInfo
{
    QString grupaOpis;
    QVector<QString> users;
    QVector<QString> ogloszenia;
    QHash<QString, QString> tematyOgloszen;
    QHash<QString, QString> tekstyOgloszen;
};

struct Wiadomosc
{
    QString sender;
    QString message;
    QString date;
};

struct Powiadomienie
{
    QString type;
    QString message;
};

class Connection : public QObject
{
    Q_OBJECT

public:
    Connection(MainWindow *parent = 0);
    ~Connection();
    void zaloguj(QString hostname);
    QVector<QString> getDane();
    QVector<QPair<QString, QString> > getGrupy();
    GroupInfo getGrupaDane(QString grupa);
    QVector<QString> getAllUsers();
    QVector<Wiadomosc> getChatLog(QString person);
    QVector<Powiadomienie> getFirstUpdate();
    void sendMessage(QString receiver, QString message);

public slots:
    void connectHost(QHostInfo hostinfo);
    void setConnected();
    void setDisconnected();
    void updateMessage();

private:
    MainWindow* parent;
    QTcpSocket* socket;
    QCryptographicHash *cryptPass;
    QString sendCommand(QString cmd, QString arg);
};

#endif // CONNECTION_HPP
