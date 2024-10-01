#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication3.h"
#include <QApplication>
#include <QMediaPlayer>
#include "qvideowidget.h"
#include <QtWidgets/QMainWindow>

class QtWidgetsApplication3 : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsApplication3(QWidget *parent = nullptr);
    ~QtWidgetsApplication3();
    QString lineText;
    QString fileName_output;
private slots:
   
    void onStartClicked();
    void onStartClicked2();
    void onLineEditTextChanged();
private:
    Ui::QtWidgetsApplication3Class ui;
    QMediaPlayer* player1;
    QMediaPlayer* player2;
   
};
