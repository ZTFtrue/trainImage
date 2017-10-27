//
// Created by ztftrue on 17-10-10.
// 图片处理
//

#ifndef OPENCV_TRAIN_TRAIN_H
#define OPENCV_TRAIN_TRAIN_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include "Distance.h"
#include "dist/json/json.h"
#include <sys/stat.h>

using namespace cv;
using namespace std;

class ImageTools {
public:
    ImageTools();

    ~ImageTools();

    bool trainPic(Mat src, string name);
    string recognition(Mat src);
};


#endif //OPENCV_TRAIN_TRAIN_H
