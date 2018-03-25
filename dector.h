#ifndef DECTOR_H
#define DECTOR_H
#define M_PI 3.14159265358979323846
#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <algorithm>
#include <time.h>

using namespace cv;
using namespace std;

class Dector {
private:
    bool debug = true;
    const static int RATIO = 2;//1代表640x480,2代表1280x720
    int CAR_CENTRE_COL = 0;
    int CAR_CENTRE_ROW = 0;
    int LINE_WIDTH_MAX = 0;
    uchar LINE_WIDTH_MIN = 0;
    uchar TWOTIMES_WHITE_LINE_WIDTH_MAX = 0;
    uchar ONE_TIME_THRESHOLD = 0;
    uchar MEAN_CENTRE_GAP = 0;
    uchar EDGE_CENTRE_ROW_GAP_1 = 0;
    uchar EDGE_CENTRE_COL_GAP_1 = 0;
    uchar EDGE_CENTRE_ROW_GAP_2 = 0;
    uchar EDGE_CENTRE_COL_GAP_2 = 0;
    uchar EDGE_CENTRE_ROW_GAP_3 = 0;
    uchar EDGE_CENTRE_COL_GAP_3 = 0;
    uchar KERNELS_COUNT = 0;
    uchar MARKS_GAP = 0;
    uchar SAME_LINE_ANGLE = 0;
    uchar SAME_LINE_GAP_1 = 0;
    uchar SAME_LINE_GAP_2 = 0;
    uchar AREA_1 = 0;
    uchar AREA_2 = 0;
    uchar AREA_3 = 0;
    enum Direction {BOTTOM_LEFT, BOTTOM_RIGHT, TOP_RIGHT, TOP_LEFT,NO};
    struct Line{
        int row;
        int startCol;
        int endCol;
        int width;
        bool isBlack;
    };
    struct Kernel {
        int row;
        int col;
        int type;//1：定位标识；２：导航标识
    };


public:
    volatile double position_err = 0;
    volatile double angle_err = 0;
    volatile int decode_value = 0;
    volatile int centre_y = 0;
    volatile int centre_x = 320;
    int imageCols = 0;
    int imageRows = 0;
    int roiCols = 640;
    int roiRows = 640;
    Dector();
    void cameraTest();
    void videoTest(string);
    void mediaStream(VideoCapture capture, int delay);
    void imageTest(string);
    void imageProcess(Mat &frame, Mat &thresholded);
    void perspective(Mat&);
    vector<Point> scanImageEfficiet(Mat&);
    void analyseLines(vector<Line>&lines, vector<Kernel>&, uchar*);
    vector<Point> analyseKernels(vector<Kernel> &);
    void analyseLocationKernels(vector<Kernel>&, vector<Kernel>&, vector<Kernel>&, vector<Point>&);
    void analyseNavigationKernels(vector<Kernel>&, vector<Point>&);
    Point denoiseAndCentralization(vector<Kernel>&);
    void findKernelsCentre(int*, int*, vector<Kernel> &);
    Point calculateMeanPointAndPush(uchar&, int&, int&, vector<Point> &kernelPoints);
    bool isValidPoint(Point &);
    vector<int> calculateEdges(vector<Point>&);
    int decodeImage(Mat&, vector<Point> &, vector<Point>&);
    Direction calculDirection(int, int, int, int , vector<int> edges);
    int decodeArea(vector<Point> encodePoints, Point &p1, Point &p2, Point &p5, Direction);
    void errorMeasure(vector<Point>&, Mat&);
    double calculateAngle(Point&, Point&, Point&);
    void removeRepeat(vector<Point> &, vector<Point> &, int, int);
    int judgePosition(Point&, Point&, Point&, Direction);
    Direction rotateDirection(Direction, Direction);
    void setP1P2(Direction, Point&, Point&, vector<Point>&);
    void myPutText(string text, Mat src, int x, int y);
};

#endif // DECTOR_H
