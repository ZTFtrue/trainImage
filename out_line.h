//
// Created by ztftrue on 17-10-10.
// 轮廓处理
//

#ifndef OPENCV_TRAIN_OUT_LINE_H
#define OPENCV_TRAIN_OUT_LINE_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "Distance.h"
#include "dist/json/json.h"
#include <sys/stat.h>

using namespace cv;
using namespace std;

class out_line {
public:

    out_line();

    ~out_line();

    bool trainOut(Size
                  size,
                  vector<Point> contour, string
                  outLineName);

    string reOut(Size
               size,
               vector<Point> contour
    );
};


#endif //OPENCV_TRAIN_OUT_LINE_H
