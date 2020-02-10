#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileInfo>
#include <QDir>
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent* e)
{
    if (e->mimeData()->hasUrls()) //判断数据类型
    {
        e->acceptProposedAction();//接收该数据类型拖拽事件
    }
    else
    {
        e->ignore();//忽略
    }

}
void MainWindow::dropEvent(QDropEvent* event)
{
    //窗口部件放下一个对象时,调用该函数
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
    {
        event->ignore();
        return;
    }
    QStringList acceptedFileTypes;
    acceptedFileTypes.append("jpg");
    acceptedFileTypes.append("png");
    acceptedFileTypes.append("bmp");

    QList<QUrl>::iterator it;
    for (it = urls.begin(); it != urls.end(); it++)
    {
        QUrl url = *it;
        QFileInfo fileInfo(it->toLocalFile());
        if (fileInfo.isFile())
        {
            if (acceptedFileTypes.contains(fileInfo.suffix().toLower()))
            {
                //是文件就直接加进来
                ui->imgListWidget->addItem(fileInfo.filePath());
            }
        }
        else if (fileInfo.isDir())
        {
            //文件路径就循环读取
            QFileInfoList fileInfoList = GetAllFileList(fileInfo.filePath());
            for (QFileInfo fileInfo1 : fileInfoList)
            {
                if (acceptedFileTypes.contains(fileInfo1.suffix().toLower()))
                {
                    ui->imgListWidget->addItem(fileInfo1.filePath());
                }
            }
        }
    }
}
//循环读取文件路径,后期可以考虑换成多线程-线程池来做.
QFileInfoList MainWindow::GetAllFileList(QString path)
{
    QDir dir(path);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (int i = 0; i != folder_list.size(); i++)
    {
        QString name = folder_list.at(i).absoluteFilePath();
        QFileInfoList child_file_list = GetAllFileList(name);
        file_list.append(child_file_list);
    }

    return file_list;
}
