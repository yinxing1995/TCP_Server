#include "mainwindow.h"
#include "datastructure.h"
#include <QDebug>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this -> setWindowTitle(tr("Fire Detecting System"));
    this -> resize(600,400);
    Load_UI();
    /*
    Box = new QLabel(this);
    Box -> move(150, 100);
    Box -> setText(tr("UI is under designing"));
    */

}

MainWindow::~MainWindow()
{
}

void MainWindow::Load_UI()
{
    StackedWidget = new QStackedWidget(this);

    Infrared = new QPushButton(this);
    Infrared -> setText(tr("IR"));
    Infrared -> move(450,25);

    Temp_Humi = new QPushButton(this);
    Temp_Humi -> setText(tr("TH"));
    Temp_Humi -> move(450,125);

    Light = new QPushButton(this);
    Light -> setText(tr("LI"));
    Light -> move(450,225);

    Equipment = new QPushButton(this);
    Equipment -> setText(tr("EQ"));
    Equipment -> move(450,325);

    Box1 = new QLabel(this);
    Box2 = new QLabel(this);
    Box3 = new QLabel(this);
    Box4 = new QLabel(this);

    Box1 -> setText(tr("Infrared Sensor"));
    Box2 -> setText(tr("Temprature & Humidity"));
    Box3 -> setText(tr("Light sensor"));
    Box4 -> setText(tr("Equipment"));

    StackedWidget -> addWidget(Box1);
    StackedWidget -> addWidget(Box2);
    StackedWidget -> addWidget(Box3);
    StackedWidget -> addWidget(Box4);

    StackedWidget -> resize(600,400);
    StackedWidget -> move(200,-150);

    connect(Infrared,SIGNAL(clicked()),this,SLOT(SwitchPage_IR()));
    connect(Temp_Humi,SIGNAL(clicked()),this,SLOT(SwitchPage_TH()));
    connect(Light,SIGNAL(clicked()),this,SLOT(SwitchPage_LI()));
    connect(Equipment,SIGNAL(clicked()),this,SLOT(SwitchPage_EQ()));
}

void MainWindow::SwitchPage_IR()
{
    StackedWidget -> setCurrentIndex(PAGE_IR);
    Box1->move(0,0);
}

void MainWindow::SwitchPage_TH()
{
    StackedWidget -> setCurrentIndex(PAGE_TH);
}

void MainWindow::SwitchPage_LI()
{
    StackedWidget -> setCurrentIndex(PAGE_LI);
}

void MainWindow::SwitchPage_EQ()
{
    StackedWidget -> setCurrentIndex(PAGE_EQ);
}

UI_Thread::UI_Thread()
{
    MainWindow ui;
    this->start();
}

UI_Thread::~UI_Thread()
{

}

void UI_Thread::run()
{
    QString *Message;
    while(1)
    {
        Message = ProcessMessage();
        if(!Message)
        {
            msleep(20);
        }
        else
        {
            //UI.Box -> setText(*Message);
            delete Message;
            msleep(5);
        }
        Message = NULL;
    }
}
