#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileInfo>
#include <QDir>
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), addFlg(true)
{
    ui->setupUi(this);

    connect(&MT1,
            SIGNAL(WhiteDisplay(QPixmap)),
            ui->WhiteImg,
            SLOT(setPixmap(QPixmap)));

    connect(&MT1,
            SIGNAL(TransDisplay(QPixmap)),
            ui->TransImg,
            SLOT(setPixmap(QPixmap)));
    connect(&MT2,
            SIGNAL(WhiteDisplay(QPixmap)),
            ui->WhiteImg,
            SLOT(setPixmap(QPixmap)));

    connect(&MT2,
            SIGNAL(TransDisplay(QPixmap)),
            ui->TransImg,
            SLOT(setPixmap(QPixmap)));

}

MainWindow::~MainWindow()
{
    MT1.quit();
    MT2.quit();
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
                if (addFlg)
                    l1.push_back(fileInfo.filePath().toStdString());
                else
                    l2.push_back(fileInfo.filePath().toStdString());
                addFlg = !addFlg;
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
                    if (addFlg)
                        l1.push_back(fileInfo1.filePath().toStdString());
                    else
                        l2.push_back(fileInfo1.filePath().toStdString());
                    addFlg = !addFlg;
                }
            }
        }
    }
    MT1.setFileLists(l1);
    MT2.setFileLists(l2);
    if (ui->comboBox->currentIndex() == 0)
    {
        MT1.setSz(Size(800, 800));
        MT2.setSz(Size(800, 800));
    }
    else
    {
        MT1.setSz(Size(800, 1200));
        MT2.setSz(Size(800, 1200));
    }
}
//只取得该目录下的子文件不考虑子文件夹
QFileInfoList MainWindow::GetAllFileList(QString path)
{
    QDir dir(path);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    return file_list;
}
void MainWindow::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    if (!WhiteShow.isNull())
    {
        ui->WhiteImg->setPixmap(WhiteShow.scaled(ui->WhiteImg->width() - 5,
                                                 ui->WhiteImg->height() - 5,
                                                 Qt::KeepAspectRatio,
                                                 Qt::SmoothTransformation));
    }
    if (!TransShow.isNull())
    {
        ui->TransImg->setPixmap(TransShow.scaled(ui->TransImg->width() - 5,
                                                 ui->TransImg->height() - 5,
                                                 Qt::KeepAspectRatio,
                                                 Qt::SmoothTransformation));
    }
}

void MainWindow::on_imgListWidget_itemClicked(QListWidgetItem* item)
{

}

void MainWindow::on_pushButton_clicked()
{
    MT1.start();
    MT2.start();
}
