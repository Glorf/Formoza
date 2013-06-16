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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define VERSION QString("Alpha 7")

#include <QMainWindow>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QMenu>
#include <QBuffer>
#include <QListWidgetItem>

#include "wiadomoscidialog.h"
#include "connection.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Connection* connection;
    QPair<QString, QString> getLoginDane();
    QList<WiadomosciDialog*> wiadomosciDialogList;
    void connected();
    void disconnected();
    void setDane();
    void setGrupy();
    void wiadomosciDialogUpdate(QString user, QString message);

private slots:
    void aktualizujButtonClicked();
    void zalogujButtonClicked();
    void setGrupaDane(QListWidgetItem *item);
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void showContextMenuForGrupyCzlonkowie(const QPoint &pos);
    void showWiadomosciDialog();
    void setConnected();
    void setDisconnected();
    void trayMessageClicked();
    
private:
    QWidget *parent;
    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QAction *quitAction;
    QAction *restoreAction;
    QAction *wiadomosciAction;

    QAction *wyslij;
    QAction *info;

    QPair<QString, QString> trayMessageState;

    void setIconsAndTray();

    QHash<QListWidgetItem*, QString> grupyTable;
    void closeEvent(QCloseEvent *event);
    QListWidgetItem* activeGroupListItem;
};

#endif // MAINWINDOW_H
