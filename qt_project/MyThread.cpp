#include "MyThread.h"
#include "MyProcess.h"
MyThread::MyThread(QObject* parent ): QThread (parent)
{

}
void MyThread::setFileLists(list<string> v)
{
    L = v;
}
void MyThread::setSz(Size sz)
{
    outSz = sz;
}
void MyThread::setNo(int no)
{
    No = no;
}
void MyThread::run()
{
    for (auto x : L)
    {
//        cout << "1"<<QString::fromStdString(x).toLocal8Bit().toStdString() << endl;
        Process p(QString::fromStdString(x).toLocal8Bit().toStdString(), outSz);
        p.start();
        Mat T;
        Mat W;
        p.getTrans(T);
        p.getWhite(W);
        resize(T,T,Size(400,400));
        resize(W,W,Size(400,400));
        emit TransDisplay(
            QPixmap::fromImage(
                QImage(
                    T.data,
                    T.cols,
                    T.rows,
                    T.step,
                    QImage::Format_RGBA8888)
                .rgbSwapped()));
        emit WhiteDisplay(
            QPixmap::fromImage(
                QImage(
                    W.data,
                    W.cols,
                    W.rows,
                    W.step,
                    QImage::Format_RGB888)
                .rgbSwapped()));
        p.Save(x);
    }
//    cout << "ok:" << No << endl;
    emit jobFinish(No);
}
