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

#include "wiadomoscidialog.h"
#include "ui_wiadomoscidialog.h"
#include "mainwindow.h"
#include <QDateTime>
#include <QScrollBar>

WiadomosciDialog::WiadomosciDialog(QWidget *parent, MainWindow *parentMain):
    QDialog(parent),
    parent(parentMain),
    ui(new Ui::WiadomosciDialog)
{
    ui->setupUi(this);
    connect(ui->wyslij_b, SIGNAL(clicked()), this, SLOT(sendMessage()));
}

WiadomosciDialog::~WiadomosciDialog()
{
    int d = parent->wiadomosciDialogList.indexOf(this);
    parent->wiadomosciDialogList.removeAt(d);
    delete ui;
}

void WiadomosciDialog::showDialog(QString activeUser)
{
    ui->comboBox->clear();
    QVector<QString> vector = parent->connection->getAllUsers();
    users = vector;

    if(activeUser!=QString())
    {
        ui->comboBox->addItem(activeUser);
        currentUser = activeUser;
    }

    for(int i=0; i<vector.size(); i++)
    {
        if((vector.at(i) != parent->getLoginDane().first) && (vector.at(i) != activeUser))
            ui->comboBox->addItem(vector.at(i));
    }

    if(activeUser==QString())
    {
        currentUser = ui->comboBox->currentText();
    }


    connect(ui->comboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(setChatLog(QString)));
    setChatLog(activeUser);
    show();
}

void WiadomosciDialog::setChatLog(QString person)
{
    QVector<Wiadomosc> vector = parent->connection->getChatLog(person);

    QString result = "";

    for(int i = 0; i<vector.size(); i++)
    {
        QDateTime czas = QDateTime::fromMSecsSinceEpoch(vector.at(i).date.toLongLong());
        result += vector.at(i).sender;
        result += QString(" <");
        result += czas.toString(Qt::SystemLocaleShortDate);
        result += QString("> : ");
        result += vector.at(i).message;
        result += QString("\n");
    }

    ui->status->setText(result);
}

void WiadomosciDialog::sendMessage()
{
    if(ui->wyslij_input->text() == QString(""))
        return;
    parent->connection->sendMessage(ui->comboBox->currentText(), ui->wyslij_input->text());
    setChatLog(ui->comboBox->currentText());
    ui->wyslij_input->setText(QString(""));
}
