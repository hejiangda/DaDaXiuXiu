#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QFileInfoList>
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

private:
    Ui::MainWindow* ui;
};

#endif // MAINWINDOW_H
