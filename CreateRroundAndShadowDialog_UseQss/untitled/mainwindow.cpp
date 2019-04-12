    #include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QGraphicsDropShadowEffect"
#include <QFile>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //ui->frame->setStyleSheet("QWidget{background-color:gray;border-top-left-radius:15px;border-top-right-radius:15px;border-bottom-left-radius:15px;border-bottom-right-radius:15px;}");



    setWindowFlags(Qt::FramelessWindowHint);//设置无标题
    setAttribute(Qt::WA_TranslucentBackground);//设置窗口透明

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
    effect->setOffset(4,4);
    effect->setColor(QColor(0,0,0,50));
    effect->setBlurRadius(10);
    ui->frame->setGraphicsEffect(effect);

}

MainWindow::~MainWindow()
{
    delete ui;
}



