#include "dector.h"

Dector::Dector() {
    switch (RATIO) {
    case 1:
        LINE_WIDTH_MAX = 50;
        LINE_WIDTH_MIN = 2;
        TWOTIMES_WHITE_LINE_WIDTH_MAX = 5;
        ONE_TIME_THRESHOLD = 4;
        MEAN_CENTRE_GAP = 35;
        EDGE_CENTRE_ROW_GAP_1 = 20;
        EDGE_CENTRE_COL_GAP_1 = 35;
        KERNELS_COUNT = 2;
        MARKS_GAP = 100;
        SAME_LINE_ANGLE = 20;
        SAME_LINE_GAP_1 = 5;
        AREA_1 = 70;
        AREA_2 = 140;
        AREA_3 = 200;
        imageCols = 640;
        imageRows = 480;
        CAR_CENTRE_COL = imageCols/2 - 20 - centre_x;
        break;
    case 2:
        LINE_WIDTH_MAX = 50;
        LINE_WIDTH_MIN = 2;
        TWOTIMES_WHITE_LINE_WIDTH_MAX = 8;
        ONE_TIME_THRESHOLD = 4;
        MEAN_CENTRE_GAP = 35;
        EDGE_CENTRE_ROW_GAP_1 = 8;
        EDGE_CENTRE_COL_GAP_1 = 14;
        EDGE_CENTRE_ROW_GAP_2 = 16;
        EDGE_CENTRE_COL_GAP_2 = 27;
        EDGE_CENTRE_ROW_GAP_3 = 20;
        EDGE_CENTRE_COL_GAP_3 = 40;
        KERNELS_COUNT = 2;
        MARKS_GAP = 180;
        SAME_LINE_ANGLE = 20;
        SAME_LINE_GAP_1 = 12;
        SAME_LINE_GAP_2 = 20;
        AREA_1 = 100;
        AREA_2 = 150;
        AREA_3 = 200;
        imageCols = 1280;
        imageRows = 720;
        CAR_CENTRE_COL = imageCols/2 - 50 - centre_x;
        CAR_CENTRE_ROW = imageRows/3 - 78 - roiRows;
        break;
    default:
        break;
    }
}

void Dector::cameraTest(){
    VideoCapture capture(1);
    capture.set(CV_CAP_PROP_FRAME_WIDTH, imageCols);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, imageRows);
    int delay = 0;
    if(debug) {
        delay = 30;
    }
    if (capture.isOpened()) {
        mediaStream(capture, delay);
    }
}


void Dector::videoTest(string fileName) {
    VideoCapture capture;
    capture.open(fileName);

    double rate = capture.get(CV_CAP_PROP_FPS);//获取视频文件的帧率
    int delay = cvRound(1000.000 / rate);
    if(debug){
        cout << "delay" << delay << endl;
    }

    if (capture.isOpened()) {
        mediaStream(capture, delay);
    }
}

void Dector::mediaStream(VideoCapture capture, int delay){
    uchar count = 0;
    double totalTime = 0;
    while (true){
        Mat frame, thresholded;
        capture >> frame;//读出每一帧的图像
        if (frame.empty()) break;

        if(centre_x + roiCols > frame.cols) {
            centre_x = 320;
        }

        int roi_x = centre_x + last_roi_x - 320;
        CAR_CENTRE_COL = frame.cols/2 - 50 - roi_x;
        Mat srcROI(frame, Rect(roi_x, imageRows - roiRows, roiCols, roiRows));
        last_roi_x = roi_x;

        clock_t start = clock();

        imageProcess(srcROI, thresholded);

        if(debug){
            totalTime += (clock() - start);
            count++;
            if (count == 1) {
                count = 0;
                totalTime = 0;

                stringstream ss;
                ss << (double)(totalTime/count)/CLOCKS_PER_SEC;
                std::string text = ss.str();
                myPutText(text, srcROI, 100, 100);
            }
            imshow("after", thresholded);
            moveWindow("after", 900, 0);
            imshow("before", srcROI);
            moveWindow("before", 0, 0);
        }

        if(delay != 0){
            waitKey(delay);
        }
    }
}

void Dector::imageTest(string fileName) {
    Mat frame = imread(fileName);
    Mat srcROI(frame, Rect(320, 80, 640, 640));
    Mat thresholded;

    clock_t start = clock();

    imageProcess(srcROI, thresholded);

    cout << "precessed time:" << (double)(clock() - start)/CLOCKS_PER_SEC << endl;
    cout << "fileName:" << fileName << endl;
    cout << endl;

    imshow(" after", thresholded);
    moveWindow(" after", 900, 0);
    imshow(" before", srcROI);
    moveWindow(" before", 0, 0);

    waitKey(0);
}

void Dector::imageProcess(Mat& frame, Mat& thresholded){
    Mat gary;
    cvtColor(frame, gary, CV_RGB2GRAY);

    clock_t start1 = clock();
    adaptiveThreshold(gary, thresholded, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 81, 40);
    if(debug) {
        cout << "threshold time:" << (double)(clock() - start1)/CLOCKS_PER_SEC << endl;
    }

    clock_t start2 = clock();
    vector<Point> points = scanImageEfficiet(thresholded);
    if(debug) {
        cout << "scanImageEfficiet time:" << (double)(clock() - start2)/CLOCKS_PER_SEC << endl;
    }

    clock_t start3 = clock();
    if(points.size() >= 6 && isValidPoint(points[3]) && isValidPoint(points[4]) && isValidPoint(points[5])) {
        errorMeasure(points, frame);
    }
    if(debug) {
        cout << "errorMeasure time:" << (double)(clock() - start3)/CLOCKS_PER_SEC << endl;

        int size = points.size();
        for(int i = 0; i < size; i++){
            if(isValidPoint(points[i])) {
                if(i == 0) {
                    circle(frame, points[i], 3, Scalar(0, 255, 0));
                } else {
                    circle(frame, points[i], 3, Scalar(0, 0, 255));
                }
            }
        }
    }


    if(points.size() >= 6 && isValidPoint(points[0]) && isValidPoint(points[1]) && isValidPoint(points[2])
            && isValidPoint(points[3]) && isValidPoint(points[4]) && isValidPoint(points[5])) {
        centre_x = points[5].x;
        centre_y = points[5].y;

        clock_t start4 = clock();
        vector<Point> encodePoints;
        decode_value = decodeImage(thresholded, points, encodePoints);

        if(debug) {
            myPutText(to_string(decode_value), frame, 100, 300);
            cout << "decode time:" << (double)(clock() - start4)/CLOCKS_PER_SEC << endl;
            for(vector<Point>::iterator it = encodePoints.begin(); it != encodePoints.end(); ++it) {
                if(it->x != 0 && it->y != 0) {
                    circle(frame, *it, 3, Scalar(0, 245, 255));
                }
            }
            vector<int> edges = calculateEdges(points);
            int col_gap = 0;
            int row_gap = 0;
            if(points[5].y <= imageRows/3) {
                col_gap = EDGE_CENTRE_COL_GAP_1;
                row_gap = EDGE_CENTRE_ROW_GAP_1;
            } else if(points[5].y > imageRows/3 && points[5].y <= 2*imageRows/3) {
                col_gap = EDGE_CENTRE_COL_GAP_2;
                row_gap = EDGE_CENTRE_ROW_GAP_2;
            } else if(points[5].y > 2*imageRows/3) {
                col_gap = EDGE_CENTRE_COL_GAP_3;
                row_gap = EDGE_CENTRE_ROW_GAP_3;
            }
            Point p1 = {edges[0] - col_gap, edges[2] - row_gap};
            Point p2 = {edges[1] + col_gap, edges[3] + row_gap};
            rectangle(frame, p1, p2, Scalar(0, 0, 255), 1, 1, 0);
        }
    }
}

void Dector::errorMeasure(vector<Point> & points, Mat& img){
    if(debug) {
        line(img, Point(CAR_CENTRE_COL, 0), Point(CAR_CENTRE_COL, imageRows), Scalar(255, 215, 0), 1, 8, 0);
        line(img, Point(0, CAR_CENTRE_ROW), Point(imageCols, CAR_CENTRE_ROW), Scalar(255, 215, 0), 1, 8, 0);
    }
    if(points.size() >= 6 && isValidPoint(points[3]) && isValidPoint(points[4]) && isValidPoint(points[5])) {
        if(debug) {
            if((points[3].y - points[5].y)*(points[4].x - points[5].x)
                    == (points[4].y - points[5].y)*(points[3].x - points[5].x)) {
                line(img, points[4], points[3], Scalar(255, 0, 0), 1, 8, 0);
            } else {
                line(img, points[3], points[5], Scalar(0, 255, 0), 1, 8, 0);
                line(img, points[4], points[5], Scalar(0, 0, 255), 1, 8, 0);
            }
        }
        if(false) {//角度误差暂时不用
            float vertical_sum_x2 = pow(points[3].x, 2) + pow(points[4].x, 2) + pow(points[5].x, 2);
            float vertical_sum_x = points[3].x + points[4].x + points[5].x;
            float vertical_sum_y = points[3].y + points[4].y + points[5].y;
            float vertical_sum_xy = points[3].x*points[3].y + points[4].x*points[4].y + points[5].x*points[5].y;

            float vertical_mm = (3*vertical_sum_x2 - pow(vertical_sum_x, 2));
            double angle_vertical = 90;
            if(vertical_mm != 0) {
                float B1 = (3*vertical_sum_xy - vertical_sum_x*vertical_sum_y)/vertical_mm;
                if(B1 >= 0) {
                    angle_vertical = atan(B1)*180/M_PI;
                } else {
                    angle_vertical = 180 + atan(B1)*180/M_PI;
                }
            }

            float horizontal_sum_x2 = pow(points[1].x, 2) + pow(points[2].x, 2) + pow(points[5].x, 2);
            float horizontal_sum_x = points[1].x + points[2].x + points[5].x;
            float horizontal_sum_y = points[1].y + points[2].y + points[5].y;
            float horizontal_sum_xy = points[1].x*points[1].y + points[2].x*points[2].y + points[5].x*points[5].y;

            float horizontal_mm = (3*horizontal_sum_x2 - pow(horizontal_sum_x, 2));
            double angle_horizontal = 90;
            if(horizontal_mm != 0) {
                float B1 = (3*horizontal_sum_xy - horizontal_sum_x*horizontal_sum_y)/horizontal_mm;
                angle_horizontal = atan(B1)*180/M_PI;
            }
            angle_err = angle_horizontal;
        }

        position_err = CAR_CENTRE_COL - points[5].x;
    }
}

int Dector::decodeImage(Mat& thresholded, vector<Point>& kernelPoints, vector<Point>& encodePoints) {
    int result = 0;

    int cols = kernelPoints[1].x - 30;
    int rows = kernelPoints[3].y - 20;
    int width = kernelPoints[2].x - kernelPoints[1].x + 70;
    int high = kernelPoints[4].y - kernelPoints[3].y + 40;
    if(cols < 0) {
        cols = 0;
    }
    if(cols + width > roiCols) {
        width = roiCols - cols;
    }
    if(rows < 0) {
        rows = 0;
    }
    if(rows + high > roiRows) {
        high = roiRows - rows;
    }
    Mat thresholdedROI(thresholded, Rect(cols, rows, width, high));

    vector<int> edges = calculateEdges(kernelPoints);
    if(edges.size() < 4) {
        return 0;
    }

    int centreRow = kernelPoints[5].y;
    int centreCol = kernelPoints[5].x;
    int area = 0;
    if(centreRow <= imageRows/3) {
        area = AREA_1;
    } else if(centreRow > imageRows/3 && centreRow <= 2*imageRows/3) {
        area = AREA_2;
    } else if(centreRow > 2*imageRows/3) {
        area = AREA_3;
    }

    int g_nThresh = 80;
    Mat cannyMat;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    Canny( thresholdedROI, cannyMat, g_nThresh, g_nThresh*2, 3 );
    findContours( cannyMat, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    vector<Point> bottom_left, bottom_right, top_right;
    Direction bottom_left_relative_dir, bottom_right_relative_dir, top_right_relative_dir;
    Point bottom_left_p1, bottom_left_p2, bottom_right_p1, bottom_right_p2, top_right_p1, top_right_p2;
    Direction locateDir = calculDirection(kernelPoints[0].x, kernelPoints[0].y, centreCol, centreRow, edges);
    for( int i = 0; i < (int)contours.size(); i++ ) {
        Rect rect = boundingRect(contours.at(i));
        int x = rect.x + rect.width/2 + cols;
        int y = rect.y + rect.height/2 + rows;

        if(rect.area() > area && rect.width < 46 && rect.height < 46) {
            Direction relative_dir = calculDirection(x, y, centreCol, centreRow, edges);
            if(relative_dir == NO) {
                continue;
            }

            Direction realDir = rotateDirection(locateDir, relative_dir);
            if(realDir == BOTTOM_LEFT) {
                bottom_left_relative_dir = relative_dir;
                removeRepeat(bottom_left, encodePoints, x, y);
                setP1P2(relative_dir, bottom_left_p1, bottom_left_p2, kernelPoints);
            } else if(realDir == BOTTOM_RIGHT) {
                bottom_right_relative_dir = relative_dir;
                removeRepeat(bottom_right, encodePoints, x, y);
                setP1P2(relative_dir, bottom_right_p1, bottom_right_p2, kernelPoints);
            }else if (realDir == TOP_RIGHT) {
                top_right_relative_dir = relative_dir;
                removeRepeat(top_right, encodePoints, x, y);
                setP1P2(relative_dir, top_right_p1, top_right_p2, kernelPoints);
            }
        }
    }

    int bottom_left_value = decodeArea(bottom_left, bottom_left_p1, bottom_left_p2, kernelPoints[5], bottom_left_relative_dir);

    int bottom_right_value = decodeArea(bottom_right, bottom_right_p1, bottom_right_p2, kernelPoints[5], bottom_right_relative_dir);

    int top_right_value = decodeArea(top_right, top_right_p1, top_right_p2, kernelPoints[5], top_right_relative_dir);

    if(bottom_left_value > 15 || bottom_right_value > 15 || bottom_right_value > 15) {
        return 0;//error
    }

    result = (top_right_value << 8) + (bottom_right_value << 4) + bottom_left_value;

    //画轮廓及其质心并显示
//    Mat drawing = Mat::zeros( cannyMat.size(), CV_8UC3 );
//    for( int i = 0; i< (int)contours.size(); i++ ) {
//        Scalar color = Scalar( 255, 0, 0);
//        drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
//        rectangle(drawing, boundingRect(contours.at(i)), cvScalar(0,255,0));
//    }
//    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
//    imshow( "Contours", drawing );

    return result;
}

int Dector::decodeArea(vector<Point> encodePoints, Point& p1, Point& p2, Point& p5, Direction relative_dir) {
    int result = 0;
    int count = encodePoints.size();
    if(count == 4) {
        return 15;
    } else if(count == 0) {
        return 0;
    } else if(count > 4) {//error
        return 16;
    }
    int tmpValues[4] = {0, 0, 0, 0};
    int index = 0;
    for(vector<Point>::iterator it = encodePoints.begin(); it != encodePoints.end(); it++) {
        int tmp = judgePosition(*it, p1, p2, relative_dir);
        tmpValues[index] = tmp;
        index++;
        result += tmp;
    }

    if((result == 2 && count == 1) || (result == 13 && count == 3)) {
        return result;
    }

    if(result & 2 == 2 && count == 2) {
        int value_2_index = 0;
        int value_8_index = 0;
        for(int i = 0; i < 4; i++) {
            if(tmpValues[i] != 0) {
                if(tmpValues[i] == 2) {
                    value_2_index = i;
                } else {
                    value_8_index = i;
                }
            }
        }
        double angle = calculateAngle(p5, encodePoints[value_8_index], encodePoints[value_2_index]);
        if(abs(180 - angle) <= SAME_LINE_ANGLE) {
            result = 10;
        }
    }
    return result;
}

int Dector::judgePosition( Point & p,Point & p1, Point & p2, Direction relative_dir) {
    int same_line_gap = 0;
    if(p.y < imageRows/2) {
        same_line_gap = SAME_LINE_GAP_1;
    } else {
        same_line_gap = SAME_LINE_GAP_2;
    }
    int result = 0;
    double angle = calculateAngle(p1, p2, p);
    if(abs(180 - angle) <= SAME_LINE_ANGLE) {
        result = 2;
    } else {
        if((abs(p.x - p1.x) <= same_line_gap && abs(p.y - p2.y) <= same_line_gap)
                || (abs(p.x - p2.x) <= same_line_gap && abs(p.y - p1.y) <= same_line_gap)){
            result = 8;
        } else {
            if(relative_dir == TOP_RIGHT || relative_dir == BOTTOM_LEFT) {
                if(abs(p.y - p2.y) <= same_line_gap || abs(p.y - p1.y) <= same_line_gap){
                    result = 4;
                } else if(abs(p.x - p1.x) <= same_line_gap || abs(p.x - p2.x) <= same_line_gap) {
                    result = 1;
                }
            }
            if(relative_dir == BOTTOM_RIGHT || relative_dir == TOP_LEFT) {
                if(abs(p.y - p2.y) <= same_line_gap || abs(p.y - p1.y) <= same_line_gap){
                    result = 1;
                } else if(abs(p.x - p1.x) <= same_line_gap || abs(p.x - p2.x) <= same_line_gap) {
                    result = 4;
                }
            }
        }
    }
    return result;
}

double Dector::calculateAngle(Point & p1, Point & p2, Point & p3){
    int x1 = p1.x - p3.x;
    int y1 = p1.y - p3.y;
    int x2 = p2.x - p3.x;
    int y2 = p2.y - p3.y;
    double value = (x1 * x2 + y1 * y2)
            /sqrt((pow(x1, 2) + pow(y1,2))*(pow(x2, 2) + pow(y2, 2)));
    return acos(value)*180/M_PI;
}

void Dector::perspective(Mat& src) {
    vector<Point> not_a_rect_shape;
    not_a_rect_shape.push_back(Point(0,0));
    not_a_rect_shape.push_back(Point(src.cols-1,0));
    not_a_rect_shape.push_back(Point(0,src.rows-1));
    not_a_rect_shape.push_back(Point(src.cols-1, src.rows-1));

    //topLeft, topRight, bottomRight, bottomLeft
    Point2f src_vertices[4];
    src_vertices[0] = not_a_rect_shape[0];
    src_vertices[1] = not_a_rect_shape[1];
    src_vertices[2] = not_a_rect_shape[2];
    src_vertices[3] = not_a_rect_shape[3];

    Point2f dst_vertices[4];
    dst_vertices[0] = Point(src.cols*0.1, src.rows*0);
    dst_vertices[1] = Point(src.cols*0.9,0);
    dst_vertices[2] = Point(src.cols*0.26,src.rows);
    dst_vertices[3] = Point(src.cols*0.66,src.rows);


    Mat warpMatrix = getPerspectiveTransform(src_vertices, dst_vertices);
    Mat rotated;
    warpPerspective(src, rotated, warpMatrix, rotated.size(), INTER_LINEAR, BORDER_CONSTANT);

    src = rotated;
}


vector<Point> Dector::scanImageEfficiet(Mat & image) {
    vector<Point> points;
    int iChannels = image.channels();
    imageRows = image.rows;
    int iCols = image.cols * iChannels;
    imageCols = image.cols;
//    cout << "imageCols:" << imageCols << endl;
//    cout << "imageRows:" << imageRows << endl;
    bool continuous = image.isContinuous();
    // check if the image data is stored continuous
    if (continuous) {
        iCols *= imageRows;
    } else {
        cout << "image store is not continuous!!!" << endl;
        return points;
    }

    uchar* p = image.ptr<uchar>(0);
    int count = 0;
    int rows = 0;
    bool whiteLine = true;
    int lineWidth = 0;
    vector<Line> lines;

    for (int j = 0; j < iCols; j++) {
        if (continuous && count == imageCols) {//到达行首
            count = 0;
            rows++;
            lineWidth = 0;
//                if (rows % 2 == 1) {//隔行遍历
//                    j += cols;
//                    rows++;
//                }
        }

        if ((whiteLine && p[j] == 255) || (!whiteLine && p[j] == 0)) {
            lineWidth++;
        } else if (whiteLine && p[j] == 0) {
            if (lineWidth >= LINE_WIDTH_MIN && lineWidth <= LINE_WIDTH_MAX) {//found a white line
                int row_1 = (j-lineWidth)/imageCols;
                int row_2 = (j-1)/imageCols;
                if(row_1 == row_2) {//可以去掉这个校验，因为加了上面lineWidth = 0;
                    Line line = {row_1, (j - lineWidth)%imageCols, (j-1)%imageCols,lineWidth, false};
                    lines.push_back(line);
                    if(debug) {
                        p[line.row*imageCols + line.startCol] = 150;
                        p[line.row*imageCols + line.endCol] = 150;
                    }
                } else {
                    cout << "bu zai tong yi hang!!!!!!!!!!!!!!!!!!!! " << endl;
                }
            }

            whiteLine = false;
            lineWidth = 1;
        } else if (!whiteLine && p[j] == 255) {
            if (lineWidth >= LINE_WIDTH_MIN && lineWidth <= LINE_WIDTH_MAX) {//found a black line
                int row_1 = (j-lineWidth)/imageCols;
                int row_2 = (j-1)/imageCols;

                if(row_1 == row_2) {
                    Line line = {row_1, (j - lineWidth)%imageCols, (j-1)%imageCols, lineWidth, true};
                    lines.push_back(line);
                    if(debug) {
                        p[line.row*imageCols + line.startCol] = 50;
                        p[line.row*imageCols + line.endCol] = 50;
                    }
                } else {
                    cout << "bu zai tong yi hang!!!!!!!!!!!!!!!!!! " << endl;
                }
            }

            whiteLine = true;
            lineWidth = 1;
        }
        count++;
    }
    vector<Kernel> kernels;
    analyseLines(lines, kernels, p);

    if(kernels.size() > 10) {
        points = analyseKernels(kernels);
    }

    return points;
}

void Dector::analyseLines(vector<Line> & lines, vector<Kernel> &kernels, uchar* p) {
    int step = 0;
    int size = lines.size();
    int lastLineWidth = 0;
    bool twoTimes = false;
    int startWhiteLineWidth = 0;
    for (int i = 0; i < size; i++) {
        if(step > 0) {
            switch (step) {
            case 1:
                if(!lines[i].isBlack && abs(lines[i].width - lastLineWidth) <= ONE_TIME_THRESHOLD) {
                    step++;
                    lastLineWidth = lines[i].width;
                    startWhiteLineWidth = lines[i].width;
                } else {
                    i = i - step + 1;
                    step = 0;
                }
                break;
            case 2:
                if(lines[i].isBlack) {
                    if(abs(lines[i].width - 2*lastLineWidth) <= abs(lines[i].width - 3*lastLineWidth)
                            && lines[i].width >= lastLineWidth) {
                        step++;
                        lastLineWidth = lines[i].width;
                        twoTimes = true;
                        break;
                    }else if (abs(lines[i].width - 2*lastLineWidth) > abs(lines[i].width - 3*lastLineWidth)
                              && lines[i].width > lastLineWidth){
                        step++;
                        lastLineWidth = lines[i].width;
                        twoTimes = false;
                        break;
                    }
                }
                i = i - step + 1;
                step = 0;
                break;
            case 3:
                if(!lines[i].isBlack) {
                    if(twoTimes && abs(2*lines[i].width - lastLineWidth) <= abs(3*lines[i].width - lastLineWidth)
                            && lastLineWidth >= lines[i].width && lines[i].width < TWOTIMES_WHITE_LINE_WIDTH_MAX
                            && startWhiteLineWidth < TWOTIMES_WHITE_LINE_WIDTH_MAX) {
                        step++;
                        lastLineWidth = lines[i].width;
                        break;
                     }else if(!twoTimes && abs(3*lines[i].width - lastLineWidth) <= abs(2*lines[i].width - lastLineWidth)
                              && lastLineWidth > lines[i].width) {
                        step++;
                        lastLineWidth = lines[i].width;
                        break;
                    }
                }
                i = i - step + 1;
                step = 0;
                break;
            case 4:
                if(lines[i].isBlack && abs(lines[i].width - lastLineWidth) <= ONE_TIME_THRESHOLD) {
                    step++;
                    lastLineWidth = lines[i].width;
                } else {
                    i = i - step + 1;
                    step = 0;
                }
                break;
            default:
                break;
            }
        }

        if(lines[i].isBlack && lines[i].width < 16 && step == 0) {
            step++;
            lastLineWidth = lines[i].width;
        }

        if(step == 5) {//找到一个路标
            step = 0;
//            int mid_1 = lines[i-4].startCol + (lines[i].startCol - lines[i-4].startCol)/2;
//            int mid_2 = lines[i-4].endCol + (lines[i].endCol - lines[i-4].endCol)/2;
            int mid_3 = lines[i-2].startCol + (lines[i-2].endCol - lines[i-2].startCol)/2;
            int mid = mid_3;//(mid_1 + mid_2 + mid_3)/3;

            if(twoTimes) {
                Kernel kernel = {lines[i].row, mid, 2};//type=2,导航标识
                kernels.push_back(kernel);
//                p[kernel.row*imageCols + kernel.col] = 80;
            } else {
                Kernel kernel = {lines[i].row, mid, 1};
                kernels.push_back(kernel);//type=1,定位标识
//                p[kernel.row*imageCols + kernel.col] = 120;
            }
        }
    }
}


vector<Point> Dector::analyseKernels(vector<Kernel> &kernels) {
    vector<Point> kernelPoints;
    vector<Kernel> locateKernels;
    vector<Kernel> navigationKernels;

    for(vector<Kernel>::iterator it = kernels.begin(); it != kernels.end(); ++it) {
        if(it->type == 1) {
            locateKernels.push_back(*it);
        }
    }

    analyseLocationKernels(kernels, locateKernels, navigationKernels, kernelPoints);

    if(isValidPoint(kernelPoints[0]) && navigationKernels.size() > 2) {
        analyseNavigationKernels(navigationKernels, kernelPoints);
    }
    if(kernelPoints.size() < 6) {
        int size = kernelPoints.size();
        for(int i = 0; i < 6 - size; i++) {
            kernelPoints.push_back(Point{0, 0});
        }
    }
    return kernelPoints;
}

void Dector::analyseLocationKernels(vector<Kernel>& kernels, vector<Kernel>& locateKernels
                                    , vector<Kernel>& navigationKernels, vector<Point>& kernelPoints) {
    int low = 0;
    int high = imageCols;
    findKernelsCentre(&low, &high, locateKernels);
    low = low - 5;
    high = high + 5;
//    cout << "low:" << low << "high:" << high << endl;

    vector<Kernel> newLocateKernels;
    for(vector<Kernel>::iterator it = kernels.begin(); it != kernels.end(); ++it) {
        if(it->col >= low && it->col <= high && it->type == 1) {
            newLocateKernels.push_back(*it);
        } else if((it->col <= low || it->col >= high) && it->type != 3) {
            navigationKernels.push_back(*it);
        }
    }
    kernelPoints.push_back(denoiseAndCentralization(newLocateKernels));
}

void Dector::analyseNavigationKernels(vector<Kernel>& navigationKernels, vector<Point>& kernelPoints) {
    int low = 0;
    int high = imageCols;
    findKernelsCentre(&low, &high, navigationKernels);
    low = low - 15;
    high = high + 15;
//    cout << "low_2:" << low << "high_2:" << high << endl;

    vector<Kernel> rightNaviKernels;
    vector<Kernel> leftNaviKernels;
    vector<Kernel> midNaviKernels;
    for(vector<Kernel>::iterator it = navigationKernels.begin(); it != navigationKernels.end(); ++it){
        if(abs(it->row - kernelPoints[0].y) <= MARKS_GAP) {
            if(it->col < low) {
                leftNaviKernels.push_back(*it);
            } else if(it->col > high) {
                rightNaviKernels.push_back(*it);
            } else {
                midNaviKernels.push_back(*it);
            }
        }
    }
    Point p1 = denoiseAndCentralization(leftNaviKernels);
    Point p2 = denoiseAndCentralization(rightNaviKernels);
    kernelPoints.push_back(p1);
    kernelPoints.push_back(p2);

    int centreRow = 0;
    if((p1.x != 0 && p1.y != 0) || (p2.x != 0 && p2.y != 0)) {
        if(p1.y != 0 && p2.y != 0) {
            centreRow = (p1.y + p2.y)/2;
        } else if(p1.y != 0) {
            centreRow = p1.y;
        } else if(p2.y != 0) {
            centreRow = p2.y;
        } else {
            centreRow = 0;
        }
    }

    if(centreRow == 0) {
        int sum_row = 0;
        for(vector<Kernel>::iterator it = midNaviKernels.begin(); it != midNaviKernels.end(); ++it){
            sum_row += it->row;
        }
        if(midNaviKernels.size() > 0) {
            centreRow = sum_row/midNaviKernels.size();
        } else {
            return;
        }
    }

    vector<Kernel> upKernels;
    vector<Kernel> downKernels;
    vector<Kernel> centreKernels;
    for(vector<Kernel>::iterator it = midNaviKernels.begin(); it != midNaviKernels.end(); ++it){
        if(it->row < centreRow - 15) {
            upKernels.push_back(*it);
        } else if(it->row > centreRow + 15) {
            downKernels.push_back(*it);
        } else {
            centreKernels.push_back(*it);
        }
    }
    kernelPoints.push_back(denoiseAndCentralization(upKernels));
    kernelPoints.push_back(denoiseAndCentralization(downKernels));
    kernelPoints.push_back(denoiseAndCentralization(centreKernels));

    if(!isValidPoint(kernelPoints[5]) && isValidPoint(kernelPoints[1]) && isValidPoint(kernelPoints[2])
            && isValidPoint(kernelPoints[3]) && isValidPoint(kernelPoints[4])) {
        kernelPoints[5].x = (kernelPoints[3].x + kernelPoints[4].x)/2;
        kernelPoints[5].y = (kernelPoints[1].y + kernelPoints[2].y)/2;
    }
}

Point Dector::denoiseAndCentralization(vector<Kernel> & kernels){
    Point p{0, 0};
    if(kernels.size() == 0) {
        return p;
    }
    bool col_erase = true;
    bool row_erase = true;
    while (col_erase || row_erase) {

        if(col_erase) {
            col_erase = false;
            int sum_col = 0;
            for(vector<Kernel>::iterator it = kernels.begin(); it != kernels.end(); ++it) {
                sum_col += it->col;
            }
            int mean_col = sum_col/kernels.size();
            int count_low = 0;
            int count_high = 0;
            for(vector<Kernel>::iterator it = kernels.begin(); it != kernels.end(); ++it){
                if(it->col < mean_col) {
                    count_low++;
                } else if(it->col > mean_col) {
                    count_high++;
                }
            }
            for(vector<Kernel>::iterator it = kernels.begin(); it != kernels.end();){
                if((it->col < mean_col && count_low < count_high)
                        || (it->col > mean_col && count_low > count_high)){
                    if(abs(it->col - mean_col) > 3) {
                        it = kernels.erase(it);
                        col_erase = true;
                    } else {
                        ++it;
                    }
                } else {
                    ++it;
                }
            }
        }

        if(row_erase) {
            row_erase = false;
            int sum_row = 0;
            for(vector<Kernel>::iterator it = kernels.begin(); it != kernels.end(); ++it) {
                sum_row += it->row;
            }
            int mean_row = sum_row/kernels.size();
            int count_up = 0;
            int count_down = 0;
            for(vector<Kernel>::iterator it = kernels.begin(); it != kernels.end(); ++it){
                if(it->row < mean_row) {
                    count_up++;
                } else if(it->row > mean_row) {
                    count_down++;
                }
            }
            for(vector<Kernel>::iterator it = kernels.begin(); it != kernels.end();){
                if((it->row < mean_row && count_up < count_down)
                        || (it->row > mean_row && count_up > count_down)){
                    if(abs(it->row - mean_row) > 5) {
                        it = kernels.erase(it);
                        row_erase = true;
                    } else {
                        ++it;
                    }
                } else {
                    ++it;
                }
            }
        }

    }

    int sum_col = 0, sum_row = 0;
    for(vector<Kernel>::iterator it = kernels.begin(); it != kernels.end(); ++it){
        sum_col += it->col;
        sum_row += it->row;
    }
    int size = kernels.size();
    if(size >= KERNELS_COUNT) {
        p = {sum_col/size, sum_row/size};
    }
    return p;
}

void Dector::findKernelsCentre(int* low, int* high, vector<Kernel> & kernels) {
    while(*high - *low > 1) {
        int mid = (*high + *low)/2;
        uchar lowCount = 0;
        uchar highCount = 0;
        for(vector<Kernel>::iterator it = kernels.begin(); it != kernels.end(); ++it) {
            if(it->col >= *low && it->col <= *high) {
                if(it->col < mid) {
                    lowCount++;
                } else {
                    highCount++;
                }
            }
        }
        if(lowCount < highCount) {
            *low = mid;
        } else if (lowCount > highCount) {
            *high = mid;
        } else {
            *low = mid - 3;
            *high = mid + 3;
            break;
        }
        lowCount = 0;
        highCount =0;
    }
}

Point Dector::calculateMeanPointAndPush(uchar& count, int& sum_col, int& sum_row
                                        , vector<Point>& kernelPoints) {
    Point p;
    if(count < KERNELS_COUNT) {
        p = {0, 0};
    } else {
        p = {sum_col/count, sum_row/count};
    }
    kernelPoints.push_back(p);
    return p;
}

bool Dector::isValidPoint(Point& p) {
    if(p.x == 0 && p.y == 0) {
        return false;
    } else {
        return true;
    }
}

vector<int> Dector::calculateEdges(vector<Point>& points) {
    vector<int> edges;
    if(points.size() < 6) {
        return edges;
    }

    if(isValidPoint(points[1])) {
        edges.push_back(points[1].x);
    } else {
        edges.push_back(0);
    }
    if(isValidPoint(points[2])) {
        edges.push_back(points[2].x);
    } else {
        edges.push_back(0);
    }
    if(isValidPoint(points[3])) {
        edges.push_back(points[3].y);
    } else {
        edges.push_back(0);
    }
    if(isValidPoint(points[4])) {
        edges.push_back(points[4].y);
    } else {
        edges.push_back(0);
    }
    return edges;
}


Dector::Direction Dector::calculDirection(int x, int y, int centreCol, int centreRow, vector<int> edges) {
    int high = edges[3] - edges[2];
    int width = edges[1] - edges[0];
    int col_gap = 0;
    int row_gap = 0;
    if(centreRow <= imageRows/3) {
        col_gap = EDGE_CENTRE_COL_GAP_1;
        row_gap = EDGE_CENTRE_ROW_GAP_1;
    } else if(centreRow > imageRows/3 && centreRow <= 2*imageRows/3) {
        col_gap = EDGE_CENTRE_COL_GAP_2;
        row_gap = EDGE_CENTRE_ROW_GAP_2;
    } else if(centreRow > 2*imageRows/3) {
        col_gap = EDGE_CENTRE_COL_GAP_3;
        row_gap = EDGE_CENTRE_ROW_GAP_3;
    }


    int row_thread = 0;
    int col_thread = 0;
    if(high > 170) {
        row_thread = 30;
    } else if(high > 150 && high <= 170) {
        row_thread = 25;
    } else if(high > 125 && high <= 150) {
        row_thread = 18;
    } else if(high <= 125) {
        row_thread = 10;
    }
    if(width > 160) {
        col_thread = 28;
    } else if(width <= 160 && width > 140) {
        col_thread = 24;
    } else if(width <= 140 && width > 120) {
        col_thread = 18;
    } else {
        col_thread = 14;
    }

    if(x >  edges[0] - col_gap && x < centreCol - col_thread && y < edges[3] + row_gap && y > centreRow + row_thread) {//bootom left
        return BOTTOM_LEFT;
    } else if(x <  edges[1] + col_gap && x > centreCol + col_thread && y < edges[3] + row_gap && y > centreRow + row_thread) {//bootom right
        return BOTTOM_RIGHT;
    } else if(x >  edges[0] - col_gap && x < centreCol - col_thread && y > edges[2] - row_gap && y < centreRow - row_thread) {//top left
        return TOP_LEFT;
    } else if (x <  edges[1] + col_gap && x > centreCol + col_thread && y > edges[2] - row_gap && y < centreRow - row_thread) {//bootom left
       return TOP_RIGHT;
    } else {
        return NO;
    }
}

Dector::Direction Dector::rotateDirection(Direction locateDir, Direction direction) {
    Direction realDir;
    if(locateDir == TOP_LEFT) {
        realDir = direction;
    } else {
        int value = direction + (TOP_LEFT - locateDir);
        if(value > 3) {
            value -= 4;
        }
        if( value == 0) {
            realDir = BOTTOM_LEFT;
        } else if(value == 1) {
            realDir = BOTTOM_RIGHT;
        } else if(value == 2) {
            realDir = TOP_RIGHT;
        } else if(value == 3) {
            realDir = TOP_LEFT;
        }
    }
    return realDir;
}

void Dector::removeRepeat(vector<Point> & encodes, vector<Point> & encodePoints, int x, int y){
    bool repeat = false;
    for(vector<Point>::iterator it = encodes.begin(); it != encodes.end(); ++it) {
        if((abs(x - it->x) + abs(y - it->y)) < 8) {
            repeat = true;
            break;
        }
    }
    if(!repeat){
        encodePoints.push_back(Point{x, y});
        encodes.push_back(Point{x, y});
    }
}

void Dector::setP1P2(Direction direction, Point& P1, Point& P2, vector<Point> &kernelPoints) {
    if(direction == BOTTOM_LEFT) {
        P1 = kernelPoints[1];
        P2 = kernelPoints[4];
    } else if(direction == BOTTOM_RIGHT) {
        P1 = kernelPoints[2];
        P2 = kernelPoints[4];
    } else if(direction == TOP_LEFT) {
        P1 = kernelPoints[1];
        P2 = kernelPoints[3];
    } else if(direction == TOP_RIGHT){
        P1 = kernelPoints[2];
        P2 = kernelPoints[3];
    }
}
 void Dector::myPutText(string text, Mat src, int x, int y){
     int font_face = cv::FONT_HERSHEY_COMPLEX;
     double font_scale = 1;
     int thickness = 2;
     Point origin;
     origin.x = x;
     origin.y = y;
     putText(src, text, origin, font_face, font_scale, cv::Scalar(0, 255, 255), thickness, 3, 0);
 }
