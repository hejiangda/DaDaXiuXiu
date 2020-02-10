#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QFileInfoList>
#include <Process.h>
#include <QListWidgetItem>
#include "MyThread.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    QFileInfoList GetAllFileList(QString path);
protected:
    void dragEnterEvent(QDragEnterEvent* e);//重新实现两个事件处理函数
    void dropEvent(QDropEvent* e);
    void resizeEvent(QResizeEvent* event);

private slots:
    void on_imgListWidget_itemClicked(QListWidgetItem* item);

    void on_pushButton_clicked();

private:
    Ui::MainWindow* ui;
    QPixmap TransShow, WhiteShow;
    Process* pro;
    list<string> l1, l2, l3;
    MyThread MT1, MT2, MT3;
    int addFlg;

};

#endif // MAINWINDOW_H
