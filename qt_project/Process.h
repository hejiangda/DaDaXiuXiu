#ifndef PROCESS_H
#define PROCESS_H
#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>

#include <QObject>
#include <QPixmap>
#include <QDebug>
#include <QMutex>
#include <QReadWriteLock>
#include <QSemaphore>
#include <QWaitCondition>

using namespace std;
using namespace cv;

class Process
{
public:
    explicit Process(String name, Size sz);
    bool getWhite(Mat& W);
    bool getTrans(Mat& T);
    void start();
    bool Save(String);

private:
    vector<Point> mergeContours(vector<vector<Point>> contours, int thre);
    bool cvAdd4cMat_q(cv::Mat& dst, cv::Mat& scr, double scale);
    string getFolderPath(string str);
    string getFileName(string str);
    bool isDirExist(QString fullPath);
    void warpFfine (Mat& inputIm, Mat& tempImg, float angle);

    Mat img;
    Mat alpha;
    Mat White;
    Mat Trans;
    Size outputSz;
    String imgName;
    bool flg;
    Mat resultT, resultW;

};

#endif // PROCESS_H
