#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileInfo>
#include <QDir>
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), addFlg(0), t1ok(false), t2ok(false), t3ok(false)
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

    connect(&MT3,
            SIGNAL(WhiteDisplay(QPixmap)),
            ui->WhiteImg,
            SLOT(setPixmap(QPixmap)));

    connect(&MT3,
            SIGNAL(TransDisplay(QPixmap)),
            ui->TransImg,
            SLOT(setPixmap(QPixmap)));
    connect(&MT1,
            SIGNAL(jobFinish(int)),
            this,
            SLOT(setflgAndReOpen(int)));
    connect(&MT2,
            SIGNAL(jobFinish(int)),
            this,
            SLOT(setflgAndReOpen(int)));
    connect(&MT3,
            SIGNAL(jobFinish(int)),
            this,
            SLOT(setflgAndReOpen(int)));
    MT1.setNo(1);
    MT2.setNo(2);
    MT3.setNo(3);
}

MainWindow::~MainWindow()
{
    MT1.quit();
    MT2.quit();
    MT3.quit();
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
                if (addFlg == 0)
                    l1.push_back(fileInfo.filePath().toStdString());
                else if (addFlg == 1)
                    l2.push_back(fileInfo.filePath().toStdString());
                else if (addFlg == 2)
                    l3.push_back(fileInfo.filePath().toStdString());
                addFlg = (addFlg + 1) % 3;
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
                    if (addFlg == 0)
                        l1.push_back(fileInfo1.filePath().toStdString());
                    else if (addFlg == 1)
                        l2.push_back(fileInfo1.filePath().toStdString());
                    else if (addFlg == 2)
                        l3.push_back(fileInfo1.filePath().toStdString());
                    addFlg = (addFlg + 1) % 3;
                }
            }
        }
    }
    MT1.setFileLists(l1);
    MT2.setFileLists(l2);
    MT3.setFileLists(l3);
    if (ui->comboBox->currentIndex() == 0)
    {
        MT1.setSz(Size(800, 800));
        MT2.setSz(Size(800, 800));
        MT3.setSz(Size(800, 800));
    }
    else
    {
        MT1.setSz(Size(800, 1200));
        MT2.setSz(Size(800, 1200));
        MT3.setSz(Size(800, 1200));
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
    t1ok = false;
    MT2.start();
    t2ok = false;
    MT3.start();
    t3ok = false;
    ui->pushButton->setDisabled(true);
}
#include <QMessageBox>
void MainWindow::setflgAndReOpen(int flg)
{
//    cout << "flg Ok:" << t1ok  << " " << t2ok << " " << t3ok << endl;
    switch (flg)
    {
        case 1:
            t1ok = true;
            break;
        case 2:
            t2ok = true;
            break;
        case 3:
            t3ok = true;
            break;
    }
    if (t1ok and t2ok and t3ok)
    {
        t1ok = t2ok = t3ok = false;
        ui->pushButton->setDisabled(false);
        l1.clear();
        l2.clear();
        l3.clear();
        ui->imgListWidget->clear();
        cout << endl << "finished" << endl;
        QApplication::beep();
        QMessageBox box;
        box.setWindowTitle("电商图片自动处理");
        box.setText("所有图片已经处理完成，请添加新图片!");
        box.exec();
    }
}
