#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QImage *img = new QImage(); //新建一个image对象
    img->load(":/login.jpg"); //将图像资源载入对象img，注意路径，可点进图片右键复制路径
    ui->label->setPixmap(QPixmap::fromImage(*img)); //将图片放入label，使用setPixmap,注意指针*img


//    QPixmap pixmap("./dog.jpg");
//    ui->label->setPixmap(pixmap);

}

MainWindow::~MainWindow()
{
    delete ui;
}
