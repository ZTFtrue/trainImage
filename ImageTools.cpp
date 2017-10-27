//
// Created by ztftrue on 17-10-10.
//

#include "ImageTools.h"
#include "out_line.h"

RNG rng1(12345);


struct CombinationOutline {//数据轮廓点
    bool one[4];
    double angle;
};

struct reCombinationOutline {//数据轮廓点
    bool one[4];
    double angle;
    double area;
    double arc;
    string nameOu;
};
struct DisContour {
    vector<Point> contour;
    int distance = 0;
    Point point;
};
//Mat dst, detected_edges;
int lowThreshold = 255;
int ratio = 2;
int kernel_size = 5;
const char *window_name = "Edge Map";
//RNG rng(12345);
out_line outLineObject;
struct TrainOutline {
    Point point;//中心点
    std::string names;
    int area;
    int arcLength;
    vector<Point> contour;
};

bool SortByPoint(const TrainOutline &v1, const TrainOutline &v2) {
    if (v1.point.x == v2.point.x) {
        return v1.point.y < v2.point.y;
    }
    return v1.point.x < v2.point.x;

}


ImageTools::ImageTools() {
    outLineObject = out_line();
}

ImageTools::~ImageTools() {
//    delete (outLineObject);
//    outLineObject = nullptr;
}

CombinationOutline getAngle(Point point1, Point point2, Point point3) {
    double a = (sqrt(pow((point1.x - point2.x), 2) + pow((point1.y - point2.y), 2)));
    double b = (sqrt(pow((point2.x - point3.x), 2) + pow((point2.y - point3.y), 2)));
    double c = (sqrt(pow((point3.x - point1.x), 2) + pow((point3.y - point1.y), 2)));

    CombinationOutline combinationOutline;
    combinationOutline.one[0] = point1.x > point2.x;
    combinationOutline.one[1] = point1.y > point2.y;
    combinationOutline.one[2] = point3.x > point2.x;
    combinationOutline.one[3] = point3.y > point2.y;


    combinationOutline.angle = (pow(a, 2) + pow(b, 2) - pow(c, 2)) / (2 * a * b);
    return combinationOutline;
}

/**
 * 训练轮廓 ，其实就是把轮廓排序 ，没有使用神经网络
 * 以后会弃用
 * @param src1
 * @param name 图片名称
 * @return
 */
bool ImageTools::trainPic(Mat src1, string name) {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    vector<TrainOutline> TrainOutlines;
    Mat canny_output;
    Mat detected_edges, src, tmp, dst;
    blur(src1, src, Size(3, 3));
    cvtColor(src, src, COLOR_BGR2GRAY);
    tmp = src;
    dst = tmp;
    for (int i = 0; i < 2; ++i) {
        pyrDown(tmp, dst, Size(tmp.cols / 2, tmp.rows / 2));
        tmp = dst;
    }

    Canny(dst, canny_output, 255, 255 * ratio,
          3);
    findContours(canny_output, contours, hierarchy, RETR_TREE,
                 CHAIN_APPROX_SIMPLE, Point(0, 0));
    if (contours.size() == 1) {//暂时不处理
        //直接训练轮廓
        if (outLineObject.reOut(src.size(), contours[0]).empty()) {//如果能识别

        } else {//如果不能

        }
        return false;
    }
    int allArea = (src.rows * src.cols) / 100;
    int allArc = ((src.rows + src.cols) * 2) / 100;
    //Hu不变矩在图像旋转、缩放、平移等操作后，仍能保持矩的不变性，
    //所以有时候用Hu不变距更能识别图像的特征
    //计算轮廓矩
    vector<Moments> mu(contours.size());
    for (size_t i = 0; i < contours.size(); i++) {
        mu[i] = moments(contours[i], false);
    }
    string a = "image";
    mkdir(a.data(), S_IRWXU);
    DisContour arcLengthStruct;
    DisContour contourAreaStruct;
    for (size_t i = 0; i < contours.size(); i++) {
        Point2d point = Point2d((mu[i].m10 / mu[i].m00),
                                (mu[i].m01 / mu[i].m00)); //计算轮廓的质心   中心
        int arc = static_cast<int &&>(fabs(arcLength(contours[i], true)));
        int area = static_cast<int &&>(fabs(contourArea(contours[i], true)));
        if (arc > allArc || area > allArea) {
            if (arcLengthStruct.distance < arc) {
                arcLengthStruct.distance = arc;
                arcLengthStruct.point = point;
                arcLengthStruct.contour.clear();
                arcLengthStruct.contour.insert(arcLengthStruct.contour.end(), contours[i].begin(), contours[i].end());
            }
            if (contourAreaStruct.distance < area) {
                contourAreaStruct.distance = area;
                contourAreaStruct.point = point;
                contourAreaStruct.contour.clear();
                contourAreaStruct.contour.insert(contourAreaStruct.contour.end(), contours[i].begin(),
                                                 contours[i].end());
            }
            TrainOutline trainOutline;
            trainOutline.point = point;
            trainOutline.arcLength = arc;
            trainOutline.area = area;
            trainOutline.contour.insert(trainOutline.contour.end(), contours[i].begin(), contours[i].end());
            TrainOutlines.push_back(trainOutline);
        }
    }

    int center = static_cast<int>(pointPolygonTest(contourAreaStruct.contour,
                                                   Point2f(src.rows / 2, src.cols / 2), false));
    a = a + "/" + to_string(center);
    mkdir(a.data(), S_IRWXU);
    if (center == 1) {
        //最大轮廓的外形
        a = a + "/AreaInCenter";
        mkdir(a.data(), S_IRWXU);
    } else if (center == 2) { //不在中间第一个和第二个不相等
        //最大轮廓的外形
        a = a + "/AreaNoCenter";
        mkdir(a.data(), S_IRWXU);
    }
    //最大轮廓和最大面积是否是同一个
    if (contourAreaStruct.point.x == arcLengthStruct.point.x &&
        contourAreaStruct.point.y == arcLengthStruct.point.y) {//是
        a = a + "/AreaAndLength";
        mkdir(a.data(), S_IRWXU);
    } else {
        a = a + "/AreaNoLength";
        mkdir(a.data(), S_IRWXU);
        int val = static_cast<int>(pointPolygonTest(contours[0],
                                                    Point2f(src.rows / 2, src.cols / 2), false));
        //按边长  存储轮廓外形  未知则存储名称+未知@1  同时训练轮廓
        if (val == 1) {
            //最大轮廓的外形
            a = a + "/LengthInCenter";
            mkdir(a.data(), S_IRWXU);
        } else {//不在中间
            a = a + "/LengthEqualityCenter";
            mkdir(a.data(), S_IRWXU);
        }
    }

    //如果轮廓为识别则记录位置，以后更改
    std::sort(TrainOutlines.begin(), TrainOutlines.end(), SortByPoint);
    Json::Value root;
    Json::Value object;
    Json::Value arrayObj;
    for (size_t i = 1; i < TrainOutlines.size(); i++) {
        //先进行轮廓识别

        std::string outLineName = outLineObject.reOut(src.size(), TrainOutlines[i].contour);

        if (outLineName.empty()) {//绘制轮廓
            //获取轮廓名，并训练
            Mat drawingW = Mat::zeros(src.size(), CV_8UC3);
            Scalar color = Scalar(rng1.uniform(0, 255), rng1.uniform(0, 255),
                                  rng1.uniform(0, 255));
            vector<vector<Point>> contoursOut;
            contoursOut.push_back(TrainOutlines[i].contour);
            drawContours(drawingW, contoursOut, 0, color, 2, 8, 0, 0, Point());
            imshow(window_name, drawingW);
            waitKey(0);
            string s;
            getline(cin, s, '\n');
            outLineName = s;
            outLineObject.trainOut(src.size(), TrainOutlines[i].contour, outLineName);
        }
        if (TrainOutlines[i].area != 0) {
            object["area"] = TrainOutlines[i - 1].area / TrainOutlines[i].area;
        }
        if (TrainOutlines[i].arcLength != 0) {
            object["arc"] = TrainOutlines[i - 1].arcLength / TrainOutlines[i].arcLength;
        }
        if (i >= 1 && i < TrainOutlines.size()) {
            CombinationOutline combinationOutline = getAngle(TrainOutlines[i - 1].point, TrainOutlines[i].point,
                                                             TrainOutlines[i + 1].point);
            object["angle"] = combinationOutline.angle;
            object["boolax"] = combinationOutline.one[0];
            object["boolay"] = combinationOutline.one[1];
            object["boolbx"] = combinationOutline.one[2];
            object["boolby"] = combinationOutline.one[3];
        }
        object["name"] = outLineName;
        arrayObj.append(object);
    }
    root["data"] = arrayObj;
    root["name"] = name;
    a = a + "/" + to_string(TrainOutlines.size());
    mkdir(a.data(), S_IRWXU);
    std::string out = root.toStyledString();
    std::ofstream ofs;
    string c;
    stringstream ss;
    int k = 0;
    for (;;) {
        ss.str("");
        ss.clear();
        ss << k;
        c = a + "/" + ss.str() + ".json";
        if (!fopen(c.data(), "r")) {
            break;
        }
        k = k + 1;
    }
    ofs.open(c.data()
    );
    ofs << out;
    ofs.close();
    return true;
}

//依然没有使用神经网络
string ImageTools::recognition(Mat src1) {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    vector<TrainOutline> TrainOutlines;
    Mat detected_edges, src, tmp, dst;
    blur(src1, src, Size(3, 3));
    cvtColor(src, src, COLOR_BGR2GRAY);
    tmp = src;
    dst = tmp;
    for (int i = 0; i < 2; ++i) {
        pyrDown(tmp, dst, Size(tmp.cols / 2, tmp.rows / 2));
        tmp = dst;
    }

    Canny(dst, detected_edges, 255, 255 * ratio,
          3);
    findContours(detected_edges, contours, hierarchy, RETR_TREE,
                 CHAIN_APPROX_SIMPLE, Point(0, 0));
    if (contours.size() == 0) {
        //直接训练轮廓

        return "空白";
    }
    if (contours.size() == 1) {
        //直接训练轮廓
        if (outLineObject.reOut(detected_edges.size(), contours[0]).empty()) {//如果能识别

        } else {//如果不能

        }
        return "";
    }

    int allArc = ((src.rows + src.cols) * 2) / 400;
    //Hu不变矩在图像旋转、缩放、平移等操作后，仍能保持矩的不变性，
    //所以有时候用Hu不变距更能识别图像的特征
    //计算轮廓矩
    vector<Moments> mu(contours.size());
    for (size_t i = 0; i < contours.size(); i++) {
        mu[i] = moments(contours[i], false);
    }
    string a = "image";
    mkdir(a.data(), S_IRWXU);
    DisContour arcLengthStruct;
    DisContour contourAreaStruct;
    for (size_t i = 0; i < contours.size(); i++) {
        Point2d point = Point2d((mu[i].m10 / mu[i].m00),
                                (mu[i].m01 / mu[i].m00)); //计算轮廓的质心   中心
        int arc = static_cast<int &&>(fabs(arcLength(contours[i], true)));
        int area = static_cast<int &&>(fabs(contourArea(contours[i], true)));

        if (contours[i].size() > allArc) {
            if (arcLengthStruct.distance < arc) {
                arcLengthStruct.distance = arc;
                arcLengthStruct.point = point;
                arcLengthStruct.contour.clear();
                arcLengthStruct.contour.insert(arcLengthStruct.contour.end(), contours[i].begin(), contours[i].end());
            }
            if (contourAreaStruct.distance < area) {
                contourAreaStruct.distance = area;
                contourAreaStruct.point = point;
                contourAreaStruct.contour.clear();
                contourAreaStruct.contour.insert(contourAreaStruct.contour.end(), contours[i].begin(),
                                                 contours[i].end());
            }
            TrainOutline trainOutline;
            trainOutline.point = point;
            trainOutline.arcLength = arc;
            trainOutline.area = area;
            trainOutline.contour.insert(trainOutline.contour.end(), contours[i].begin(), contours[i].end());
            TrainOutlines.push_back(trainOutline);
        }
    }

    int center = static_cast<int>(pointPolygonTest(contourAreaStruct.contour,
                                                   Point2f(src.rows / 2, src.cols / 2), false));
    a = a + "/" + to_string(center);
    if (center == 1) {
        //最大轮廓的外形
        a = a + "/AreaInCenter";
    } else if (center == 2) { //不在中间第一个和第二个不相等
        //最大轮廓的外形
        a = a + "/AreaNoCenter";
    }
    //最大轮廓和最大面积是否是同一个
    if (contourAreaStruct.point.x == arcLengthStruct.point.x &&
        contourAreaStruct.point.y == arcLengthStruct.point.y) {//是
        a = a + "/AreaAndLength";
    } else {
        a = a + "/AreaNoLength";
        int val = static_cast<int>(pointPolygonTest(contours[0],
                                                    Point2f(src.rows / 2, src.cols / 2), false));
        //按边长  存储轮廓外形  未知则存储名称+未知@1  同时训练轮廓
        if (val == 1) {
            //最大轮廓的外形
            a = a + "/LengthInCenter";
        } else {//不在中间
            a = a + "/LengthEqualityCenter";
        }
    }

    //如果轮廓为识别则记录位置，以后更改
    std::sort(TrainOutlines.begin(), TrainOutlines.end(), SortByPoint);


    vector<reCombinationOutline> comB;
    for (size_t i = 1; i < TrainOutlines.size(); i++) {
        //先进行轮廓识别
        reCombinationOutline reCombinationOutline1;
        std::string outLineName = outLineObject.reOut(detected_edges.size(), TrainOutlines[i].contour);
        if (outLineName.empty()) {//绘制轮廓并存储轮廓

        }

        if (TrainOutlines[i].area != 0) {
            reCombinationOutline1.area = TrainOutlines[i - 1].area / TrainOutlines[i].area;
        }
        if (TrainOutlines[i].arcLength != 0) {
            reCombinationOutline1.arc = TrainOutlines[i - 1].arcLength / TrainOutlines[i].arcLength;
        }
        if (i >= 1 && i < TrainOutlines.size()) {
            CombinationOutline combinationOutline = getAngle(TrainOutlines[i - 1].point, TrainOutlines[i].point,
                                                             TrainOutlines[i + 1].point);
            reCombinationOutline1.angle = combinationOutline.angle;
            reCombinationOutline1.one[0] = combinationOutline.one[0];
            reCombinationOutline1.one[1] = combinationOutline.one[1];
            reCombinationOutline1.one[2] = combinationOutline.one[2];
            reCombinationOutline1.one[3] = combinationOutline.one[3];
        }
        reCombinationOutline1.nameOu = outLineName;
        comB.push_back(reCombinationOutline1);
    }


    Json::Value root;
    Json::Value object;
    Json::Value arrayObj;
    a = a + "/" + to_string(TrainOutlines.size());
    std::string out = root.toStyledString();
    std::ofstream ofs;
    std::ifstream ifs;
    string c;
    stringstream ss;
    int k = 0;
    int test = 0;

    for (;;) {
        ss.str("");
        ss.clear();
        ss << k;
        c = a + "/" + ss.str() + ".json";
        if (fopen(c.data(), "r")) {
            //开始识别
            ifs.open(c.data());
            ifs >> root;
            arrayObj = root["data"];
            string name = root["name"].asString();
            int comI = 0;
            bool isRe = true;
            for (int j = 0; j < arrayObj.size(); ++j) {//对比
                object = arrayObj[j];
                reCombinationOutline combinationOutline;
                combinationOutline.one[0] = object["boolax"].asBool();
                combinationOutline.one[1] = object["boolay"].asBool();
                combinationOutline.one[2] = object["boolbx"].asBool();
                combinationOutline.one[3] = object["boolby"].asBool();
                combinationOutline.angle = object["angle"].asDouble();
                if (comB[comI].nameOu == object["name"].asString() &&
                    comB[comI].area == object["area"].asDouble() &&
                    comB[comI].area == object["arc"].asDouble() &&
                    comB[comI].one[0] == object["boolax"].asDouble() &&
                    comB[comI].one[1] == object["boolay"].asDouble() &&
                    comB[comI].one[2] == object["boolbx"].asDouble() &&
                    comB[comI].one[3] == object["boolby"].asDouble()) {
                    comI = comI + 1;
                    test = 0;
                } else if (test == 0 &&
                           comB[comI + 1].nameOu == object["name"].asString() &&
                           comB[comI + 1].area == object["area"].asDouble() &&
                           comB[comI + 1].area == object["arc"].asDouble() &&
                           comB[comI + 1].one[0] == object["boolax"].asDouble() &&
                           comB[comI + 1].one[1] == object["boolay"].asDouble() &&
                           comB[comI + 1].one[2] == object["boolbx"].asDouble() &&
                           comB[comI + 1].one[3] == object["boolby"].asDouble()) {
                    test = 1;
                    comI = comI + 2;
                } else {
                    isRe = false;
                    break;
                }

            }
            ifs.close();
            if (isRe) {
                return root["name"].asString();
            }
        }
        test = 0;
        k = k + 1;
        return "fish";
    }

}


