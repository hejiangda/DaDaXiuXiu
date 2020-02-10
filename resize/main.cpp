#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
//获取点间距离
double getDistance(Point A, Point B)
{
    double dis;
    dis = pow((A.x - B.x), 2) + pow((A.y - B.y), 2);
    return sqrt(dis);
}
/**
 * 拼接合并轮廓集合
 * @param contours
 * @return
 */
vector<Point> mergeContours(vector<vector<Point>> contours, double thre)
{

//    vector<vector<Point>> allcontours;
    vector<Point> contourslist;

    for (int i = 0; i < contours.size(); i++)
    {
        vector<Point> vec_i;
        vec_i = contours[i];
        Rect rect = boundingRect(contours[i]);

        //将矩形的四个点保存在P中

        if (rect.area() > thre)
            contourslist.insert(contourslist.end(), vec_i.begin(), vec_i.end());
    }

    return contourslist;
}
//void createAlphaMat(Mat &mat)
//{
//    for (int i = 0; i < mat.rows; ++i) {
//        for (int j = 0; j < mat.cols; ++j) {
//            Vec4b& rgba = mat.at<Vec4b>(i, j);
//            rgba[0] = UCHAR_MAX;
//            rgba[1] = saturate_cast<uchar>((float (mat.cols - j)) / ((float)mat.cols) * UCHAR_MAX);
//            rgba[2] = saturate_cast<uchar>((float (mat.rows - i)) / ((float)mat.rows) * UCHAR_MAX);
//            rgba[3] = saturate_cast<uchar>(0.5 * (rgba[1] + rgba[2]));
//        }
//    }
//}

//        int index = 0;
//        for (; index >= 0; index = hierarchy[index][0])
//        {
//            Scalar color(rand() & 255, rand() & 255, rand() & 255);
//            drawContours(img, contours, index, color, CV_FILLED, 8, hierarchy);
//        }


int cvAdd4cMat_q(cv::Mat& dst, cv::Mat& scr, double scale)
{
    if (dst.channels() != 3 || scr.channels() != 4)
    {
        return true;
    }
    if (scale < 0.01)
        return false;
    std::vector<cv::Mat>scr_channels;
    std::vector<cv::Mat>dstt_channels;
    split(scr, scr_channels);
    split(dst, dstt_channels);
    CV_Assert(scr_channels.size() == 4 && dstt_channels.size() == 3);

    if (scale < 1)
    {
        scr_channels[3] *= scale;
        scale = 1;
    }
    for (int i = 0; i < 3; i++)
    {
        dstt_channels[i] = dstt_channels[i].mul(255.0 / scale - scr_channels[3], scale / 255.0);
        dstt_channels[i] += scr_channels[i].mul(scr_channels[3], scale / 255.0);
    }
    merge(dstt_channels, dst);
    return true;
}

int main()
{
//    "/home/happy/桌面/tupian/IMG_4286_¸±±¾.png"
//    Mat img = imread("/home/happy/桌面/pic/269700986621646269.jpg", CV_LOAD_IMAGE_UNCHANGED);
    Mat img = imread("/home/happy/桌面/tupian/IMG_4286_¸±±¾.png", CV_LOAD_IMAGE_UNCHANGED);

    Mat res;
    Mat alpha;
    if (img.channels() == 4)
    {
        vector<Mat>chann;
        split(img, chann);
        Mat GrayImg = chann[3];

        vector<vector<Point>> contours;
        vector<Vec4i>hierarchy;
        findContours(GrayImg, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        double sizOfOri = img.cols * img.rows;
        vector<Point>newCon = mergeContours(contours, sizOfOri / 100);
//        drawContours(GrayImg, contours, -1, Scalar(255), CV_FILLED, 8, hierarchy);

        Rect rect = boundingRect(newCon);

        alpha = GrayImg(rect);

        vector<Mat>chn;
        chn.push_back(chann[0](rect));
        chn.push_back(chann[1](rect));
        chn.push_back(chann[2](rect));
        chn.push_back(alpha);

        merge(chn, res);
    }
    else
    {
        Mat GrayImg;
        cvtColor(img, GrayImg, CV_BGR2GRAY);
        threshold(GrayImg, GrayImg, 254, 255, CV_THRESH_BINARY_INV);

        vector<vector<Point>> contours;
        vector<Vec4i>hierarchy;
        findContours(GrayImg, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        double sizOfOri = img.cols * img.rows;

        vector<Point>newCon = mergeContours(contours, sizOfOri / 100);

        drawContours(GrayImg, contours, -1, Scalar(255), CV_FILLED, 8, hierarchy);


        Rect rect = boundingRect(newCon);
        Mat final = img(rect);
        alpha = GrayImg(rect);

        blur(alpha, alpha, Size(5, 5));

        vector<Mat>chn;
        chn.push_back(final);
        chn.push_back(alpha);

        merge(chn, res);


    }
    imshow("res", res);
    waitKey(0);
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    imwrite("alpha.png", res, compression_params);

    return 0;
}
