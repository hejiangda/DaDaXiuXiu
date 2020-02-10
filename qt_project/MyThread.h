#ifndef MYTHREAD_H
#define MYTHREAD_H


#include <QObject>
#include <QThread>
#include <QPixmap>
#include "opencv2/opencv.hpp"
#include <QListWidgetItem>
#include <bits/stdc++.h>
using namespace std;
using namespace cv;

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject* parent = nullptr);
    void setFileLists(list<string> v);
    void setSz(Size sz);
    void setNo(int no);
signals:
    void WhiteDisplay(QPixmap pixmap);
    void TransDisplay(QPixmap pixmap);
    void jobFinish(int flg);
public slots:

private:
    void run() override;
    list<string> L;
    Size outSz;
    int No;
};

#endif // MYTHREAD_H
