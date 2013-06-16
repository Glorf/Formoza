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

#ifndef WIADOMOSCIDIALOG_H
#define WIADOMOSCIDIALOG_H

#include <QDialog>

namespace Ui {
class WiadomosciDialog;
}

class MainWindow;

class WiadomosciDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WiadomosciDialog(QWidget *parent = 0, MainWindow *parentMain = 0);
    ~WiadomosciDialog();
    void showDialog(QString activeUser = QString());
    QString currentUser;

public slots:
    void setChatLog(QString person);
    void sendMessage();

private:
    MainWindow *parent;
    Ui::WiadomosciDialog *ui;
    QVector<QString> users;
};


#endif // WIADOMOSCIDIALOG_H
