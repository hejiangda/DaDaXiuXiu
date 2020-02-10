#include "MyThread.h"
#include "Process.h"
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
void MyThread::run()
{
    for (auto x : L)
    {
        Process p(x, outSz);
        p.start();

        Mat T;
        Mat W;
        p.getTrans(T);
        p.getWhite(W);
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

}
