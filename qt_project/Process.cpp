#include "Process.h"
#include <QDir>
Process::Process(String name, Size sz):  outputSz(sz), imgName(name), flg(false)
{

}
static void getBinMask( const Mat& comMask, Mat& binMask )
{
    if ( comMask.empty() || comMask.type() != CV_8UC1 )
        CV_Error( Error::StsBadArg, "comMask is empty or has incorrect type (not CV_8UC1)" );
    if ( binMask.empty() || binMask.rows != comMask.rows || binMask.cols != comMask.cols )
        binMask.create( comMask.size(), CV_8UC1 );
    binMask = comMask & 1;
}
void Process::start()
{
    cout << "fileName: " << imgName << endl;
    img = imread(imgName, CV_LOAD_IMAGE_UNCHANGED);
    if (img.cols > 1600)
        resize(img, img, Size(1600, 1600 * img.rows / img.cols));
    int sizOfOri = img.cols * img.rows;
    float rotAngle = 0;

    if (img.channels() == 4)
    {
        vector<Mat>chann;
        split(img, chann);
        Mat GrayImg = chann[3];

        vector<vector<Point>> contours;
        vector<Vec4i>hierarchy;
        findContours(GrayImg, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        vector<Point>newCon = mergeContours(contours, sizOfOri / 100);

        RotatedRect rrect = minAreaRect(newCon);
        cout << rrect.angle;
        rotAngle = rrect.angle;

        Mat tempGray;
        warpFfine(GrayImg, tempGray, rotAngle);
        Mat tempImg;
        warpFfine(img, tempImg, rotAngle);

        contours.clear();
        hierarchy.clear();
        findContours(tempGray, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        newCon.clear();
        newCon = mergeContours(contours, sizOfOri / 100);
        Rect rect = boundingRect(newCon);
        alpha = tempGray(rect);
        GaussianBlur(alpha, alpha, Size(3, 3), 1, 1);
        vector<Mat>chn;
        split(tempImg(rect), chn);
        chn.pop_back();
        chn.push_back(alpha);

        merge(chn, Trans);
    }
    else
    {
        // GrabCut
        Mat mask;
        Mat bgdModel, fgdModel;
        if ( !mask.empty() )
            mask.setTo(Scalar::all(GC_BGD));

        Rect r = Rect(Point(1, 1), Point(img.cols - 2, img.rows - 2));
        grabCut( img, mask, r, bgdModel, fgdModel, 3, GC_INIT_WITH_RECT );
        Mat binMask;

        getBinMask( mask, binMask );
        threshold(binMask, binMask, 0, 255, CV_THRESH_BINARY);
        // GrabCut End

        vector<vector<Point>> contours;
        vector<Vec4i>hierarchy;
        findContours(binMask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        vector<Point>newCon = mergeContours(contours, sizOfOri / 10);

        RotatedRect rrect = minAreaRect(newCon);
        cout << rrect.angle;
        rotAngle = rrect.angle;

        Mat tempGray;
        warpFfine(binMask, tempGray, rotAngle);
        Mat tempImg;
        warpFfine(img, tempImg, rotAngle);

        contours.clear();
        hierarchy.clear();
        findContours(tempGray, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        newCon.clear();
        newCon = mergeContours(contours, sizOfOri / 100);
        Rect rect = boundingRect(newCon);
        alpha = tempGray(rect);
        GaussianBlur(alpha, alpha, Size(3, 3), 1, 1);
        vector<Mat>chn;
        chn.push_back(tempImg(rect));
        chn.push_back(alpha);

        merge(chn, Trans);
    }

    White = Mat(Trans.size(), CV_8UC3, Scalar::all(255));
    cvAdd4cMat_q(White, Trans, 1);
    int x, y;
    x = Trans.cols;
    y = Trans.rows;
    int z = x > y ? x : y;
    resultT = Mat(z, z, CV_8UC4, Scalar(255, 255, 255, 0));
    resultW = Mat(z, z, CV_8UC3, Scalar(255, 255, 255));
    if (x == z)
    {
        Rect r(0, (x - y) / 2, x, y);
        Mat dst = resultT(r);
        Trans.convertTo(dst, dst.type());
        dst = resultW(r);
        White.convertTo(dst, dst.type());
    }
    else
    {
        Rect r((y - x) / 2, 0, x, y);
        Mat dst = resultT(r);
        Trans.convertTo(dst, dst.type());
        dst = resultW(r);
        White.convertTo(dst, dst.type());
    }
    resize(resultT, resultT, outputSz);
    resize(resultW, resultW, outputSz);

    flg = true;
}
/**
 * 拼接合并轮廓集合
 * @param contours
 * @return vector<Point> contourslist
 */
vector<Point> Process::mergeContours(vector<vector<Point>> contours, int thre)
{
    vector<Point> contourslist;

    for (int i = 0; i < contours.size(); i++)
    {
        Rect rect = boundingRect(contours[i]);
        if (rect.area() > thre)
        {
            vector<Point> vec_i;
            vec_i = contours[i];
            contourslist.insert(contourslist.end(), vec_i.begin(), vec_i.end());
        }
    }
    return contourslist;
}
bool Process::cvAdd4cMat_q(cv::Mat& dst, cv::Mat& scr, double scale)
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
// 取文件名字 不包括后缀
string Process::getFileName(string str)
{
    string::size_type idx = str.rfind('/', str.length());
    string::size_type pidx = str.rfind('.', str.length());
    string filename = str.substr(idx + 1, pidx - (idx + 1));
    return filename;
}
// 取文件夹名字 无后缀
string Process::getFolderPath(string str)
{
    string::size_type idx = str.rfind('/', str.length());
    string folder = str.substr(0, idx);
    return folder;
}
bool Process::Save(String name)
{
    if (!flg)
        return false;

    string folder = getFolderPath(name);
    string fileName = getFileName(name);
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    isDirExist(QString::fromStdString(folder + "/透明图"));
    isDirExist(QString::fromStdString(folder + "/白底图"));
    imwrite(folder + "/透明图/" + fileName + "-T.png", resultT, compression_params);
    imwrite(folder + "/白底图/" + fileName + "-W.jpg", resultW);
    return true;
}
bool Process::isDirExist(QString fullPath)
{
    QDir dir(fullPath);
    if (dir.exists())
    {
        return true;
    }
    else
    {
        bool ok = dir.mkpath(fullPath);//创建多级目录
        return ok;
    }
}
bool Process::getWhite(Mat& W)
{
    if (!flg)
        return false;
    W = resultW;

    return true;
}
bool Process::getTrans(Mat& T)
{
    if (!flg)
        return false;
    T = resultT;
    return true;
}
/*****************************************
*inputim:要旋转的图像
*tempImg:结果图像
*angle:旋转角度，（逆时针为正，顺时针为负）
*/
void Process::warpFfine (Mat& inputIm, Mat& tempImg, float angle)
{

    CV_Assert(!inputIm.empty());
    Mat inputImg;
    inputIm.copyTo(inputImg);

    float radian = (float) (angle / 180.0 * CV_PI);

    //填充图像使其符合旋转要求
    int uniSize = (int) ( max(inputImg.cols, inputImg.rows) * 1.414 );
    int dx = (int) (uniSize - inputImg.cols) / 2;
    int dy = (int) (uniSize - inputImg.rows) / 2;

    copyMakeBorder(inputImg, tempImg, dy, dy, dx, dx, BORDER_CONSTANT);

    //旋轉中心
    Point2f center( (float)(tempImg.cols / 2), (float) (tempImg.rows / 2));
    Mat affine_matrix = getRotationMatrix2D( center, angle, 1.0 );

    //旋轉
    warpAffine(tempImg, tempImg, affine_matrix, tempImg.size());

    //旋轉后的圖像大小
    float sinVal = fabs(sin(radian));
    float cosVal = fabs(cos(radian));

    Size targetSize( (int)(inputImg.cols * cosVal + inputImg.rows * sinVal),
                     (int)(inputImg.cols * sinVal + inputImg.rows * cosVal) );

    //剪掉四周边框
    int x = (tempImg.cols - targetSize.width) / 2;
    int y = (tempImg.rows - targetSize.height) / 2;

    Rect rect(x, y, targetSize.width, targetSize.height);
    tempImg = Mat(tempImg, rect);
}
