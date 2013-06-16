#-------------------------------------------------
#
# Project created by QtCreator 2013-05-17T20:41:05
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Formoza
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    wiadomoscidialog.cpp \
    connection.cpp

HEADERS  += mainwindow.hpp \
    wiadomoscidialog.hpp \
    connection.hpp

FORMS    += mainwindow.ui \
    wiadomoscidialog.ui

OTHER_FILES +=

RESOURCES += \
    Resources.qrc

RC_FILE = res.rc
