#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QPushButton>
#include <QStackedWidget>

#define PAGE_IR 0
#define PAGE_TH 1
#define PAGE_LI 2
#define PAGE_EQ 3

class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QLabel *Box1;
    QLabel *Box2;
    QLabel *Box3;
    QLabel *Box4;

    QPushButton *Infrared;
    QPushButton *Temp_Humi;
    QPushButton *Light;
    QPushButton *Equipment;

    QStackedWidget *StackedWidget;

    void Load_UI();

private slots:
    void SwitchPage_IR();
    void SwitchPage_TH();
    void SwitchPage_LI();
    void SwitchPage_EQ();
};

class UI_Thread : public QThread
{
    Q_OBJECT
public:
    UI_Thread();
    ~UI_Thread();
    MainWindow UI;

private:
    void UpdateUI();
    void run();
};
#endif // MAINWINDOW_H
