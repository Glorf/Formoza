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

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QDesktopServices>
#include <QtNetwork/QHostAddress>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    parent(parent),
    ui(new Ui::MainWindow),
    trayIcon(new QSystemTrayIcon(this)),
    trayIconMenu(new QMenu(this)),
    quitAction(new QAction("Zamknij", this)),
    restoreAction(new QAction("Przywróć", this)),
    wiadomosciAction(new QAction("Wiadomości", this)),
    wyslij(new QAction("Wyślij wiadomość", this)),
    info(new QAction("Informacje", this)),
    connection(new Connection(this))
{
    ui->setupUi(this);
    ui->Ustawienia_status->setText(QString("Rozłączony"));
    ui->Ustawienia_wersja->setText(QString("Wersja: ")+VERSION);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
    connect (ui->Ustawienia_b_aktualizuj, SIGNAL(clicked()), this, SLOT(aktualizujButtonClicked()));
    connect (ui->Ustawienia_b_zaloguj, SIGNAL(clicked()), this, SLOT(zalogujButtonClicked()));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(show()));
    connect(wiadomosciAction, SIGNAL(triggered()), this, SLOT(showWiadomosciDialog()));
    connect(ui->Grupy_list, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(setGrupaDane(QListWidgetItem *)));
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(trayMessageClicked()));
    setIconsAndTray();
    trayMessageState.first = QString("INFORMATION");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete trayIcon;
    delete trayIconMenu;
    delete quitAction;
    delete restoreAction;
}

QPair<QString, QString> MainWindow::getLoginDane()
{
    QPair<QString, QString> dane;
    dane.first = ui->Ustawienia_login_input->text();
    dane.second = ui->Ustawienia_haslo_input->text();
    return dane;
}

void MainWindow::setGrupaDane(QListWidgetItem *item)
{
    QString grupa = grupyTable.value(item);
    GroupInfo dane = connection->getGrupaDane(grupa);
    ui->Grupy_Czlonkowie_list->clear();
    ui->Grupy_opis->setText(dane.grupaOpis);

    for(int i=0; i<dane.users.size(); i++)
    {
        QListWidgetItem *user = new QListWidgetItem(dane.users.at(i));
        ui->Grupy_Czlonkowie_list->addItem(user);
    }
}

void MainWindow::connected()
{
    ui->Ustawienia_status->setText("Połączony");
    ui->Ustawienia_b_zaloguj->setText("Wyloguj");
    ui->Ustawienia_b_zaloguj->setEnabled(true);
    //trayIconMenu->addAction(wiadomosciAction);
    disconnect (ui->Ustawienia_b_zaloguj, SIGNAL(clicked()), this, SLOT(zalogujButtonClicked()));
    connect (ui->Ustawienia_b_zaloguj, SIGNAL(clicked()), this, SLOT(setDisconnected()));
}

void MainWindow::setConnected()
{
    connection->setConnected();
}

void MainWindow::setDisconnected()
{
    connection->setDisconnected();
}

void MainWindow::disconnected()
{
    ui->Ustawienia_status->setText("Rozłączony");
    ui->Ustawienia_login_input->setEnabled(true);
    ui->Ustawienia_haslo_input->setEnabled(true);
    ui->Ustawienia_b_zaloguj->setText("Zaloguj");
    ui->Ustawienia_b_zaloguj->setEnabled(true);
    ui->OMnie->setEnabled(false);
    ui->Grupy->setEnabled(false);
    ui->Grupy_list->clear();
    ui->Grupy_Czlonkowie_list->clear();
    //trayIconMenu->removeAction(wiadomosciAction);

    disconnect (ui->Ustawienia_b_zaloguj, SIGNAL(clicked()), this, SLOT(setDisconnected()));
    connect (ui->Ustawienia_b_zaloguj, SIGNAL(clicked()), this, SLOT(zalogujButtonClicked()));
}

void MainWindow::setDane()
{
    QVector<QString> dane = connection->getDane();
    ui->OMnie_imie->setText(dane.at(0));
    ui->OMnie_nazwisko->setText(dane.at(1));
    ui->OMnie_stopien->setText(dane.at(2));
    ui->OMnie_funkcja->setText(dane.at(3));
    ui->OMnie->setEnabled(true);
}

void MainWindow::setGrupy()
{
    QVector<QPair<QString, QString> > grupy = connection->getGrupy();
    ui->Grupy_list->clear();
    grupyTable.clear();
    ui->Grupy->setEnabled(true);

    for(int i = 0; i<grupy.size(); i++)
    {
        QListWidgetItem *item = new QListWidgetItem(grupy.at(i).first);
        grupyTable.insert(item, grupy.at(i).second);
        ui->Grupy_list->addItem(item);
    }
}

void MainWindow::setIconsAndTray()
{
    setWindowIcon(QIcon(":/icons/formoza_icon.png"));
    trayIcon->setIcon(QIcon(":/icons/formoza_icon.png"));
    trayIcon->setToolTip(QString("DM Formoza"));
    trayIcon->show();
    trayIcon->showMessage(QString("DM Formoza"),QString("Aplikacja DM Formozy wersja: ") + VERSION);

    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayIconMenu);

    ui->Grupy_Czlonkowie_list->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->Grupy_Czlonkowie_list, SIGNAL(customContextMenuRequested(const QPoint &)),
    SLOT(showContextMenuForGrupyCzlonkowie(const QPoint &)));

    connect(wyslij, SIGNAL(triggered()), this, SLOT(showWiadomosciDialog()));
}

void MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason==QSystemTrayIcon::DoubleClick)
        show();
}

void MainWindow::closeEvent(QCloseEvent *event)
 {
     if (trayIcon->isVisible()) {
         hide();
         event->ignore();
     }
 }

void MainWindow::showContextMenuForGrupyCzlonkowie(const QPoint &pos)
{
    if(ui->Grupy_Czlonkowie_list->itemAt(pos)==NULL)
        return;

    if(ui->Grupy_Czlonkowie_list->itemAt(pos)->text() == ui->Ustawienia_login_input->text())
        return;

    activeGroupListItem = ui->Grupy_Czlonkowie_list->itemAt(pos);

    QMenu contextMenu(this);


    contextMenu.addAction(wyslij);
    contextMenu.addAction(info);
    contextMenu.exec(ui->Grupy_Czlonkowie_list->mapToGlobal(pos));
}

void MainWindow::showWiadomosciDialog()
{
    WiadomosciDialog* dialog = new WiadomosciDialog(parent, this);
    dialog->showDialog(activeGroupListItem->text());
    wiadomosciDialogList.push_back(dialog);
}

void MainWindow::aktualizujButtonClicked()
{
    QDesktopServices::openUrl(QUrl("http://dmformoza.no-ip.org/", QUrl::TolerantMode));
}

void MainWindow::zalogujButtonClicked()
{
    ui->Ustawienia_status->setText("Łączenie");
    connection->zaloguj(QString("dmformoza.no-ip.org"));
    ui->Ustawienia_login_input->setEnabled(false);
    ui->Ustawienia_haslo_input->setEnabled(false);
    ui->Ustawienia_b_zaloguj->setEnabled(false);
}

void MainWindow::wiadomosciDialogUpdate(QString user, QString message)
{
    int iter = 0;

    for(int i=0; i<wiadomosciDialogList.size(); i++)
    {
        if(wiadomosciDialogList.at(i)->currentUser == user)
        {
            wiadomosciDialogList.at(i)->setChatLog(user);
            iter+=1;
        }
    }

    if(iter==0)
    {
        trayIcon->showMessage(QString("Nowa wiadomość od: ") + user, QString("\"") + message + QString("\""));
        trayMessageState.first = QString("MESSAGE");
        trayMessageState.second.push_back(user);
    }
}

void MainWindow::trayMessageClicked()
{
    if(trayMessageState.first == QString("INFORMATION"))
        return;

    else if(trayMessageState.first == QString("MESSAGE"))
    {
        WiadomosciDialog* dialog = new WiadomosciDialog(parent, this);
        dialog->showDialog(trayMessageState.second.at(0));
        wiadomosciDialogList.push_back(dialog);
    }

    else if(trayMessageState.first == QString("NOTIFICATION"))
    {
        for(int i=0; i<trayMessageState.second.size(); i++)
        {
            bool shouldBe = true;
            for(int i2=0; i2<wiadomosciDialogList.size(); i2++)
            {
                if(wiadomosciDialogList.at(i2)->currentUser == trayMessageState.second.at(i))
                    shouldBe = false;
            }

            if(shouldBe)
            {
                WiadomosciDialog* dialog = new WiadomosciDialog(parent, this);
                dialog->showDialog(trayMessageState.second.at(i));
                wiadomosciDialogList.push_back(dialog);
            }
        }
    }

    trayMessageState.first = QString("INFORMATION");
    trayMessageState.second.clear();
}

void MainWindow::showPowiadomienia()
{
    QVector<Powiadomienie> vector = connection->getFirstUpdate();
    if(vector.size() == 0)
        return;

    QString powiad;

    trayMessageState.first = QString("NOTIFICATION");

    for(int i=0; i<vector.size();i++)
    {
        if(vector.at(i).type=="wia")
        {
            powiad += QString("Nowa wiadomość od: ");
            powiad += vector.at(i).message;
            powiad += QString("\n");

            trayMessageState.second.push_back(vector.at(i).message);
        }
    }

    trayIcon->showMessage(QString("Powiadomienia"), powiad);
}
