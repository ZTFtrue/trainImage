//============================================================================
// Name        : 决策树.cpp
// Author      : ztftrue
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <opencv2/opencv.hpp>
#include "ImageTools.h"
#include <node6/node.h>

using namespace std;
using namespace cv;
using namespace v8;


int main() {
    Mat src = imread("img/fish.jpg", IMREAD_COLOR);
    ImageTools imageTools;
    imageTools.trainPic(src, "fish");
//    cout << imageTools.recognition(src) << endl;
    return 0;
}


